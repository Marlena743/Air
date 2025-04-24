QT += core gui network charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AirQualityMonitor
TEMPLATE = app

# Rozróżnienie między trybem testowym a zwykłym
CONFIG(test) {
    # Pliki źródłowe dla trybu testowego
    SOURCES += \
        tests/test_dataanalyzer.cpp \
        tests/test_jsonrepository.cpp
} else {
    # Główny plik main.cpp tylko w zwykłym trybie
    SOURCES += \
        main.cpp
}

# Wspólne pliki źródłowe dla wszystkich trybów
SOURCES += \
    mainwindow.cpp \
    src/api/restapi.cpp \
    src/api/networkmanager.cpp \
    src/data/station.cpp \
    src/data/sensor.cpp \
    src/data/measurement.cpp \
    src/database/jsonrepository.cpp \
    src/analysis/dataanalyzer.cpp

HEADERS += \
    mainwindow.h \
    src/api/restapi.h \
    src/api/networkmanager.h \
    src/data/station.h \
    src/data/sensor.h \
    src/data/measurement.h \
    src/database/jsonrepository.h \
    src/analysis/dataanalyzer.h

FORMS += \
    # Tu można dodać pliki .ui jeśli używasz designera Qt

# Dodaj katalogi do ścieżki wyszukiwania
INCLUDEPATH += \
    src
