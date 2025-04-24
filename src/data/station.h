/**
 * @file station.h
 * @brief Header file for the Station class
 */

 #ifndef STATION_H
 #define STATION_H
 
 #include <QString>
 #include <QJsonObject>
 #include <QJsonDocument>
 
 /**
  * @class Station
  * @brief Represents a measurement station
  */
 class Station
 {
 public:
     /**
      * @brief Default constructor
      */
     Station();
     
     /**
      * @brief Constructor with parameters
      * @param id Station ID
      * @param name Station name
      * @param latitude Geographic latitude
      * @param longitude Geographic longitude
      * @param city City name
      * @param address Street address
      * @param commune Commune name
      * @param district District name
      * @param province Province name
      */
     Station(int id, const QString &name, double latitude, double longitude,
             const QString &city, const QString &address, const QString &commune,
             const QString &district, const QString &province);
     
     /**
      * @brief Gets the station ID
      * @return Station ID
      */
     int getId() const;
     
     /**
      * @brief Gets the station name
      * @return Station name
      */
     QString getName() const;
     
     /**
      * @brief Gets the geographic latitude
      * @return Geographic latitude
      */
     double getLatitude() const;
     
     /**
      * @brief Gets the geographic longitude
      * @return Geographic longitude
      */
     double getLongitude() const;
     
     /**
      * @brief Gets the city name
      * @return City name
      */
     QString getCity() const;
     
     /**
      * @brief Gets the street address
      * @return Street address
      */
     QString getAddress() const;
     
     /**
      * @brief Gets the commune name
      * @return Commune name
      */
     QString getCommune() const;
     
     /**
      * @brief Gets the district name
      * @return District name
      */
     QString getDistrict() const;
     
     /**
      * @brief Gets the province name
      * @return Province name
      */
     QString getProvince() const;
     
     /**
      * @brief Converts the station to JSON
      * @return JSON representation of the station
      */
     QJsonObject toJson() const;
     
     /**
      * @brief Creates a station from JSON
      * @param json JSON object
      * @return Station object
      */
     static Station fromJson(const QJsonObject &json);
 
 private:
     int id;
     QString name;
     double latitude;
     double longitude;
     QString city;
     QString address;
     QString commune;
     QString district;
     QString province;
 };
 
 #endif // STATION_H