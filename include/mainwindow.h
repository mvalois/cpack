/**
 * @file mainwindow.h
 *
 * @author Jean-Baptiste Jorand
 * @author Yannick Bass
 * Copyright (c) 2018 Jorand Jean-baptiste Bass Yannick
 * All rights reserved.
 *
 * Please see the attached LICENSE file for additional licensing information.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "statsgen.h"

#include <QMainWindow>
#include <QThread>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QtCharts>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



public slots:
    void findFile();
    void startGame();
    void handleResults();
    void enableWithCount();
    void disableWithCount();



private:
    Ui::MainWindow *ui;
    Statsgen * stats;
    QMessageBox waitBox;
    QVBoxLayout * layoutCharset;
    QVBoxLayout * layoutLength;
    int firstTime=1;
    void initGraphicalStats(QBarSeries * barLength, QPieSeries * pieCharset, double & percentageTotal, double & percentageSecurity);
};


class WorkerThread : public QThread
{

    Q_OBJECT
    void run() {

        _s->generate_stats();
        emit resultReady();
    }
public:
    WorkerThread(Statsgen *s)
    {
        _s=s;
    }
signals:
    void resultReady();
private:
    Statsgen *_s;
};

#endif // MAINWINDOW_H
