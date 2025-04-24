/**
 * @file sensor.cpp
 * @brief Implementation of the Sensor class
 */

 #include "src/data/sensor.h"

 Sensor::Sensor() :
     id(0),
     stationId(0),
     idParam(0)
 {
 }
 
 Sensor::Sensor(int id, int stationId, const QString &paramName, 
                const QString &paramFormula, const QString &paramCode, int idParam) :
     id(id),
     stationId(stationId),
     paramName(paramName),
     paramFormula(paramFormula),
     paramCode(paramCode),
     idParam(idParam)
 {
 }
 
 int Sensor::getId() const
 {
     return id;
 }
 
 int Sensor::getStationId() const
 {
     return stationId;
 }
 
 QString Sensor::getParamName() const
 {
     return paramName;
 }
 
 QString Sensor::getParamFormula() const
 {
     return paramFormula;
 }
 
 QString Sensor::getParamCode() const
 {
     return paramCode;
 }
 
 int Sensor::getIdParam() const
 {
     return idParam;
 }
 
 QJsonObject Sensor::toJson() const
 {
     QJsonObject json;
     json["id"] = id;
     json["stationId"] = stationId;
     json["paramName"] = paramName;
     json["paramFormula"] = paramFormula;
     json["paramCode"] = paramCode;
     json["idParam"] = idParam;
     return json;
 }
 
 Sensor Sensor::fromJson(const QJsonObject &json)
 {
     int id = json["id"].toInt();
     int stationId = json["stationId"].toInt();
     QString paramName = json["paramName"].toString();
     QString paramFormula = json["paramFormula"].toString();
     QString paramCode = json["paramCode"].toString();
     int idParam = json["idParam"].toInt();
     
     return Sensor(id, stationId, paramName, paramFormula, paramCode, idParam);
 }
