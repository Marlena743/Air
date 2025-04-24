/**
 * @file jsonrepository.h
 * @brief Header file for the JSON repository
 */

 #ifndef JSONREPOSITORY_H
 #define JSONREPOSITORY_H
 
 #include <QObject>
 #include <QFile>
 #include <QDir>
 #include <QJsonDocument>
 #include <QJsonObject>
 #include <QJsonArray>
 #include <QDateTime>
 #include <QMutex>
 
 #include "../data/station.h"
 #include "../data/sensor.h"
 #include "../data/measurement.h"
 
 /**
  * @class JsonRepository
  * @brief Repository for storing and retrieving data in JSON format
  */
 class JsonRepository : public QObject
 {
     Q_OBJECT
 
 public:
     /**
      * @brief Constructor
      * @param parent Parent object
      */
     explicit JsonRepository(QObject *parent = nullptr);
 
     /**
      * @brief Saves stations to a JSON file
      * @param stations List of stations
      * @return True if successful, false otherwise
      */
     bool saveStations(const QList<Station> &stations);
 
     /**
      * @brief Loads stations from a JSON file
      * @return List of stations
      */
     QList<Station> loadStations();
 
     /**
      * @brief Saves sensors to a JSON file
      * @param sensors List of sensors
      * @return True if successful, false otherwise
      */
     bool saveSensors(const QList<Sensor> &sensors);
 
     /**
      * @brief Loads sensors from a JSON file
      * @return List of sensors
      */
     QList<Sensor> loadSensors();
 
     /**
      * @brief Loads sensors for a specific station from a JSON file
      * @param stationId Station ID
      * @return List of sensors
      */
     QList<Sensor> loadSensors(int stationId);
 
     /**
      * @brief Saves measurements to a JSON file
      * @param measurements List of measurements
      * @param sensorId Sensor ID
      * @return True if successful, false otherwise
      */
     bool saveMeasurements(const QList<Measurement> &measurements, int sensorId);
 
     /**
      * @brief Loads measurements from a JSON file
      * @param sensorId Sensor ID
      * @param startDate Start date (optional)
      * @param endDate End date (optional)
      * @return List of measurements
      */
     QList<Measurement> loadMeasurements(int sensorId,
                                         const QDateTime &startDate = QDateTime(),
                                         const QDateTime &endDate = QDateTime());
 
 signals:
     /**
      * @brief Signal emitted when an error occurs
      * @param errorMessage Error message
      */
     void errorOccurred(const QString &errorMessage);
 
 private:
     QMutex mutex;
     const QString DATA_DIR = "data";
     const QString STATIONS_FILE = "stations.json";
     const QString SENSORS_FILE = "sensors.json";
     const QString MEASUREMENTS_FILE_PREFIX = "measurements_";
 
     /**
      * @brief Ensures that the data directory exists
      * @return True if the directory exists or was created, false otherwise
      */
     bool ensureDataDirectoryExists();
 
     /**
      * @brief Gets the full path to a file
      * @param fileName File name
      * @return Full path to the file
      */
     QString getFilePath(const QString &fileName) const;
 
     /**
      * @brief Gets the measurements file name for a sensor
      * @param sensorId Sensor ID
      * @return File name
      */
     QString getMeasurementsFileName(int sensorId) const;
 };
 
 #endif // JSONREPOSITORY_H
 