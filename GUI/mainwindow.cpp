#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QTextStream>
#include <iostream>


#include <QtCharts>

using namespace QtCharts;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->Browser, SIGNAL(clicked(bool)), this, SLOT(findFile()));
    connect(ui->startButton, SIGNAL(clicked(bool)), this, SLOT(startGame()));
}


MainWindow::~MainWindow()
{
    delete ui;
}


/***************************************************/
/*                      SLOTS                      */
/***************************************************/

void MainWindow::findFile() {
    filename = QFileDialog::getOpenFileName(this, tr("Browser"), QDir::currentPath(), tr("Text files (*.txt)"));
    ui->fileLine->setText(filename);
}


void MainWindow::startGame() {
    stats = new Statsgen();

    stats->setTop(10);
    stats->generate_stats();
    stats->print_stats();

    QPieSeries * length = stats->getLengthPiechart(ui->resultFrame);

    QChart *chart = new QChart();
    chart->addSeries(length);
    chart->setTitle("Length pichart");
    //chart->legend()->hide();

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);


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
