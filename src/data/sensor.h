/**
 * @file sensor.h
 * @brief Header file for the Sensor class
 */

 #ifndef SENSOR_H
 #define SENSOR_H
 
 #include <QString>
 #include <QJsonObject>
 #include <QJsonDocument>
 
 /**
  * @class Sensor
  * @brief Represents a measurement sensor
  */
 class Sensor
 {
 public:
     /**
      * @brief Default constructor
      */
     Sensor();
     
     /**
      * @brief Constructor with parameters
      * @param id Sensor ID
      * @param stationId Station ID
      * @param paramName Parameter name
      * @param paramFormula Parameter formula
      * @param paramCode Parameter code
      * @param idParam Parameter ID
      */
     Sensor(int id, int stationId, const QString &paramName, 
            const QString &paramFormula, const QString &paramCode, int idParam);
     
     /**
      * @brief Gets the sensor ID
      * @return Sensor ID
      */
     int getId() const;
     
     /**
      * @brief Gets the station ID
      * @return Station ID
      */
     int getStationId() const;
     
     /**
      * @brief Gets the parameter name
      * @return Parameter name
      */
     QString getParamName() const;
     
     /**
      * @brief Gets the parameter formula
      * @return Parameter formula
      */
     QString getParamFormula() const;
     
     /**
      * @brief Gets the parameter code
      * @return Parameter code
      */
     QString getParamCode() const;
     
     /**
      * @brief Gets the parameter ID
      * @return Parameter ID
      */
     int getIdParam() const;
     
     /**
      * @brief Converts the sensor to JSON
      * @return JSON representation of the sensor
      */
     QJsonObject toJson() const;
     
     /**
      * @brief Creates a sensor from JSON
      * @param json JSON object
      * @return Sensor object
      */
     static Sensor fromJson(const QJsonObject &json);
 
 private:
     int id;
     int stationId;
     QString paramName;
     QString paramFormula;
     QString paramCode;
     int idParam;
 };
 
 #endif // SENSOR_H