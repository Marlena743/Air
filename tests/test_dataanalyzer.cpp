/**
 * @file test_dataanalyzer.cpp
 * @brief Test file for the DataAnalyzer class
 */

 #include <QtTest/QtTest>
 #include "../src/analysis/dataanalyzer.h"
 #include "../src/data/measurement.h"
 
 /**
  * @class TestDataAnalyzer
  * @brief Tests for the DataAnalyzer class
  */
 class TestDataAnalyzer : public QObject
 {
     Q_OBJECT
 
 private slots:
     /**
      * @brief Test initialization
      */
     void initTestCase();
     
     /**
      * @brief Test basic analysis
      */
     void testBasicAnalysis();
     
     /**
      * @brief Test trend calculation with increasing values
      */
     void testIncreasingTrend();
     
     /**
      * @brief Test trend calculation with decreasing values
      */
     void testDecreasingTrend();
     
     /**
      * @brief Test trend calculation with stable values
      */
     void testStableTrend();
     
     /**
      * @brief Test trend calculation with fluctuating values
      */
     void testFluctuatingTrend();
     
     /**
      * @brief Test analysis with empty data
      */
     void testEmptyData();
     
     /**
      * @brief Test analysis with invalid measurements
      */
     void testInvalidMeasurements();
     
     /**
      * @brief Test cleanup
      */
     void cleanupTestCase();
 
 private:
     DataAnalyzer *analyzer;
 };
 
 void TestDataAnalyzer::initTestCase()
 {
     analyzer = new DataAnalyzer();
 }
 
 void TestDataAnalyzer::testBasicAnalysis()
 {
     // Create test data
     QList<Measurement> measurements;
     QDateTime now = QDateTime::currentDateTime();
     
     measurements.append(Measurement(1, "PM10", now, 25.5));
     measurements.append(Measurement(1, "PM10", now.addSecs(3600), 30.7));
     measurements.append(Measurement(1, "PM10", now.addSecs(7200), 15.2));
     measurements.append(Measurement(1, "PM10", now.addSecs(10800), 20.0));
     
     // Analyze the data
     AnalysisResult result = analyzer->analyze(measurements);
     
     // Verify the results
     QCOMPARE(result.minValue, 15.2);
     QCOMPARE(result.maxValue, 30.7);
     QCOMPARE(qRound(result.avgValue * 100) / 100.0, 22.85); // Round to 2 decimal places
     
     // Verify min/max dates
     QCOMPARE(result.minDate, now.addSecs(7200));
     QCOMPARE(result.maxDate, now.addSecs(3600));
 }
 
 void TestDataAnalyzer::testIncreasingTrend()
 {
     // Create test data with clear increasing trend
     QList<Measurement> measurements;
     QDateTime now = QDateTime::currentDateTime();
     
     measurements.append(Measurement(1, "PM10", now, 10.0));
     measurements.append(Measurement(1, "PM10", now.addSecs(3600), 15.0));
     measurements.append(Measurement(1, "PM10", now.addSecs(7200), 20.0));
     measurements.append(Measurement(1, "PM10", now.addSecs(10800), 25.0));
     measurements.append(Measurement(1, "PM10", now.addSecs(14400), 30.0));
     
     // Analyze the data
     AnalysisResult result = analyzer->analyze(measurements);
     
     // Verify the trend
     QCOMPARE(result.trend, TrendType::Increasing);
 }
 
 void TestDataAnalyzer::testDecreasingTrend()
 {
     // Create test data with clear decreasing trend
     QList<Measurement> measurements;
     QDateTime now = QDateTime::currentDateTime();
     
     measurements.append(Measurement(1, "PM10", now, 50.0));
     measurements.append(Measurement(1, "PM10", now.addSecs(3600), 40.0));
     measurements.append(Measurement(1, "PM10", now.addSecs(7200), 30.0));
     measurements.append(Measurement(1, "PM10", now.addSecs(10800), 20.0));
     measurements.append(Measurement(1, "PM10", now.addSecs(14400), 10.0));
     
     // Analyze the data
     AnalysisResult result = analyzer->analyze(measurements);
     
     // Verify the trend
     QCOMPARE(result.trend, TrendType::Decreasing);
 }
 
 void TestDataAnalyzer::testStableTrend()
 {
     // Create test data with stable values
     QList<Measurement> measurements;
     QDateTime now = QDateTime::currentDateTime();
     
     measurements.append(Measurement(1, "PM10", now, 20.0));
     measurements.append(Measurement(1, "PM10", now.addSecs(3600), 20.1));
     measurements.append(Measurement(1, "PM10", now.addSecs(7200), 20.05));
     measurements.append(Measurement(1, "PM10", now.addSecs(10800), 19.95));
     measurements.append(Measurement(1, "PM10", now.addSecs(14400), 20.05));
     
     // Analyze the data
     AnalysisResult result = analyzer->analyze(measurements);
     
     // Verify the trend
     QCOMPARE(result.trend, TrendType::Stable);
 }
 
 void TestDataAnalyzer::testFluctuatingTrend()
 {
     // Create test data with highly fluctuating values
     QList<Measurement> measurements;
     QDateTime now = QDateTime::currentDateTime();
     
     measurements.append(Measurement(1, "PM10", now, 10.0));
     measurements.append(Measurement(1, "PM10", now.addSecs(3600), 40.0));
     measurements.append(Measurement(1, "PM10", now.addSecs(7200), 15.0));
     measurements.append(Measurement(1, "PM10", now.addSecs(10800), 35.0));
     measurements.append(Measurement(1, "PM10", now.addSecs(14400), 5.0));
     
     // Analyze the data
     AnalysisResult result = analyzer->analyze(measurements);
     
     // Verify the trend
     QCOMPARE(result.trend, TrendType::Fluctuating);
 }
 
 void TestDataAnalyzer::testEmptyData()
 {
     // Create empty data
     QList<Measurement> measurements;
     
     // Analyze the data
     AnalysisResult result = analyzer->analyze(measurements);
     
     // Verify that default values are returned
     QCOMPARE(result.minValue, 0.0);
     QCOMPARE(result.maxValue, 0.0);
     QCOMPARE(result.avgValue, 0.0);
     QCOMPARE(result.trend, TrendType::Unknown);
 }
 
 void TestDataAnalyzer::testInvalidMeasurements()
 {
     // Create test data with invalid measurements
     QList<Measurement> measurements;
     QDateTime now = QDateTime::currentDateTime();
     
     Measurement invalidMeasurement(1, "PM10", now, 0.0);
     invalidMeasurement.isValid(); // This should normally be false for an invalid measurement
     
     measurements.append(invalidMeasurement);
     measurements.append(Measurement(1, "PM10", now.addSecs(3600), 20.0));
     
     // Analyze the data (should only consider the valid measurement)
     AnalysisResult result = analyzer->analyze(measurements);
     
     // Verify the results
     QCOMPARE(result.minValue, 20.0);
     QCOMPARE(result.maxValue, 20.0);
     QCOMPARE(result.avgValue, 20.0);
 }
 
 void TestDataAnalyzer::cleanupTestCase()
 {
     delete analyzer;
 }
 
 QTEST_MAIN(TestDataAnalyzer)
 #include "test_dataanalyzer.moc"