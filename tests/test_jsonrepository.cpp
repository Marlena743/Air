/**
 * @file test_jsonrepository.cpp
 * @brief Test file for the JsonRepository class
 */

 #include <QtTest/QtTest>
 #include "../src/database/jsonrepository.h"
 #include "../src/data/station.h"
 #include "../src/data/sensor.h"
 #include "../src/data/measurement.h"
 
 /**
  * @class TestJsonRepository
  * @brief Tests for the JsonRepository class
  */
 class TestJsonRepository : public QObject
 {
     Q_OBJECT
 
 private slots:
     /**
      * @brief Test initialization
      */
     void initTestCase();
     
     /**
      * @brief Test saving and loading stations
      */
     void testStations();
     
     /**
      * @brief Test saving and loading sensors
      */
     void testSensors();
     
     /**
      * @brief Test saving and loading measurements
      */
     void testMeasurements();
     
     /**
      * @brief Test cleanup
      */
     void cleanupTestCase();
 
 private:
     JsonRepository *repository;
 };
 
 void TestJsonRepository::initTestCase()
 {
     repository = new JsonRepository();
 }
 
 void TestJsonRepository::testStations()
 {
     // Create test data
     QList<Station> testStations;
     testStations.append(Station(1, "Test Station 1", 52.1, 21.0, "Warsaw", "Test St.", "Commune1", "District1", "Mazowieckie"));
     testStations.append(Station(2, "Test Station 2", 50.1, 19.0, "Krakow", "Test Ave.", "Commune2", "District2", "Malopolskie"));
     
     // Save stations
     bool saveResult = repository->saveStations(testStations);
     QVERIFY(saveResult);
     
     // Load stations
     QList<Station> loadedStations = repository->loadStations();
     
     // Verify the result
     QCOMPARE(loadedStations.size(), testStations.size());
     
     for (int i = 0; i < testStations.size(); ++i) {
         QCOMPARE(loadedStations[i].getId(), testStations[i].getId());
         QCOMPARE(loadedStations[i].getName(), testStations[i].getName());
         QCOMPARE(loadedStations[i].getCity(), testStations[i].getCity());
         QCOMPARE(loadedStations[i].getLatitude(), testStations[i].getLatitude());
         QCOMPARE(loadedStations[i].getLongitude(), testStations[i].getLongitude());
     }
 }
 
 void TestJsonRepository::testSensors()
 {
     // Create test data
     QList<Sensor> testSensors;
     testSensors.append(Sensor(1, 1, "PM10", "PM10", "PM10", 1));
     testSensors.append(Sensor(2, 1, "PM2.5", "PM2.5", "PM25", 2));
     
     // Save sensors
     bool saveResult = repository->saveSensors(testSensors);
     QVERIFY(saveResult);
     
     // Load sensors
     QList<Sensor> loadedSensors = repository->loadSensors();
     
     // Verify the result
     QCOMPARE(loadedSensors.size(), testSensors.size());
     
     for (int i = 0; i < testSensors.size(); ++i) {
         QCOMPARE(loadedSensors[i].getId(), testSensors[i].getId());
         QCOMPARE(loadedSensors[i].getStationId(), testSensors[i].getStationId());
         QCOMPARE(loadedSensors[i].getParamName(), testSensors[i].getParamName());
         QCOMPARE(loadedSensors[i].getParamFormula(), testSensors[i].getParamFormula());
         QCOMPARE(loadedSensors[i].getParamCode(), testSensors[i].getParamCode());
     }
 }
 
 void TestJsonRepository::testMeasurements()
 {
     // Create test data
     QList<Measurement> testMeasurements;
     QDateTime now = QDateTime::currentDateTime();
     
     testMeasurements.append(Measurement(1, "PM10", now, 25.5));
     testMeasurements.append(Measurement(1, "PM10", now.addSecs(3600), 26.7));
     testMeasurements.append(Measurement(1, "PM10", now.addSecs(7200), 27.2));
     
     // Save measurements
     bool saveResult = repository->saveMeasurements(testMeasurements, 1);
     QVERIFY(saveResult);
     
     // Load measurements
     QList<Measurement> loadedMeasurements = repository->loadMeasurements(1);
     
     // Verify the result
     QCOMPARE(loadedMeasurements.size(), testMeasurements.size());
     
     for (int i = 0; i < testMeasurements.size(); ++i) {
         QCOMPARE(loadedMeasurements[i].getSensorId(), testMeasurements[i].getSensorId());
         QCOMPARE(loadedMeasurements[i].getParamCode(), testMeasurements[i].getParamCode());
         QCOMPARE(loadedMeasurements[i].getValue(), testMeasurements[i].getValue());
         
         // Compare dates with a small tolerance for serialization differences
         qint64 dateDiff = qAbs(loadedMeasurements[i].getDate().secsTo(testMeasurements[i].getDate()));
         QVERIFY(dateDiff < 2); // Allow 2 seconds difference
     }
     
     // Test date range filtering
     QDateTime startDate = now.addSecs(1800); // 30 minutes after the first measurement
     QDateTime endDate = now.addSecs(8000);   // After the last measurement
     
     QList<Measurement> filteredMeasurements = repository->loadMeasurements(1, startDate, endDate);
     
     // Only the second and third measurements should be included
     QCOMPARE(filteredMeasurements.size(), 2);
     
     if (filteredMeasurements.size() == 2) {
         QCOMPARE(filteredMeasurements[0].getValue(), testMeasurements[1].getValue());
         QCOMPARE(filteredMeasurements[1].getValue(), testMeasurements[2].getValue());
     }
 }
 
 void TestJsonRepository::cleanupTestCase()
 {
     delete repository;
     
     // Clean up test files
     QDir dataDir("data");
     if (dataDir.exists()) {
         dataDir.remove("stations.json");
         dataDir.remove("sensors.json");
         dataDir.remove("measurements_1.json");
     }
 }
 
 QTEST_MAIN(TestJsonRepository)
 #include "test_jsonrepository.moc"