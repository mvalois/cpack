QT += core gui
QT += widgets
QT += charts

TARGET  = cpack-gui
TEMPLATE = app

INCLUDEPATH += include


SOURCES += src/gui/maingui.cpp\
        src/gui/mainwindow.cpp\
        src/core/statsgen.cpp

HEADERS  += include/mainwindow.h\
        include/statsgen.h\
        include/utils.h

FORMS    += forms/mainwindow.ui


QMAKE_CXXFLAGS += -std=c++11 -lpthread