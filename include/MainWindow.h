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

#include "Statsgen.h"
#include "Utils.h"

#include <QMainWindow>
#include <QThread>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QtCharts>


class ProgressThread : public QThread
{
    Q_OBJECT
    void run() {
        unsigned int progress = 0;
        uint64_t processed = 0;
        int finished = 0;
        const int nbthreads = _s.getNbThreads();
        const struct thread_data* td = _s.getThreadsData();

        while(td[nbthreads-1].lineEnd == 0){
            msleep(500);
        }
        const uint64_t nblines = td[nbthreads-1].lineEnd;

        while(finished != nbthreads){
            processed = 0;
            finished = 0;
            for(int i=0; i < nbthreads; ++i){
                processed += td[i].total_counter;
                finished += td[i].finished;
            }
            progress = (int) percentage(processed, nblines);
            _qpb.setValue(progress);
            msleep(500);
        }
        _qpb.setValue(100);
    }
public:
    ProgressThread(Statsgen &s, QProgressBar& qpb):_s(s), _qpb(qpb){}
private:
    Statsgen &_s;
    QProgressBar &_qpb;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setFilename(const std::string& filename);
    void setThreads(const int& nb_threads);


public slots:
    void findFile();
    void startGame();
    void handleResults();
    void enableWithCount();
    void disableWithCount();



private:
    Ui::MainWindow *ui;
    Statsgen stats;
    QMessageBox waitBox;
    QVBoxLayout * layoutCharset = nullptr;
    QVBoxLayout * layoutLength = nullptr;
    QVBoxLayout * layoutSimple = nullptr;
    QVBoxLayout * layoutAdvanced = nullptr;
    ProgressThread* progressThread = nullptr;
    int firstTime=1;
    double initGraphicalStats(QBarSeries * barLength, QPieSeries * pieCharset, QPieSeries* pieSimple, QPieSeries* pieAdvanced, double & percentageTotal, double & percentageSecurity);
    QVBoxLayout*  drawPieChart(QPieSeries* qps, QVBoxLayout* layout, const std::string& title);
};


class WorkerThread : public QThread
{

    Q_OBJECT
    void run() {

        _s.generate_stats();
        emit resultReady();
    }
public:
    WorkerThread(Statsgen &s):_s(s){}
signals:
    void resultReady();
private:
    Statsgen &_s;
};

#endif // MAINWINDOW_H
