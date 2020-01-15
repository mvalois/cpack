QT += core gui
QT += widgets
QT += charts

TARGET  = cppack-gui
TEMPLATE = app

INCLUDEPATH += include
OBJECTS_DIR=obj/gui/
MOC_DIR=obj/gui/


SOURCES += src/gui/maingui.cpp\
        src/gui/mainwindow.cpp\
        src/core/statsgen.cpp

HEADERS  += include/mainwindow.h\
        include/statsgen.h\
        include/utils.h

FORMS    += forms/mainwindow.ui


QMAKE_CXXFLAGS += -std=c++11 -lpthread
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3