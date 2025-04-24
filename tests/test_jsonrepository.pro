QT += testlib core gui network
CONFIG += console testcase
TEMPLATE = app

TARGET = test_jsonrepository

INCLUDEPATH += ..

SOURCES += \
    test_jsonrepository.cpp \
    ../src/database/jsonrepository.cpp \
    ../src/data/station.cpp \
    ../src/data/sensor.cpp \
    ../src/data/measurement.cpp

HEADERS += \
    ../src/database/jsonrepository.h \
    ../src/data/station.h \
    ../src/data/sensor.h \
    ../src/data/measurement.h