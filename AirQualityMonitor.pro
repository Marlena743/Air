QT += core gui network charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AirQualityMonitor
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    src/api/restapi.cpp \
    src/data/station.cpp \
    src/data/sensor.cpp \
    src/data/measurement.cpp \
    src/database/jsonrepository.cpp \
    src/analysis/dataanalyzer.cpp

HEADERS += \
    mainwindow.h \
    src/api/restapi.h \
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
