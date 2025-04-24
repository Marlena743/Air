/**
 * @file measurement.h
 * @brief Header file for the Measurement class
 */

 #ifndef MEASUREMENT_H
 #define MEASUREMENT_H
 
 #include <QString>
 #include <QDateTime>
 #include <QJsonObject>
 #include <QJsonArray>
 #include <QJsonDocument>
 
 /**
  * @class Measurement
  * @brief Represents a measurement value
  */
 class Measurement
 {
 public:
     /**
      * @brief Default constructor
      */
     Measurement();
     
     /**
      * @brief Constructor with parameters
      * @param sensorId Sensor ID
      * @param paramCode Parameter code
      * @param date Measurement date and time
      * @param value Measurement value
      */
     Measurement(int sensorId, const QString &paramCode, 
                 const QDateTime &date, double value);
     
     /**
      * @brief Gets the sensor ID
      * @return Sensor ID
      */
     int getSensorId() const;
     
     /**
      * @brief Gets the parameter code
      * @return Parameter code
      */
     QString getParamCode() const;
     
     /**
      * @brief Gets the measurement date and time
      * @return Measurement date and time
      */
     QDateTime getDate() const;
     
     /**
      * @brief Gets the measurement value
      * @return Measurement value
      */
     double getValue() const;
     
     /**
      * @brief Checks if the measurement has a valid value
      * @return True if the value is valid, false otherwise
      */
     bool isValid() const;
     
     /**
      * @brief Converts the measurement to JSON
      * @return JSON representation of the measurement
      */
     QJsonObject toJson() const;
     
     /**
      * @brief Creates a measurement from JSON
      * @param json JSON object
      * @param sensorId Sensor ID
      * @param paramCode Parameter code
      * @return Measurement object
      */
     static Measurement fromJson(const QJsonObject &json, int sensorId, const QString &paramCode);
 
 private:
     int sensorId;
     QString paramCode;
     QDateTime date;
     double value;
     bool valid;
 };
 
 #endif // MEASUREMENT_H