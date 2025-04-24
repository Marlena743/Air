/**
 * @file measurement.cpp
 * @brief Implementation of the Measurement class
 */

 #include "src/data/measurement.h"

 Measurement::Measurement() :
     sensorId(0),
     value(0.0),
     valid(false)
 {
 }

 Measurement::Measurement(int sensorId, const QString &paramCode,
                         const QDateTime &date, double value) :
     sensorId(sensorId),
     paramCode(paramCode),
     date(date),
     value(value),
     valid(true)
 {
 }

 int Measurement::getSensorId() const
 {
     return sensorId;
 }

 QString Measurement::getParamCode() const
 {
     return paramCode;
 }

 QDateTime Measurement::getDate() const
 {
     return date;
 }

 double Measurement::getValue() const
 {
     return value;
 }

 bool Measurement::isValid() const
 {
     return valid;
 }

 QJsonObject Measurement::toJson() const
 {
     QJsonObject json;
     json["sensorId"] = sensorId;
     json["paramCode"] = paramCode;
     json["date"] = date.toString(Qt::ISODate);
     json["value"] = value;
     json["valid"] = valid;
     return json;
 }

 Measurement Measurement::fromJson(const QJsonObject &json, int sensorId, const QString &paramCode)
 {
     QString dateStr = json["date"].toString();
     QDateTime date = QDateTime::fromString(dateStr, Qt::ISODate);

     // If value is null, create an invalid measurement
     if (json["value"].isNull()) {
         Measurement measurement;
         measurement.sensorId = sensorId;
         measurement.paramCode = paramCode;
         measurement.date = date;
         measurement.value = 0.0;
         measurement.valid = false;
         return measurement;
     }

     double value = json["value"].toDouble();
     bool valid = json["valid"].toBool(true); // Default to true if not specified

     Measurement measurement(sensorId, paramCode, date, value);
     measurement.valid = valid;
     return measurement;
 }
