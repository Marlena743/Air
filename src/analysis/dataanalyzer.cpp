/**
 * @file dataanalyzer.cpp
 * @brief Implementation of the DataAnalyzer class
 */

 #include "src/analysis/dataanalyzer.h"

 DataAnalyzer::DataAnalyzer(QObject *parent) : QObject(parent)
 {
 }
 
 AnalysisResult DataAnalyzer::analyze(const QList<Measurement> &measurements)
 {
     AnalysisResult result;
     
     // Get valid measurements
     QList<Measurement> validMeasurements = getValidMeasurements(measurements);
     
     // If there are no valid measurements, return default result
     if (validMeasurements.isEmpty()) {
         return result;
     }
     
     // Calculate metrics
     calculateMinimum(validMeasurements, result);
     calculateMaximum(validMeasurements, result);
     calculateAverage(validMeasurements, result);
     calculateTrend(validMeasurements, result);
     
     return result;
 }
 
 QString DataAnalyzer::getTrendDescription(TrendType trend)
 {
     switch (trend) {
         case TrendType::Increasing:
             return "Wzrostowy";
         case TrendType::Decreasing:
             return "Malejący";
         case TrendType::Stable:
             return "Stabilny";
         case TrendType::Fluctuating:
             return "Zmienny";
         case TrendType::Unknown:
         default:
             return "Nieznany";
     }
 }
 
 void DataAnalyzer::calculateMinimum(const QList<Measurement> &measurements, AnalysisResult &result)
 {
     // Initialize with the first measurement
     result.minValue = measurements.first().getValue();
     result.minDate = measurements.first().getDate();
     
     // Find the minimum value
     for (const Measurement &measurement : measurements) {
         if (measurement.getValue() < result.minValue) {
             result.minValue = measurement.getValue();
             result.minDate = measurement.getDate();
         }
     }
 }
 
 void DataAnalyzer::calculateMaximum(const QList<Measurement> &measurements, AnalysisResult &result)
 {
     // Initialize with the first measurement
     result.maxValue = measurements.first().getValue();
     result.maxDate = measurements.first().getDate();
     
     // Find the maximum value
     for (const Measurement &measurement : measurements) {
         if (measurement.getValue() > result.maxValue) {
             result.maxValue = measurement.getValue();
             result.maxDate = measurement.getDate();
         }
     }
 }
 
 void DataAnalyzer::calculateAverage(const QList<Measurement> &measurements, AnalysisResult &result)
 {
     double sum = 0.0;
     
     // Sum all values
     for (const Measurement &measurement : measurements) {
         sum += measurement.getValue();
     }
     
     // Calculate average
     result.avgValue = sum / measurements.size();
 }
 
 void DataAnalyzer::calculateTrend(const QList<Measurement> &measurements, AnalysisResult &result)
 {
     // Need at least 2 measurements to calculate trend
     if (measurements.size() < 2) {
         result.trend = TrendType::Unknown;
         return;
     }
     
     // Sort measurements by date
     QList<Measurement> sortedMeasurements = measurements;
     std::sort(sortedMeasurements.begin(), sortedMeasurements.end(), 
              [](const Measurement &a, const Measurement &b) {
                  return a.getDate() < b.getDate();
              });
     
     // Calculate linear regression
     double sumX = 0.0;
     double sumY = 0.0;
     double sumXY = 0.0;
     double sumX2 = 0.0;
     double n = sortedMeasurements.size();
     
     // Use the first date as reference (x=0)
     QDateTime firstDate = sortedMeasurements.first().getDate();
     
     for (int i = 0; i < n; ++i) {
         double x = firstDate.secsTo(sortedMeasurements[i].getDate()) / 3600.0; // hours
         double y = sortedMeasurements[i].getValue();
         
         sumX += x;
         sumY += y;
         sumXY += x * y;
         sumX2 += x * x;
     }
     
     // Calculate slope
     double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
     
     // Determine trend based on slope
     if (std::abs(slope) < 0.001) { // Near zero
         result.trend = TrendType::Stable;
     } else if (slope > 0) {
         result.trend = TrendType::Increasing;
     } else {
         result.trend = TrendType::Decreasing;
     }
     
     // Check for fluctuations
     double meanValue = sumY / n;
     double sumDeviation = 0.0;
     
     for (const Measurement &measurement : sortedMeasurements) {
         double deviation = std::abs(measurement.getValue() - meanValue);
         sumDeviation += deviation;
     }
     
     double meanDeviation = sumDeviation / n;
     double relativeDeviation = meanDeviation / meanValue;
     
     // If the relative deviation is large, it's fluctuating
     if (relativeDeviation > 0.2) { // 20% threshold
         result.trend = TrendType::Fluctuating;
     }
 }
 
 QList<Measurement> DataAnalyzer::getValidMeasurements(const QList<Measurement> &measurements)
 {
     QList<Measurement> validMeasurements;
     
     for (const Measurement &measurement : measurements) {
         if (measurement.isValid()) {
             validMeasurements.append(measurement);
         }
     }
     
     return validMeasurements;
 }
