
QT += core gui
QT += widgets
QT += charts

TARGET  = GUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        statsgen.cpp

HEADERS  += mainwindow.h\
        statsgen.h\
        utils.h

FORMS    += mainwindow.ui


QMAKE_CXXFLAGS += -std=c++11 -lpthread
