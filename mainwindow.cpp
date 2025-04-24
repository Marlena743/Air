/**
 * @file mainwindow.cpp
 * @brief Implementation of the main application window
 */

 #include "mainwindow.h"
 #include <QDateTime>
 #include <QMessageBox>
 #include <QThread>
 
 #include <QApplication>
 #include <QHeaderView>
 #include <QFormLayout>
 #include <QGroupBox>
 #include <QHBoxLayout>
 #include <QVBoxLayout>
 #include <QLabel>
 #include <QLineEdit>
 #include <QPushButton>
 #include <QComboBox>
 #include <QTableWidget>
 #include <QDateTimeEdit>
 #include <QProgressBar>
 #include <QStatusBar>
 #include <QPalette>
 #include <QBrush>
 #include <QFont>
 #include <QColor>
 #include <QStyle>
 #include <QTcpSocket>
 
 MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), abort(false), offlineMode(false)
 {
     // Create objects
     api = new RestApi(this);
     repository = new JsonRepository(this);
     analyzer = new DataAnalyzer(this);
 
     // Set up UI
     setupUi();
 
     // Connect signals and slots
     connect(loadStationsButton, &QPushButton::clicked, this, &MainWindow::loadStations);
     connect(filterButton, &QPushButton::clicked, this, &MainWindow::filterStationsByCity);
     connect(stationComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::loadSensors);
     connect(loadSensorsButton, &QPushButton::clicked, this, &MainWindow::loadSensors);
     connect(loadMeasurementsButton, &QPushButton::clicked, this, &MainWindow::loadMeasurements);
     connect(saveMeasurementsButton, &QPushButton::clicked, this, &MainWindow::saveMeasurementsToDatabase);
     connect(loadFromDatabaseButton, &QPushButton::clicked, this, &MainWindow::loadMeasurementsFromDatabase);
     connect(updateDateRangeButton, &QPushButton::clicked, this, &MainWindow::updateDateRange);
 
     // Connect API signals
     connect(api, &RestApi::stationsReceived, this, [this](const QList<Station> &receivedStations) {
         stations = receivedStations;
 
         // Save to local database
         repository->saveStations(stations);
 
         // Update UI
         updateStationsUI();
 
         showStatusMessage("Loaded " + QString::number(stations.size()) + " stations");
     });
 
     connect(api, &RestApi::sensorsReceived, this, [this](const QList<Sensor> &receivedSensors) {
         sensors = receivedSensors;
 
         // Save to local database
         repository->saveSensors(sensors);
 
         // Update UI
         updateSensorsUI();
 
         showStatusMessage("Loaded " + QString::number(sensors.size()) + " sensors");
     });
 
     connect(api, &RestApi::measurementsReceived, this, [this](const QList<Measurement> &receivedMeasurements) {
         measurements = receivedMeasurements;
 
         // Update UI
         updateMeasurementsUI();
 
         showStatusMessage("Loaded " + QString::number(measurements.size()) + " measurements");
     });
 
     connect(api, &RestApi::errorOccurred, this, &MainWindow::handleApiError);
     connect(repository, &JsonRepository::errorOccurred, this, &MainWindow::handleDatabaseError);
 
     // Set initial dates
     QDateTime now = QDateTime::currentDateTime();
     endDateEdit->setDateTime(now);
     startDateEdit->setDateTime(now.addDays(-7));
 
     // Create the chart
     createChart();
 
     // Check network status on startup
     checkNetworkStatus();
 
     showStatusMessage("Ready");
 }
 
 MainWindow::~MainWindow()
 {
     // Stop the worker thread if it's running
     if (workerThread.isRunning()) {
         abort = true;
         condition.wakeAll();
         workerThread.wait();
     }
 }
 
 void MainWindow::loadStations()
 {
     showStatusMessage("Loading stations...");
 
     // Check if offline mode
     if (offlineMode) {
         QList<Station> loadedStations = repository->loadStations();
         if (!loadedStations.isEmpty()) {
             stations = loadedStations;
             updateStationsUI();
             showStatusMessage("Loaded " + QString::number(stations.size()) + " stations from local database");
         } else {
             showStatusMessage("No stations found in local database");
             QMessageBox::warning(this, "Offline Mode",
                                  "No stations data available in offline mode. Please connect to the internet.");
         }
     } else {
         api->getAllStations();
     }
 }
 
 void MainWindow::filterStationsByCity()
 {
     QString cityName = cityFilterEdit->text().trimmed();
     if (cityName.isEmpty()) {
         QMessageBox::warning(this, "Filter Error", "Please enter a city name");
         return;
     }
 
     showStatusMessage("Filtering stations by city: " + cityName);
     api->getStationsByCity(cityName);
 }
 
 void MainWindow::loadSensors()
 {
     int currentStationIndex = stationComboBox->currentIndex();
     if (currentStationIndex < 0) {
         QMessageBox::warning(this, "Load Error", "Please select a station first");
         return;
     }
 
     int stationId = stationComboBox->itemData(currentStationIndex).toInt();
     showStatusMessage("Loading sensors for station ID: " + QString::number(stationId));
 
     // Check if offline mode
     if (offlineMode) {
         QList<Sensor> loadedSensors = repository->loadSensors(stationId);
         if (!loadedSensors.isEmpty()) {
             sensors = loadedSensors;
             updateSensorsUI();
             showStatusMessage("Loaded " + QString::number(sensors.size()) + " sensors from local database");
         } else {
             showStatusMessage("No sensors found in local database for station ID: " + QString::number(stationId));
             QMessageBox::warning(this, "Offline Mode",
                                  "No sensor data available in offline mode for this station. Please connect to the internet.");
         }
     } else {
         // Add temporary connection for error handling
         auto *connection = new QMetaObject::Connection;
         *connection = connect(api, &RestApi::errorOccurred, this, [this, stationId, connection](const QString &error) {
             Q_UNUSED(error);
             disconnect(*connection);
             delete connection;
 
             // Switch to offline mode and try loading from database
             offlineMode = true;
             modeLabel->setText("Offline Mode");
             modeLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
 
             QList<Sensor> loadedSensors = repository->loadSensors(stationId);
             if (!loadedSensors.isEmpty()) {
                 sensors = loadedSensors;
                 updateSensorsUI();
                 showStatusMessage("Loaded " + QString::number(sensors.size()) + " sensors from local database");
             } else {
                 showStatusMessage("No sensors found in local database for station ID: " + QString::number(stationId));
             }
         });
 
         api->getSensors(stationId);
     }
 }
 
 void MainWindow::loadMeasurements()
 {
     int currentSensorIndex = sensorComboBox->currentIndex();
     if (currentSensorIndex < 0) {
         QMessageBox::warning(this, "Load Error", "Please select a sensor first");
         return;
     }
 
     int sensorId = sensorComboBox->itemData(currentSensorIndex).toInt();
     QDateTime startDate = startDateEdit->dateTime();
     QDateTime endDate = endDateEdit->dateTime();
 
     showStatusMessage("Loading measurements for sensor ID: " + QString::number(sensorId));
 
     // Check if offline mode
     if (offlineMode) {
         QList<Measurement> loadedMeasurements = repository->loadMeasurements(sensorId, startDate, endDate);
         if (!loadedMeasurements.isEmpty()) {
             measurements = loadedMeasurements;
             updateMeasurementsUI();
             showStatusMessage("Loaded " + QString::number(measurements.size()) + " measurements from local database");
         } else {
             showStatusMessage("No measurements found in local database for sensor ID: " + QString::number(sensorId));
             QMessageBox::warning(this, "Offline Mode",
                                  "No measurement data available in offline mode for this sensor. Please connect to the internet.");
         }
     } else {
         // Add temporary connection for error handling
         auto *connection = new QMetaObject::Connection;
         *connection = connect(api, &RestApi::errorOccurred, this, [this, sensorId, startDate, endDate, connection](const QString &error) {
             Q_UNUSED(error);
             disconnect(*connection);
             delete connection;
 
             // Switch to offline mode and try loading from database
             offlineMode = true;
             modeLabel->setText("Offline Mode");
             modeLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
 
             QList<Measurement> loadedMeasurements = repository->loadMeasurements(sensorId, startDate, endDate);
             if (!loadedMeasurements.isEmpty()) {
                 measurements = loadedMeasurements;
                 updateMeasurementsUI();
                 showStatusMessage("Loaded " + QString::number(measurements.size()) + " measurements from local database");
             } else {
                 showStatusMessage("No measurements found in local database for sensor ID: " + QString::number(sensorId));
             }
         });
 
         api->getMeasurements(sensorId);
     }
 }
 
 void MainWindow::saveMeasurementsToDatabase()
 {
     if (measurements.isEmpty()) {
         QMessageBox::warning(this, "Save Error", "No measurements to save");
         return;
     }
 
     int currentSensorIndex = sensorComboBox->currentIndex();
     if (currentSensorIndex < 0) {
         QMessageBox::warning(this, "Save Error", "Please select a sensor first");
         return;
     }
 
     int sensorId = sensorComboBox->itemData(currentSensorIndex).toInt();
 
     showStatusMessage("Saving measurements for sensor ID: " + QString::number(sensorId) + "...");
 
     // Use a worker thread for database operations
     QThread *thread = QThread::create([this, sensorId]() {
         bool success = repository->saveMeasurements(measurements, sensorId);
         if (success) {
             QMetaObject::invokeMethod(this, "showStatusMessage",
                                       Q_ARG(QString, "Measurements saved successfully"));
         }
     });
 
     connect(thread, &QThread::finished, thread, &QThread::deleteLater);
     thread->start();
 }
 
 void MainWindow::loadMeasurementsFromDatabase()
 {
     int currentSensorIndex = sensorComboBox->currentIndex();
     if (currentSensorIndex < 0) {
         QMessageBox::warning(this, "Load Error", "Please select a sensor first");
         return;
     }
 
     int sensorId = sensorComboBox->itemData(currentSensorIndex).toInt();
     QDateTime startDate = startDateEdit->dateTime();
     QDateTime endDate = endDateEdit->dateTime();
 
     showStatusMessage("Loading measurements from database for sensor ID: " + QString::number(sensorId) + "...");
 
     // Use a worker thread for database operations
     QThread *thread = QThread::create([this, sensorId, startDate, endDate]() {
         QList<Measurement> loadedMeasurements = repository->loadMeasurements(sensorId, startDate, endDate);
 
         QMetaObject::invokeMethod(this, [this, loadedMeasurements]() {
             measurements = loadedMeasurements;
             updateMeasurementsUI();
             showStatusMessage("Loaded " + QString::number(measurements.size()) + " measurements from database");
         });
     });
 
     connect(thread, &QThread::finished, thread, &QThread::deleteLater);
     thread->start();
 }
 
 void MainWindow::analyzeMeasurements()
 {
     if (measurements.isEmpty()) {
         minValueLabel->setText("N/A");
         maxValueLabel->setText("N/A");
         avgValueLabel->setText("N/A");
         trendLabel->setText("N/A");
         minDateLabel->setText("N/A");
         maxDateLabel->setText("N/A");
         return;
     }
 
     // Analyze measurements in a separate thread
     QThread *thread = QThread::create([this]() {
         AnalysisResult result = analyzer->analyze(measurements);
 
         QMetaObject::invokeMethod(this, [this, result]() {
             minValueLabel->setText(QString::number(result.minValue));
             maxValueLabel->setText(QString::number(result.maxValue));
             avgValueLabel->setText(QString::number(result.avgValue, 'f', 2));
             trendLabel->setText(DataAnalyzer::getTrendDescription(result.trend));
             minDateLabel->setText(result.minDate.toString("yyyy-MM-dd hh:mm"));
             maxDateLabel->setText(result.maxDate.toString("yyyy-MM-dd hh:mm"));
         });
     });
 
     connect(thread, &QThread::finished, thread, &QThread::deleteLater);
     thread->start();
 }
 
 void MainWindow::updateChart()
 {
     // Clear the current data
     series->clear();
 
     // Add data points to the series
     for (const Measurement &measurement : measurements) {
         if (measurement.isValid()) {
             series->append(measurement.getDate().toMSecsSinceEpoch(), measurement.getValue());
         }
     }
 
     // If there are no valid data points, return
     if (series->count() == 0) {
         return;
     }
 
     // Remove old axes and series
     chart->removeSeries(series);
     QList<QAbstractAxis*> axes = chart->axes();
     for (QAbstractAxis* axis : axes) {
         chart->removeAxis(axis);
         delete axis;
     }
 
     // Add series back
     chart->addSeries(series);
 
     // Set up the axes
     QDateTimeAxis *axisX = new QDateTimeAxis();
     axisX->setFormat("yyyy-MM-dd hh:mm");
     axisX->setTitleText("Data i czas");
 
     QValueAxis *axisY = new QValueAxis();
     axisY->setTitleText("Wartość");
 
     chart->addAxis(axisX, Qt::AlignBottom);
     chart->addAxis(axisY, Qt::AlignLeft);
     series->attachAxis(axisX);
     series->attachAxis(axisY);
 
     // Set the range of the axes
     QDateTime minDate = QDateTime::fromMSecsSinceEpoch(series->at(0).x());
     QDateTime maxDate = QDateTime::fromMSecsSinceEpoch(series->at(series->count() - 1).x());
 
     axisX->setRange(minDate, maxDate);
 
     double minValue = series->at(0).y();
     double maxValue = series->at(0).y();
 
     for (int i = 1; i < series->count(); ++i) {
         minValue = qMin(minValue, series->at(i).y());
         maxValue = qMax(maxValue, series->at(i).y());
     }
 
     // Add a small margin
     minValue = qMax(0.0, minValue - 0.1 * (maxValue - minValue));
     maxValue = maxValue + 0.1 * (maxValue - minValue);
 
     axisY->setRange(minValue, maxValue);
 
     // Update the chart
     chartView->update();
 }
 
 void MainWindow::updateDateRange()
 {
     QDateTime startDate = startDateEdit->dateTime();
     QDateTime endDate = endDateEdit->dateTime();
 
     if (startDate > endDate) {
         QMessageBox::warning(this, "Date Range Error", "Start date must be before end date");
         return;
     }
 
     // Filter measurements by date range
     QList<Measurement> filteredMeasurements;
 
     for (const Measurement &measurement : measurements) {
         if (measurement.getDate() >= startDate && measurement.getDate() <= endDate) {
             filteredMeasurements.append(measurement);
         }
     }
 
     if (filteredMeasurements.isEmpty()) {
         QMessageBox::warning(this, "Date Range Error", "No measurements in the selected date range");
         return;
     }
 
     measurements = filteredMeasurements;
 
     // Update UI
     updateMeasurementsUI();
 
     showStatusMessage("Filtered measurements by date range");
 }
 
 void MainWindow::handleApiError(const QString &error)
 {
     showStatusMessage("API Error: " + error);
 
     // Switch to offline mode
     offlineMode = true;
     modeLabel->setText("Offline Mode");
     modeLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
 
     QMessageBox::warning(this, "Network Error",
                          error + "\n\nSwitching to offline mode. Data will be loaded from local database.");
 
     // If no stations are loaded, try loading from database
     if (stations.isEmpty()) {
         QMessageBox::StandardButton reply = QMessageBox::question(
             this, "Load from Database",
             "Would you like to load stations from the local database?",
             QMessageBox::Yes | QMessageBox::No);
 
         if (reply == QMessageBox::Yes) {
             QList<Station> loadedStations = repository->loadStations();
             if (!loadedStations.isEmpty()) {
                 stations = loadedStations;
                 updateStationsUI();
                 showStatusMessage("Loaded " + QString::number(stations.size()) + " stations from database");
             } else {
                 showStatusMessage("No stations found in the database");
             }
         }
     }
 }
 
 void MainWindow::handleDatabaseError(const QString &error)
 {
     showStatusMessage("Database Error: " + error);
     QMessageBox::critical(this, "Database Error", error);
 }
 
 void MainWindow::setupUi()
 {
     // Ustaw styl aplikacji
     QApplication::setStyle("Fusion");
 
     // Zastosuj paletę kolorów
     QPalette palette;
     palette.setColor(QPalette::Window, QColor(240, 240, 245));
     palette.setColor(QPalette::WindowText, QColor(50, 50, 50));
     palette.setColor(QPalette::Base, QColor(255, 255, 255));
     palette.setColor(QPalette::AlternateBase, QColor(230, 235, 245));
     palette.setColor(QPalette::Button, QColor(100, 130, 190));
     palette.setColor(QPalette::ButtonText, QColor(0, 0, 128));
     palette.setColor(QPalette::Highlight, QColor(70, 110, 180));
     palette.setColor(QPalette::HighlightedText, Qt::white);
     QApplication::setPalette(palette);
 
     // Widget główny
     QWidget *centralWidget = new QWidget(this);
     setCentralWidget(centralWidget);
 
     // Główny układ
     QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
     mainLayout->setSpacing(8);
     mainLayout->setContentsMargins(8, 8, 8, 8);
 
     // Górna część: Stacje i sensory w układzie poziomym
     QWidget *topWidget = new QWidget(centralWidget);
     QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
     topLayout->setSpacing(8);
     topLayout->setContentsMargins(0, 0, 0, 0);
 
     // Lewa strona - Stacje
     QGroupBox *stationGroupBox = new QGroupBox("Stacje pomiarowe", topWidget);
     stationGroupBox->setStyleSheet("QGroupBox { font-weight: bold; }");
     QVBoxLayout *stationLayout = new QVBoxLayout(stationGroupBox);
     stationLayout->setSpacing(6);
 
     QHBoxLayout *stationFilterLayout = new QHBoxLayout();
     QLabel *cityFilterLabel = new QLabel("Miasto:", stationGroupBox);
     cityFilterEdit = new QLineEdit(stationGroupBox);
     cityFilterEdit->setPlaceholderText("Wprowadź nazwę miasta");
     filterButton = new QPushButton("Filtruj", stationGroupBox);
     loadStationsButton = new QPushButton("Załaduj wszystkie", stationGroupBox);
 
     // Zastosuj stylizację przycisków
     QString buttonStyle = "QPushButton { padding: 4px 8px; border-radius: 3px; }";
     filterButton->setStyleSheet(buttonStyle);
     loadStationsButton->setStyleSheet(buttonStyle);
 
     stationFilterLayout->addWidget(cityFilterLabel);
     stationFilterLayout->addWidget(cityFilterEdit, 3);
     stationFilterLayout->addWidget(filterButton, 1);
     stationFilterLayout->addWidget(loadStationsButton, 1);
     stationLayout->addLayout(stationFilterLayout);
 
     QLabel *stationLabel = new QLabel("Wybierz stację:", stationGroupBox);
     stationComboBox = new QComboBox(stationGroupBox);
     stationComboBox->setStyleSheet("QComboBox { padding: 3px; }");
 
     QHBoxLayout *stationSelectionLayout = new QHBoxLayout();
     stationSelectionLayout->addWidget(stationLabel);
     stationSelectionLayout->addWidget(stationComboBox);
     stationLayout->addLayout(stationSelectionLayout);
 
     stationTable = new QTableWidget(stationGroupBox);
     stationTable->setColumnCount(4);
     stationTable->setHorizontalHeaderLabels({"ID", "Nazwa", "Miasto", "Adres"});
     stationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
     stationTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
     stationTable->setAlternatingRowColors(true);
     stationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
     stationTable->setStyleSheet("QTableView { gridline-color: #d0d0d0; }");
     stationTable->setMaximumHeight(150);  // Ograniczenie wysokości tabeli
 
     stationLayout->addWidget(stationTable);
 
     // Prawa strona - Sensory
     QGroupBox *sensorGroupBox = new QGroupBox("Stanowiska pomiarowe", topWidget);
     sensorGroupBox->setStyleSheet("QGroupBox { font-weight: bold; }");
     QVBoxLayout *sensorLayout = new QVBoxLayout(sensorGroupBox);
     sensorLayout->setSpacing(6);
 
     QHBoxLayout *sensorSelectionLayout = new QHBoxLayout();
     QLabel *sensorLabel = new QLabel("Wybierz sensor:", sensorGroupBox);
     sensorComboBox = new QComboBox(sensorGroupBox);
     sensorComboBox->setStyleSheet("QComboBox { padding: 3px; }");
     loadSensorsButton = new QPushButton("Załaduj sensory", sensorGroupBox);
     loadSensorsButton->setStyleSheet(buttonStyle);
 
     sensorSelectionLayout->addWidget(sensorLabel);
     sensorSelectionLayout->addWidget(sensorComboBox, 3);
     sensorSelectionLayout->addWidget(loadSensorsButton, 1);
     sensorLayout->addLayout(sensorSelectionLayout);
 
     sensorTable = new QTableWidget(sensorGroupBox);
     sensorTable->setColumnCount(4);
     sensorTable->setHorizontalHeaderLabels({"ID", "Parametr", "Formuła", "Kod"});
     sensorTable->setSelectionBehavior(QAbstractItemView::SelectRows);
     sensorTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
     sensorTable->setAlternatingRowColors(true);
     sensorTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
     sensorTable->setStyleSheet("QTableView { gridline-color: #d0d0d0; }");
     sensorTable->setMaximumHeight(150);  // Ograniczenie wysokości tabeli
 
     sensorLayout->addWidget(sensorTable);
 
     // Dodaj grupy do górnego układu
     topLayout->addWidget(stationGroupBox);
     topLayout->addWidget(sensorGroupBox);
 
     // Środkowa część: Kontrolki pomiarów
     QGroupBox *controlGroupBox = new QGroupBox("Sterowanie pomiarami", centralWidget);
     controlGroupBox->setStyleSheet("QGroupBox { font-weight: bold; }");
     QHBoxLayout *controlLayout = new QHBoxLayout(controlGroupBox);
     controlLayout->setSpacing(10);
 
     // Lewa strona - Zakres dat
     QGroupBox *dateRangeGroupBox = new QGroupBox("Zakres dat", controlGroupBox);
     QHBoxLayout *dateRangeLayout = new QHBoxLayout(dateRangeGroupBox);
     dateRangeLayout->setSpacing(5);
 
     QLabel *startDateLabel = new QLabel("Od:", dateRangeGroupBox);
     startDateEdit = new QDateTimeEdit(dateRangeGroupBox);
     startDateEdit->setCalendarPopup(true);
     startDateEdit->setDisplayFormat("yyyy-MM-dd hh:mm");
     startDateEdit->setStyleSheet("QDateTimeEdit { padding: 3px; }");
 
     QLabel *endDateLabel = new QLabel("Do:", dateRangeGroupBox);
     endDateEdit = new QDateTimeEdit(dateRangeGroupBox);
     endDateEdit->setCalendarPopup(true);
     endDateEdit->setDisplayFormat("yyyy-MM-dd hh:mm");
     endDateEdit->setStyleSheet("QDateTimeEdit { padding: 3px; }");
 
     updateDateRangeButton = new QPushButton("Aktualizuj", dateRangeGroupBox);
     updateDateRangeButton->setStyleSheet(buttonStyle);
 
     dateRangeLayout->addWidget(startDateLabel);
     dateRangeLayout->addWidget(startDateEdit, 2);
     dateRangeLayout->addWidget(endDateLabel);
     dateRangeLayout->addWidget(endDateEdit, 2);
     dateRangeLayout->addWidget(updateDateRangeButton, 1);
 
     // Prawa strona - Akcje
     QGroupBox *actionGroupBox = new QGroupBox("Akcje", controlGroupBox);
     QHBoxLayout *actionLayout = new QHBoxLayout(actionGroupBox);
     actionLayout->setSpacing(5);
 
     loadMeasurementsButton = new QPushButton("Załaduj pomiary", actionGroupBox);
     saveMeasurementsButton = new QPushButton("Zapisz do bazy", actionGroupBox);
     loadFromDatabaseButton = new QPushButton("Załaduj z bazy", actionGroupBox);
 
     loadMeasurementsButton->setStyleSheet(buttonStyle);
     saveMeasurementsButton->setStyleSheet(buttonStyle);
     loadFromDatabaseButton->setStyleSheet(buttonStyle);
 
     actionLayout->addWidget(loadMeasurementsButton);
     actionLayout->addWidget(saveMeasurementsButton);
     actionLayout->addWidget(loadFromDatabaseButton);
 
     controlLayout->addWidget(dateRangeGroupBox, 3);
     controlLayout->addWidget(actionGroupBox, 2);
 
     // Dolna część: Wykres, dane pomiarowe i analiza
     QWidget *bottomWidget = new QWidget(centralWidget);
     QHBoxLayout *bottomLayout = new QHBoxLayout(bottomWidget);
     bottomLayout->setSpacing(8);
     bottomLayout->setContentsMargins(0, 0, 0, 0);
 
     // Lewa strona - Wykres
     QGroupBox *chartGroupBox = new QGroupBox("Wykres", bottomWidget);
     chartGroupBox->setStyleSheet("QGroupBox { font-weight: bold; }");
     QVBoxLayout *chartLayout = new QVBoxLayout(chartGroupBox);
 
     chartView = new QChartView(chartGroupBox);
     chartView->setRenderHint(QPainter::Antialiasing);
 
     chartLayout->addWidget(chartView);
 
     // Prawa strona - Dane i analiza
     QWidget *dataAnalysisWidget = new QWidget(bottomWidget);
     QVBoxLayout *dataAnalysisLayout = new QVBoxLayout(dataAnalysisWidget);
     dataAnalysisLayout->setSpacing(8);
     dataAnalysisLayout->setContentsMargins(0, 0, 0, 0);
 
     // Tabela pomiarów
     QGroupBox *tableGroupBox = new QGroupBox("Dane pomiarowe", dataAnalysisWidget);
     tableGroupBox->setStyleSheet("QGroupBox { font-weight: bold; }");
     QVBoxLayout *tableLayout = new QVBoxLayout(tableGroupBox);
 
     measurementTable = new QTableWidget(tableGroupBox);
     measurementTable->setColumnCount(2);
     measurementTable->setHorizontalHeaderLabels({"Data", "Wartość"});
     measurementTable->setSelectionBehavior(QAbstractItemView::SelectRows);
     measurementTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
     measurementTable->setAlternatingRowColors(true);
     measurementTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
     measurementTable->setStyleSheet("QTableView { gridline-color: #d0d0d0; }");
 
     tableLayout->addWidget(measurementTable);
 
     // Panel analizy
     analysisGroupBox = new QGroupBox("Analiza danych", dataAnalysisWidget);
     analysisGroupBox->setStyleSheet("QGroupBox { font-weight: bold; }");
     QFormLayout *analysisLayout = new QFormLayout(analysisGroupBox);
     analysisLayout->setLabelAlignment(Qt::AlignRight);
     analysisLayout->setFormAlignment(Qt::AlignLeft);
     analysisLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
 
     minValueLabel = new QLabel("N/A", analysisGroupBox);
     maxValueLabel = new QLabel("N/A", analysisGroupBox);
     avgValueLabel = new QLabel("N/A", analysisGroupBox);
     trendLabel = new QLabel("N/A", analysisGroupBox);
     minDateLabel = new QLabel("N/A", analysisGroupBox);
     maxDateLabel = new QLabel("N/A", analysisGroupBox);
 
     // Ustaw nowoczesną czcionkę dla wartości analizy
     QFont valueFont("Segoe UI", 10, QFont::Bold);
     minValueLabel->setFont(valueFont);
     maxValueLabel->setFont(valueFont);
     avgValueLabel->setFont(valueFont);
     trendLabel->setFont(valueFont);
     minDateLabel->setFont(valueFont);
     maxDateLabel->setFont(valueFont);
 
     // Dodaj kolorowe wskaźniki
     minValueLabel->setStyleSheet("color: #2c5aa0;");
     maxValueLabel->setStyleSheet("color: #c03030;");
     avgValueLabel->setStyleSheet("color: #107040;");
     trendLabel->setStyleSheet("color: #6030a0;");
 
     analysisLayout->addRow("Wartość minimalna:", minValueLabel);
     analysisLayout->addRow("Data wartości minimalnej:", minDateLabel);
     analysisLayout->addRow("Wartość maksymalna:", maxValueLabel);
     analysisLayout->addRow("Data wartości maksymalnej:", maxDateLabel);
     analysisLayout->addRow("Wartość średnia:", avgValueLabel);
     analysisLayout->addRow("Trend:", trendLabel);
 
     dataAnalysisLayout->addWidget(tableGroupBox, 2);
     dataAnalysisLayout->addWidget(analysisGroupBox, 1);
 
     // Dodaj do głównego układu dolnego
     bottomLayout->addWidget(chartGroupBox, 3);
     bottomLayout->addWidget(dataAnalysisWidget, 2);
 
     // Dodaj wszystkie części do głównego układu
     mainLayout->addWidget(topWidget, 1);     // Stacje i sensory
     mainLayout->addWidget(controlGroupBox, 0); // Kontrolki (mniejsza waga)
     mainLayout->addWidget(bottomWidget, 3);   // Wykres i analiza (większa waga)
 
     // Konfiguracja paska stanu
     setStatusBar(new QStatusBar(this));
     statusBar()->setStyleSheet("QStatusBar { background-color: #f0f0f0; border-top: 1px solid #d0d0d0; }");
 
     // Pasek postępu
     progressBar = new QProgressBar(this);
     progressBar->setRange(0, 100);
     progressBar->setValue(0);
     progressBar->setVisible(false);
     progressBar->setStyleSheet("QProgressBar { border: 1px solid #bbb; border-radius: 3px; text-align: center; } "
                                "QProgressBar::chunk { background-color: #4a9eff; width: 20px; }");
     statusBar()->addPermanentWidget(progressBar);
 
     // Dodaj etykietę trybu w pasku statusu
     modeLabel = new QLabel("Online Mode", this);
     modeLabel->setStyleSheet("QLabel { color: green; }");
     statusBar()->addPermanentWidget(modeLabel);
 
     // Ustaw właściwości okna
     setWindowTitle("Monitor Jakości Powietrza");
     resize(1200, 800);
 }
 
 void MainWindow::checkNetworkStatus()
 {
     // Prosta metoda sprawdzania dostępności sieci
     QTcpSocket socket;
     socket.connectToHost("api.gios.gov.pl", 80);
     bool isAvailable = socket.waitForConnected(3000); // 3 sekundy timeout
     socket.abort();
 
     offlineMode = !isAvailable;
 
     if (offlineMode) {
         modeLabel->setText("Offline Mode");
         modeLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
         showStatusMessage("No network connection. Operating in offline mode.");
     } else {
         modeLabel->setText("Online Mode");
         modeLabel->setStyleSheet("QLabel { color: green; }");
     }
 }
 
 void MainWindow::updateStationsUI()
 {
     // Clear combo box
     stationComboBox->clear();
 
     // Add stations to combo box
     for (const Station &station : stations) {
         stationComboBox->addItem(station.getName() + " (" + station.getCity() + ")", station.getId());
     }
 
     // Update station table
     stationTable->setRowCount(stations.size());
     for (int i = 0; i < stations.size(); ++i) {
         const Station &station = stations[i];
 
         stationTable->setItem(i, 0, new QTableWidgetItem(QString::number(station.getId())));
         stationTable->setItem(i, 1, new QTableWidgetItem(station.getName()));
         stationTable->setItem(i, 2, new QTableWidgetItem(station.getCity()));
         stationTable->setItem(i, 3, new QTableWidgetItem(station.getAddress()));
     }
 }
 
 void MainWindow::updateSensorsUI()
 {
     // Clear combo box
     sensorComboBox->clear();
 
     // Add sensors to combo box
     for (const Sensor &sensor : sensors) {
         sensorComboBox->addItem(sensor.getParamName() + " (" + sensor.getParamFormula() + ")", sensor.getId());
     }
 
     // Update sensor table
     sensorTable->setRowCount(sensors.size());
     for (int i = 0; i < sensors.size(); ++i) {
         const Sensor &sensor = sensors[i];
 
         sensorTable->setItem(i, 0, new QTableWidgetItem(QString::number(sensor.getId())));
         sensorTable->setItem(i, 1, new QTableWidgetItem(sensor.getParamName()));
         sensorTable->setItem(i, 2, new QTableWidgetItem(sensor.getParamFormula()));
         sensorTable->setItem(i, 3, new QTableWidgetItem(sensor.getParamCode()));
     }
 }
 
 void MainWindow::updateMeasurementsUI()
 {
     // Update the chart and analysis
     updateChart();
     analyzeMeasurements();
 
     // Update measurement table
     measurementTable->setRowCount(measurements.size());
     for (int i = 0; i < measurements.size(); ++i) {
         const Measurement &measurement = measurements[i];
 
         measurementTable->setItem(i, 0, new QTableWidgetItem(measurement.getDate().toString("yyyy-MM-dd hh:mm")));
 
         if (measurement.isValid()) {
             measurementTable->setItem(i, 1, new QTableWidgetItem(QString::number(measurement.getValue())));
         } else {
             measurementTable->setItem(i, 1, new QTableWidgetItem("N/A"));
         }
     }
 }
 
 void MainWindow::createChart()
 {
     chart = new QChart();
     chart->setTitle("Pomiary jakości powietrza");
 
     series = new QLineSeries();
     series->setName("Wartość");
 
     chart->addSeries(series);
 
     // Set up the axes
     QDateTimeAxis *axisX = new QDateTimeAxis();
     axisX->setFormat("yyyy-MM-dd hh:mm");
     axisX->setTitleText("Data i czas");
 
     QValueAxis *axisY = new QValueAxis();
     axisY->setTitleText("Wartość");
 
     chart->addAxis(axisX, Qt::AlignBottom);
     chart->addAxis(axisY, Qt::AlignLeft);
     series->attachAxis(axisX);
     series->attachAxis(axisY);
 
     chartView->setChart(chart);
 }
 
 void MainWindow::showStatusMessage(const QString &message)
 {
     statusBar()->showMessage(message);
 }
 