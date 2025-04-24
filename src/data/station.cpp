/**
 * @file station.cpp
 * @brief Implementation of the Station class
 */

 #include "src/data/station.h"

 Station::Station() :
     id(0),
     latitude(0.0),
     longitude(0.0)
 {
 }
 
 Station::Station(int id, const QString &name, double latitude, double longitude,
                 const QString &city, const QString &address, const QString &commune,
                 const QString &district, const QString &province) :
     id(id),
     name(name),
     latitude(latitude),
     longitude(longitude),
     city(city),
     address(address),
     commune(commune),
     district(district),
     province(province)
 {
 }
 
 int Station::getId() const
 {
     return id;
 }
 
 QString Station::getName() const
 {
     return name;
 }
 
 double Station::getLatitude() const
 {
     return latitude;
 }
 
 double Station::getLongitude() const
 {
     return longitude;
 }
 
 QString Station::getCity() const
 {
     return city;
 }
 
 QString Station::getAddress() const
 {
     return address;
 }
 
 QString Station::getCommune() const
 {
     return commune;
 }
 
 QString Station::getDistrict() const
 {
     return district;
 }
 
 QString Station::getProvince() const
 {
     return province;
 }
 
 QJsonObject Station::toJson() const
 {
     QJsonObject json;
     json["id"] = id;
     json["name"] = name;
     json["latitude"] = latitude;
     json["longitude"] = longitude;
     json["city"] = city;
     json["address"] = address;
     json["commune"] = commune;
     json["district"] = district;
     json["province"] = province;
     return json;
 }
 
 Station Station::fromJson(const QJsonObject &json)
 {
     int id = json["id"].toInt();
     QString name = json["name"].toString();
     double latitude = json["latitude"].toDouble();
     double longitude = json["longitude"].toDouble();
     QString city = json["city"].toString();
     QString address = json["address"].toString();
     QString commune = json["commune"].toString();
     QString district = json["district"].toString();
     QString province = json["province"].toString();
     
     return Station(id, name, latitude, longitude, city, address, commune, district, province);
 }
