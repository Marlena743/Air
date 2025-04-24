QT += testlib core gui charts
CONFIG += console testcase
TEMPLATE = app

TARGET = test_dataanalyzer

INCLUDEPATH += ..

SOURCES += \
    test_dataanalyzer.cpp \
    ../src/analysis/dataanalyzer.cpp \
    ../src/data/measurement.cpp

HEADERS += \
    ../src/analysis/dataanalyzer.h \
    ../src/data/measurement.h