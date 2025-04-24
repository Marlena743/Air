/**
 * @file dataanalyzer.h
 * @brief Header file for the DataAnalyzer class
 */

 #ifndef DATAANALYZER_H
 #define DATAANALYZER_H
 
 #include <QObject>
 #include <QList>
 #include <QPair>
 #include <QDateTime>
 #include <algorithm>
 #include <numeric>
 
 #include "../data/measurement.h"
 
 /**
  * @enum TrendType
  * @brief Represents the trend of measurement values
  */
 enum class TrendType {
     Increasing,
     Decreasing,
     Stable,
     Fluctuating,
     Unknown
 };
 
 /**
  * @class AnalysisResult
  * @brief Contains the results of data analysis
  */
 class AnalysisResult
 {
 public:
     double minValue;
     QDateTime minDate;
     double maxValue;
     QDateTime maxDate;
     double avgValue;
     TrendType trend;
     
     /**
      * @brief Default constructor
      */
     AnalysisResult() : 
         minValue(0.0), 
         maxValue(0.0), 
         avgValue(0.0), 
         trend(TrendType::Unknown) {}
 };
 
 /**
  * @class DataAnalyzer
  * @brief Analyzes measurement data
  */
 class DataAnalyzer : public QObject
 {
     Q_OBJECT
 
 public:
     /**
      * @brief Constructor
      * @param parent Parent object
      */
     explicit DataAnalyzer(QObject *parent = nullptr);
     
     /**
      * @brief Analyzes measurement data
      * @param measurements List of measurements
      * @return Analysis results
      */
     AnalysisResult analyze(const QList<Measurement> &measurements);
     
     /**
      * @brief Gets a textual representation of a trend
      * @param trend Trend type
      * @return Trend description
      */
     static QString getTrendDescription(TrendType trend);
 
 private:
     /**
      * @brief Calculates the minimum value and its date
      * @param measurements List of measurements
      * @param result Analysis results
      */
     void calculateMinimum(const QList<Measurement> &measurements, AnalysisResult &result);
     
     /**
      * @brief Calculates the maximum value and its date
      * @param measurements List of measurements
      * @param result Analysis results
      */
     void calculateMaximum(const QList<Measurement> &measurements, AnalysisResult &result);
     
     /**
      * @brief Calculates the average value
      * @param measurements List of measurements
      * @param result Analysis results
      */
     void calculateAverage(const QList<Measurement> &measurements, AnalysisResult &result);
     
     /**
      * @brief Calculates the trend
      * @param measurements List of measurements
      * @param result Analysis results
      */
     void calculateTrend(const QList<Measurement> &measurements, AnalysisResult &result);
     
     /**
      * @brief Gets valid measurements
      * @param measurements List of measurements
      * @return List of valid measurements
      */
     QList<Measurement> getValidMeasurements(const QList<Measurement> &measurements);
 };
 
 #endif // DATAANALYZER_H