/**
 * @file jsonrepository.cpp
 * @brief Implementation of the JSON repository
 */

 #include "src/database/jsonrepository.h"

 JsonRepository::JsonRepository(QObject *parent) : QObject(parent)
 {
     ensureDataDirectoryExists();
 }
 
 bool JsonRepository::saveStations(const QList<Station> &stations)
 {
     QMutexLocker locker(&mutex);
 
     // Create a JSON array to store stations
     QJsonArray stationsArray;
     for (const Station &station : stations) {
         stationsArray.append(station.toJson());
     }
 
     // Create a JSON document
     QJsonDocument document(stationsArray);
 
     // Write to file
     QFile jsonFile(getFilePath(STATIONS_FILE));
     if (!jsonFile.open(QIODevice::WriteOnly)) {
         emit errorOccurred("Failed to open stations file for writing: " + jsonFile.errorString());
         return false;
     }
 
     jsonFile.write(document.toJson());
     jsonFile.close();
 
     return true;
 }
 
 QList<Station> JsonRepository::loadStations()
 {
     QMutexLocker locker(&mutex);
     QList<Station> stations;
 
     if (!ensureDataDirectoryExists()) {
         return stations;
     }
 
     QFile jsonFile(getFilePath(STATIONS_FILE));
     if (!jsonFile.exists()) {
         // No error, just return an empty list
         return stations;
     }
 
     if (!jsonFile.open(QIODevice::ReadOnly)) {
         emit errorOccurred("Failed to open stations file for reading: " + jsonFile.errorString());
         return stations;
     }
 
     // Read the JSON data
     QByteArray jsonData = jsonFile.readAll();
     jsonFile.close();
 
     // Parse the JSON document
     QJsonDocument document = QJsonDocument::fromJson(jsonData);
 
     // Check if the document is an array
     if (!document.isArray()) {
         emit errorOccurred("Invalid stations file format");
         return stations;
     }
 
     // Parse the array of stations
     QJsonArray stationsArray = document.array();
     for (const QJsonValue &value : stationsArray) {
         if (value.isObject()) {
             Station station = Station::fromJson(value.toObject());
             stations.append(station);
         }
     }
 
     return stations;
 }
 
 bool JsonRepository::saveSensors(const QList<Sensor> &sensors)
 {
     QMutexLocker locker(&mutex);
 
     // Create a JSON array to store sensors
     QJsonArray sensorsArray;
     for (const Sensor &sensor : sensors) {
         sensorsArray.append(sensor.toJson());
     }
 
     // Create a JSON document
     QJsonDocument document(sensorsArray);
 
     // Write to file
     QFile jsonFile(getFilePath(SENSORS_FILE));
     if (!jsonFile.open(QIODevice::WriteOnly)) {
         emit errorOccurred("Failed to open sensors file for writing: " + jsonFile.errorString());
         return false;
     }
 
     jsonFile.write(document.toJson());
     jsonFile.close();
 
     return true;
 }
 
 QList<Sensor> JsonRepository::loadSensors()
 {
     QMutexLocker locker(&mutex);
     QList<Sensor> sensors;
 
     if (!ensureDataDirectoryExists()) {
         return sensors;
     }
 
     QFile jsonFile(getFilePath(SENSORS_FILE));
     if (!jsonFile.exists()) {
         // No error, just return an empty list
         return sensors;
     }
 
     if (!jsonFile.open(QIODevice::ReadOnly)) {
         emit errorOccurred("Failed to open sensors file for reading: " + jsonFile.errorString());
         return sensors;
     }
 
     // Read the JSON data
     QByteArray jsonData = jsonFile.readAll();
     jsonFile.close();
 
     // Parse the JSON document
     QJsonDocument document = QJsonDocument::fromJson(jsonData);
 
     // Check if the document is an array
     if (!document.isArray()) {
         emit errorOccurred("Invalid sensors file format");
         return sensors;
     }
 
     // Parse the array of sensors
     QJsonArray sensorsArray = document.array();
     for (const QJsonValue &value : sensorsArray) {
         if (value.isObject()) {
             Sensor sensor = Sensor::fromJson(value.toObject());
             sensors.append(sensor);
         }
     }
 
     return sensors;
 }
 
 QList<Sensor> JsonRepository::loadSensors(int stationId)
 {
     QMutexLocker locker(&mutex);
     QList<Sensor> sensors;
 
     if (!ensureDataDirectoryExists()) {
         return sensors;
     }
 
     QFile jsonFile(getFilePath(SENSORS_FILE));
     if (!jsonFile.exists()) {
         // No error, just return an empty list
         return sensors;
     }
 
     if (!jsonFile.open(QIODevice::ReadOnly)) {
         emit errorOccurred("Failed to open sensors file for reading: " + jsonFile.errorString());
         return sensors;
     }
 
     // Read the JSON data
     QByteArray jsonData = jsonFile.readAll();
     jsonFile.close();
 
     // Parse the JSON document
     QJsonDocument document = QJsonDocument::fromJson(jsonData);
 
     // Check if the document is an array
     if (!document.isArray()) {
         emit errorOccurred("Invalid sensors file format");
         return sensors;
     }
 
     // Parse the array of sensors and filter by stationId
     QJsonArray sensorsArray = document.array();
     for (const QJsonValue &value : sensorsArray) {
         if (value.isObject()) {
             Sensor sensor = Sensor::fromJson(value.toObject());
             if (sensor.getStationId() == stationId) {
                 sensors.append(sensor);
             }
         }
     }
 
     return sensors;
 }
 
 bool JsonRepository::saveMeasurements(const QList<Measurement> &measurements, int sensorId)
 {
     QMutexLocker locker(&mutex);
 
     // Create a JSON array to store measurements
     QJsonArray measurementsArray;
     for (const Measurement &measurement : measurements) {
         measurementsArray.append(measurement.toJson());
     }
 
     // Create a JSON document
     QJsonDocument document(measurementsArray);
 
     // Write to file
     QFile jsonFile(getFilePath(getMeasurementsFileName(sensorId)));
     if (!jsonFile.open(QIODevice::WriteOnly)) {
         emit errorOccurred("Failed to open measurements file for writing: " + jsonFile.errorString());
         return false;
     }
 
     jsonFile.write(document.toJson());
     jsonFile.close();
 
     return true;
 }
 
 QList<Measurement> JsonRepository::loadMeasurements(int sensorId,
                                                     const QDateTime &startDate,
                                                     const QDateTime &endDate)
 {
     QMutexLocker locker(&mutex);
     QList<Measurement> measurements;
 
     if (!ensureDataDirectoryExists()) {
         return measurements;
     }
 
     QFile jsonFile(getFilePath(getMeasurementsFileName(sensorId)));
     if (!jsonFile.exists()) {
         // No error, just return an empty list
         return measurements;
     }
 
     if (!jsonFile.open(QIODevice::ReadOnly)) {
         emit errorOccurred("Failed to open measurements file for reading: " + jsonFile.errorString());
         return measurements;
     }
 
     // Read the JSON data
     QByteArray jsonData = jsonFile.readAll();
     jsonFile.close();
 
     // Parse the JSON document
     QJsonDocument document = QJsonDocument::fromJson(jsonData);
 
     // Check if the document is an array
     if (!document.isArray()) {
         emit errorOccurred("Invalid measurements file format");
         return measurements;
     }
 
     // Parse the array of measurements
     QJsonArray measurementsArray = document.array();
     for (const QJsonValue &value : measurementsArray) {
         if (value.isObject()) {
             Measurement measurement = Measurement::fromJson(value.toObject(), sensorId, "");
 
             // Filter by date range if specified
             if (startDate.isValid() && measurement.getDate() < startDate) {
                 continue;
             }
 
             if (endDate.isValid() && measurement.getDate() > endDate) {
                 continue;
             }
 
             measurements.append(measurement);
         }
     }
 
     return measurements;
 }
 
 bool JsonRepository::ensureDataDirectoryExists()
 {
     QDir dir;
     if (!dir.exists(DATA_DIR)) {
         if (!dir.mkdir(DATA_DIR)) {
             emit errorOccurred("Failed to create data directory");
             return false;
         }
     }
     return true;
 }
 
 QString JsonRepository::getFilePath(const QString &fileName) const
 {
     return DATA_DIR + "/" + fileName;
 }
 
 QString JsonRepository::getMeasurementsFileName(int sensorId) const
 {
     return MEASUREMENTS_FILE_PREFIX + QString::number(sensorId) + ".json";
 }
 