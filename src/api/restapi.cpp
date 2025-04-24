/**
 * @file restapi.cpp
 * @brief Implementation of the REST API wrapper
 */

#include "src/api/restapi.h"
#include <QEventLoop>
#include <QMetaEnum>

RestApi::RestApi(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &RestApi::handleNetworkReply);

    timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    connect(timeoutTimer, &QTimer::timeout,
            this, &RestApi::handleTimeout);
}

RestApi::~RestApi()
{
    cancelRequests();
}

void RestApi::getAllStations()
{
    QUrl url(BASE_URL + STATIONS_ENDPOINT);
    QNetworkRequest request(url);

    networkManager->get(request);
    timeoutTimer->start(TIMEOUT_MS);
}

void RestApi::getStationsByCity(const QString &cityName)
{
    // First get all stations, then filter by city
    connect(this, &RestApi::stationsReceived, this, [this, cityName](const QList<Station> &allStations) {
        QList<Station> filteredStations;
        for (const Station &station : allStations) {
            if (station.getCity().toLower().contains(cityName.toLower())) {
                filteredStations.append(station);
            }
        }

        // Disconnect to avoid multiple signals
        disconnect(this, &RestApi::stationsReceived, this, nullptr);

        // Emit the filtered stations
        emit stationsReceived(filteredStations);
    });

    getAllStations();
}

void RestApi::getSensors(int stationId)
{
    QUrl url(BASE_URL + SENSORS_ENDPOINT + "/" + QString::number(stationId));
    QNetworkRequest request(url);

    networkManager->get(request);
    timeoutTimer->start(TIMEOUT_MS);
}

void RestApi::getMeasurements(int sensorId)
{
    QUrl url(BASE_URL + MEASUREMENTS_ENDPOINT + "/" + QString::number(sensorId));
    QNetworkRequest request(url);

    networkManager->get(request);
    timeoutTimer->start(TIMEOUT_MS);
}

void RestApi::getAirQualityIndex(int stationId)
{
    QUrl url(BASE_URL + AIR_QUALITY_INDEX_ENDPOINT + "/" + QString::number(stationId));
    QNetworkRequest request(url);

    networkManager->get(request);
    timeoutTimer->start(TIMEOUT_MS);
}

void RestApi::cancelRequests()
{
    // Find all pending replies
    QList<QNetworkReply*> replies = networkManager->findChildren<QNetworkReply*>();
    for (QNetworkReply *reply : replies) {
        reply->abort();
    }

    timeoutTimer->stop();
}

void RestApi::handleNetworkReply(QNetworkReply *reply)
{
    // Stop the timeout timer
    timeoutTimer->stop();

    // Check for errors
    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred("Network error: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    // Read the response data
    QByteArray data = reply->readAll();

    // Parse the JSON data
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull()) {
        emit errorOccurred("Invalid JSON data received");
        reply->deleteLater();
        return;
    }

    // Process the data based on the endpoint
    QString url = reply->url().toString();

    try {
        if (url.contains(STATIONS_ENDPOINT)) {
            if (!jsonDoc.isArray()) {
                throw std::runtime_error("Expected JSON array for stations");
            }
            QList<Station> stations = parseStations(jsonDoc.array());
            emit stationsReceived(stations);
        } else if (url.contains(SENSORS_ENDPOINT)) {
            if (!jsonDoc.isArray()) {
                throw std::runtime_error("Expected JSON array for sensors");
            }
            QList<Sensor> sensors = parseSensors(jsonDoc.array());
            emit sensorsReceived(sensors);
        } else if (url.contains(MEASUREMENTS_ENDPOINT)) {
            if (!jsonDoc.isObject()) {
                throw std::runtime_error("Expected JSON object for measurements");
            }
            QList<Measurement> measurements = parseMeasurements(jsonDoc.object());
            emit measurementsReceived(measurements);
        } else if (url.contains(AIR_QUALITY_INDEX_ENDPOINT)) {
            // Air quality index not implemented in this version
        }
    } catch (const std::exception &e) {
        emit errorOccurred(QString("Error processing data: ") + e.what());
    }

    reply->deleteLater();
}

void RestApi::handleTimeout()
{
    emit errorOccurred("Request timed out");
    cancelRequests();
}

QList<Station> RestApi::parseStations(const QJsonArray &json)
{
    QList<Station> stations;

    for (const QJsonValue &value : json) {
        if (!value.isObject()) {
            continue;
        }

        QJsonObject stationObj = value.toObject();

        int id = stationObj["id"].toInt();
        QString stationName = stationObj["stationName"].toString();
        double gegrLat = stationObj["gegrLat"].toDouble();
        double gegrLon = stationObj["gegrLon"].toDouble();

        QJsonObject cityObj = stationObj["city"].toObject();
        QString cityName = cityObj["name"].toString();

        QJsonObject communeObj = cityObj["commune"].toObject();
        QString communeName = communeObj["communeName"].toString();
        QString districtName = communeObj["districtName"].toString();
        QString provinceName = communeObj["provinceName"].toString();

        QString addressStreet = stationObj["addressStreet"].toString();

        Station station(id, stationName, gegrLat, gegrLon, cityName,
                        addressStreet, communeName, districtName, provinceName);
        stations.append(station);
    }

    return stations;
}

QList<Sensor> RestApi::parseSensors(const QJsonArray &json)
{
    QList<Sensor> sensors;

    for (const QJsonValue &value : json) {
        if (!value.isObject()) {
            continue;
        }

        QJsonObject sensorObj = value.toObject();

        int id = sensorObj["id"].toInt();
        int stationId = sensorObj["stationId"].toInt();

        QJsonObject paramObj = sensorObj["param"].toObject();
        QString paramName = paramObj["paramName"].toString();
        QString paramFormula = paramObj["paramFormula"].toString();
        QString paramCode = paramObj["paramCode"].toString();
        int idParam = paramObj["idParam"].toInt();

        Sensor sensor(id, stationId, paramName, paramFormula, paramCode, idParam);
        sensors.append(sensor);
    }

    return sensors;
}

QList<Measurement> RestApi::parseMeasurements(const QJsonObject &json)
{
    QList<Measurement> measurements;

    QString key = json["key"].toString();
    QJsonArray valuesArray = json["values"].toArray();

    int sensorId = 0;
    QString url = json["url"].toString();
    if (url.contains(MEASUREMENTS_ENDPOINT)) {
        QStringList parts = url.split("/");
        if (parts.size() > 0) {
            bool ok;
            sensorId = parts.last().toInt(&ok);
            if (!ok) {
                sensorId = 0;
            }
        }
    }

    for (const QJsonValue &value : valuesArray) {
        if (!value.isObject()) {
            continue;
        }

        QJsonObject measurementObj = value.toObject();

        QString dateStr = measurementObj["date"].toString();
        QDateTime date = QDateTime::fromString(dateStr, Qt::ISODate);

        if (measurementObj["value"].isNull()) {
            Measurement measurement(sensorId, key, date, 0.0);
            measurements.append(measurement);
        } else {
            double measurementValue = measurementObj["value"].toDouble();
            Measurement measurement(sensorId, key, date, measurementValue);
            measurements.append(measurement);
        }
    }

    return measurements;
}
