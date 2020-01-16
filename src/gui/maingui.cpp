/**
 * @file main.cpp
 *
 * @author Jean-Baptiste Jorand
 * @author Yannick Bass
 * Copyright (c) 2018 Jorand Jean-baptiste Bass Yannick
 * All rights reserved.
 *
 * Please see the attached LICENSE file for additional licensing information.
 */

#include <QApplication>
#include <thread>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    // set file if given on cmd line
    if(argc >= 2){
    	w.setFilename(argv[1]);
    }
    w.setThreads(std::thread::hardware_concurrency());
    return a.exec();
}
