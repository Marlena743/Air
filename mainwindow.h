/**
 * @file mainwindow.h
 * @brief Header file for the main application window
 */

 #ifndef MAINWINDOW_H
 #define MAINWINDOW_H
 #include "api/restapi.h"
 #include "data/station.h"
 #include "data/sensor.h"
 #include "data/measurement.h"
 #include "database/jsonrepository.h"
 #include "analysis/dataanalyzer.h"
 #include <QMainWindow>
 
 #include <QChart>
 #include <QChartView>
 #include <QLineSeries>
 
 #include <QComboBox>
 #include <QLineEdit>
 #include <QPushButton>
 #include <QLabel>
 #include <QTableWidget>
 #include <QDateTimeEdit>
 #include <QDateTimeAxis>
 #include <QValueAxis>
 #include <QProgressBar>
 #include <QGroupBox>
 #include <QMessageBox>
 #include <QVBoxLayout>
 #include <QHBoxLayout>
 #include <QFormLayout>
 #include <QGridLayout>
 #include <QStatusBar>
 #include <QThread>
 #include <QMutex>
 #include <QWaitCondition>
 #include <QApplication>
 #include <QHeaderView>
 
 /**
   * @class MainWindow
   * @brief The main window of the application
   */
 class MainWindow : public QMainWindow
 {
     Q_OBJECT
 
 public:
     /**
       * @brief Constructor
       * @param parent Parent widget
       */
     explicit MainWindow(QWidget *parent = nullptr);
 
     /**
       * @brief Destructor
       */
     ~MainWindow();
 
 public slots:
     /**
       * @brief Displays a message in the status bar
       * @param message Message to display
       */
     void showStatusMessage(const QString &message);
 
 private slots:
     /**
       * @brief Loads all stations from the API
       */
     void loadStations();
 
     /**
       * @brief Filters stations by city name
       */
     void filterStationsByCity();
 
     /**
       * @brief Loads sensors for the selected station
       */
     void loadSensors();
 
     /**
       * @brief Loads measurements for the selected sensor
       */
     void loadMeasurements();
 
     /**
       * @brief Saves measurements to the local database
       */
     void saveMeasurementsToDatabase();
 
     /**
       * @brief Loads measurements from the local database
       */
     void loadMeasurementsFromDatabase();
 
     /**
       * @brief Analyzes the loaded measurements
       */
     void analyzeMeasurements();
 
     /**
       * @brief Updates the chart with current measurements
       */
     void updateChart();
 
     /**
       * @brief Updates the date range for the chart
       */
     void updateDateRange();
 
     /**
       * @brief Handles API request errors
       * @param error Error message
       */
     void handleApiError(const QString &error);
 
     /**
       * @brief Handles database errors
       * @param error Error message
       */
     void handleDatabaseError(const QString &error);
 
 private:
     // UI components
     QComboBox *stationComboBox;
     QLineEdit *cityFilterEdit;
     QPushButton *filterButton;
     QPushButton *loadStationsButton;
     QTableWidget *stationTable;
 
     QComboBox *sensorComboBox;
     QPushButton *loadSensorsButton;
     QTableWidget *sensorTable;
 
     QPushButton *loadMeasurementsButton;
     QPushButton *saveMeasurementsButton;
     QPushButton *loadFromDatabaseButton;
     QDateTimeEdit *startDateEdit;
     QDateTimeEdit *endDateEdit;
     QPushButton *updateDateRangeButton;
 
     QChartView *chartView;
     QChart *chart;
     QLineSeries *series;
 
     QTableWidget *measurementTable;
     QGroupBox *analysisGroupBox;
     QLabel *minValueLabel;
     QLabel *maxValueLabel;
     QLabel *avgValueLabel;
     QLabel *trendLabel;
     QLabel *minDateLabel;
     QLabel *maxDateLabel;
 
     QProgressBar *progressBar;
 
     // Data components
     RestApi *api;
     JsonRepository *repository;
     DataAnalyzer *analyzer;
 
     QList<Station> stations;
     QList<Sensor> sensors;
     QList<Measurement> measurements;
 
     QThread workerThread;
     QMutex mutex;
     QWaitCondition condition;
     bool abort;
     bool offlineMode;
     QLabel *modeLabel;
     
     /**
      * @brief Updates UI with stations data
      */
     void updateStationsUI();
     
     /**
      * @brief Updates UI with sensors data
      */
     void updateSensorsUI();
     
     /**
      * @brief Updates UI with measurements data
      */
     void updateMeasurementsUI();
     
     /**
      * @brief Checks network status and updates offline mode flag
      */
     void checkNetworkStatus();
     /**
       * @brief Sets up the UI components
       */
     void setupUi();
 
     /**
       * @brief Creates the chart
       */
     void createChart();
 
     /**
       * @brief Applies a custom style to the application
       */
     void applyCustomStyle();
 
     /**
       * @brief Centers the window on the screen
       */
     void centerOnScreen();
 };
 
 #endif // MAINWINDOW_H
 