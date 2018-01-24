#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "statsgen.h"

#include <QMainWindow>

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



private:
    Ui::MainWindow *ui;
    QString filename;
    Statsgen stats;
};

#endif // MAINWINDOW_H
