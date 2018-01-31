#-------------------------------------------------
#
# Project created by QtCreator 2018-01-10T12:47:54
#
#-------------------------------------------------

QT      += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT      += charts

TARGET = GUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        statsgen.cpp

HEADERS  += mainwindow.h\
        statsgen.h\
        utils.h

FORMS    += mainwindow.ui


QMAKE_CXXFLAGS += -std=c++11 -lpthread
