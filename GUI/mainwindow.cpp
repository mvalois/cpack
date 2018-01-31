#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QTextStream>
#include <iostream>





MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->Browser, SIGNAL(clicked(bool)), this, SLOT(findFile()));
    connect(ui->startButton, SIGNAL(clicked(bool)), this, SLOT(startGame()));
    connect(ui->withcountButton,SIGNAL(clicked()),this,SLOT(enableWithCount()));
    connect(ui->classicButton,SIGNAL(clicked()),this,SLOT(disableWithCount()));
    connect(ui->hiderareCheckBox,SIGNAL(clicked(bool)),this,SLOT(hiderare(bool)));
}


MainWindow::~MainWindow()
{
    delete ui;
}


/***************************************************/
/*                      SLOTS                      */
/***************************************************/

void MainWindow::findFile() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Browser"), QDir::currentPath(), tr("Text files (*.txt)"));
    ui->fileLine->setText(filename);
}


void MainWindow::startGame() {
    stats = new Statsgen();
    stats->setFilename(ui->fileLine->text().toStdString());

    if(ui->withcountButton->isChecked())
        stats->setWithcount(true);

    stats->setTop(10);
    stats->setNbThread(4);

    WorkerThread *workerThread = new WorkerThread(stats);
    connect(workerThread, SIGNAL(resultReady()), this, SLOT(handleResults()));
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    workerThread->start();
    waitBox.setText("Analysis in progress");

    waitBox.exec();
/*
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    int c = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        c++;
    }
    ui->teResult->setText(QString::number(c));
    */

}

void MainWindow::enableWithCount()
{
    ui->classicButton->setChecked(false);
}

void MainWindow::disableWithCount()
{
    ui->withcountButton->setChecked(false);
}

void MainWindow::handleResults()
{
    waitBox.close();
    stats->print_stats();
}

void MainWindow::hiderare(bool checked)
{
    if(checked)
        stats->setHiderare(1);
    else
        stats->setHiderare(0);
}


