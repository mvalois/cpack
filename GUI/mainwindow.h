#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "statsgen.h"

#include <QMainWindow>
#include <QThread>
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



private:
    Ui::MainWindow *ui;
    QString filename;
    Statsgen * stats;
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
