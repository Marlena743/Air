/**
 * @file restapi.h
 * @brief Header file for the REST API wrapper
 */

 #ifndef RESTAPI_H
 #define RESTAPI_H
 
 #include <QObject>
 #include <QNetworkAccessManager>
 #include <QNetworkReply>
 #include <QNetworkRequest>
 #include <QJsonDocument>
 #include <QJsonObject>
 #include <QJsonArray>
 #include <QUrl>
 #include <QTimer>
 
 #include "../data/station.h"
 #include "../data/sensor.h"
 #include "../data/measurement.h"
 
 /**
  * @class RestApi
  * @brief Wrapper for the GIOŚ REST API
  */
 class RestApi : public QObject
 {
     Q_OBJECT
 
 public:
     /**
      * @brief Constructor
      * @param parent Parent object
      */
     explicit RestApi(QObject *parent = nullptr);
     
     /**
      * @brief Destructor
      */
     ~RestApi();
     
     /**
      * @brief Gets all stations
      */
     void getAllStations();
     
     /**
      * @brief Gets stations by city name
      * @param cityName City name
      */
     void getStationsByCity(const QString &cityName);
     
     /**
      * @brief Gets sensors for a station
      * @param stationId Station ID
      */
     void getSensors(int stationId);
     
     /**
      * @brief Gets measurements for a sensor
      * @param sensorId Sensor ID
      */
     void getMeasurements(int sensorId);
     
     /**
      * @brief Gets the air quality index for a station
      * @param stationId Station ID
      */
     void getAirQualityIndex(int stationId);
     
     /**
      * @brief Cancels all pending requests
      */
     void cancelRequests();
 
 signals:
     /**
      * @brief Signal emitted when stations are received
      * @param stations List of stations
      */
     void stationsReceived(const QList<Station> &stations);
     
     /**
      * @brief Signal emitted when sensors are received
      * @param sensors List of sensors
      */
     void sensorsReceived(const QList<Sensor> &sensors);
     
     /**
      * @brief Signal emitted when measurements are received
      * @param measurements List of measurements
      */
     void measurementsReceived(const QList<Measurement> &measurements);
     
     /**
      * @brief Signal emitted when an error occurs
      * @param errorMessage Error message
      */
     void errorOccurred(const QString &errorMessage);
 
 private slots:
     /**
      * @brief Handles network replies
      * @param reply Network reply
      */
     void handleNetworkReply(QNetworkReply *reply);
     
     /**
      * @brief Handles timeouts
      */
     void handleTimeout();
 
 private:
     QNetworkAccessManager *networkManager;
     QTimer *timeoutTimer;
     const int TIMEOUT_MS = 10000;
     
     const QString BASE_URL = "https://api.gios.gov.pl/pjp-api/rest";
     const QString STATIONS_ENDPOINT = "/station/findAll";
     const QString SENSORS_ENDPOINT = "/station/sensors";
     const QString MEASUREMENTS_ENDPOINT = "/data/getData";
     const QString AIR_QUALITY_INDEX_ENDPOINT = "/aqindex/getIndex";
     
     /**
      * @brief Parses station data from JSON
      * @param json JSON data
      * @return List of stations
      */
     QList<Station> parseStations(const QJsonArray &json);
     
     /**
      * @brief Parses sensor data from JSON
      * @param json JSON data
      * @return List of sensors
      */
     QList<Sensor> parseSensors(const QJsonArray &json);
     
     /**
      * @brief Parses measurement data from JSON
      * @param json JSON data
      * @return List of measurements
      */
     QList<Measurement> parseMeasurements(const QJsonObject &json);
 };
 
 #endif // RESTAPI_H