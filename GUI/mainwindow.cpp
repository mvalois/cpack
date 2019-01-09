/**
 * @file mainwindow.cpp
 *
 * @author Jean-Baptiste Jorand
 * @author Yannick Bass
 * Copyright (c) 2018 Jorand Jean-baptiste Bass Yannick
 * All rights reserved.
 *
 * Please see the attached LICENSE file for additional licensing information.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QTextStream>
#include <iostream>
#include <QtCharts>





MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->Browser, SIGNAL(clicked(bool)), this, SLOT(findFile()));
    connect(ui->startButton, SIGNAL(clicked(bool)), this, SLOT(startGame()));
    connect(ui->withcountButton,SIGNAL(clicked()),this,SLOT(enableWithCount()));
    connect(ui->classicButton,SIGNAL(clicked()),this,SLOT(disableWithCount()));
}


MainWindow::~MainWindow()
{
    delete ui;
}


/***************************************************/
/*                      SLOTS                      */
/***************************************************/

void MainWindow::findFile() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Browser"), QDir::currentPath(), tr("Text files (*.txt);; All Files (*)"));
    ui->fileLine->setText(filename);
}


void MainWindow::startGame() {
    stats = new Statsgen();
    stats->setFilename(ui->fileLine->text());

    if(ui->withcountButton->isChecked())
    {
        stats->setWithcount(true);
    }

    if(ui->threadsCheckBox->isChecked())
    {
        try
        {
            stats->setNbThread(std::stoi(ui->ThreadlineEdit->text().toStdString()));
        }
        catch(std::invalid_argument)
        {
            stats->setNbThread(1);
        }
        catch(std::out_of_range)
        {
            stats->setNbThread(1);
        }

    }

    if(ui->regexCheckBox->isChecked())
    {
        stats->setRegex(ui->RegexlineEdit->text().toStdString());
    }

    if(ui->maxSimpleCheckBox->isChecked())
    {
        try
        {
            stats->setLimitSimplemask(std::stoi(ui->SimplelineEdit->text().toStdString()));
        }
        catch(std::invalid_argument)
        {
            stats->setLimitSimplemask(100);
        }
        catch(std::out_of_range)
        {
            stats->setLimitSimplemask(100);
        }

    }

    if(ui->maxAdvancedCheckBox->isChecked())
    {
        try
        {
            stats->setLimitAdvancedmask(std::stoi(ui->AdvancedlineEdit->text().toStdString()));
        }
        catch(std::invalid_argument)
        {
            stats->setLimitAdvancedmask(100);
        }
        catch(std::out_of_range)
        {
            stats->setLimitAdvancedmask(100);
        }

    }

    if(ui->topCheckBox->isChecked())
    {
        try
        {
            stats->setTop(std::stoi(ui->topLineEdit->text().toStdString()));
        }
        catch(std::invalid_argument)
        {
            stats->setTop(10);
        }
        catch(std::out_of_range)
        {
            stats->setTop(10);
        }

    }

    try
    {
        int length=std::stoi(ui->lengthLineEdit->text().toStdString());
        int special=std::stoi(ui->specialLineEdit->text().toStdString());
        int digit=std::stoi(ui->digitLineEdit->text().toStdString());
        int upper=std::stoi(ui->upperLineEdit->text().toStdString());
        int lower=std::stoi(ui->lowerLineEdit->text().toStdString());

        stats->setSecurityRules(length,special,digit,upper,lower);
    }
    catch(std::invalid_argument)
    {
        stats->setSecurityRules(8,0,1,1,1);
    }
    catch(std::out_of_range)
    {
        stats->setSecurityRules(8,0,1,1,1);
    }

    ui->ThreadlineEdit->setDisabled(true);
    ui->topLineEdit->setDisabled(true);
    ui->SimplelineEdit->setDisabled(true);
    ui->AdvancedlineEdit->setDisabled(true);
    ui->RegexlineEdit->setDisabled(true);
    ui->lengthLineEdit->setDisabled(true);
    ui->lowerLineEdit->setDisabled(true);
    ui->upperLineEdit->setDisabled(true);
    ui->specialLineEdit->setDisabled(true);
    ui->digitLineEdit->setDisabled(true);
    ui->startButton->setDisabled(true);
    ui->threadsCheckBox->setDisabled(true);
    ui->topCheckBox->setDisabled(true);
    ui->regexCheckBox->setDisabled(true);
    ui->maxAdvancedCheckBox->setDisabled(true);
    ui->maxSimpleCheckBox->setDisabled(true);


    //delete layoutCharset;
    delete layoutLength;

    WorkerThread *workerThread = new WorkerThread(stats);
    connect(workerThread, SIGNAL(resultReady()), this, SLOT(handleResults()));
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    workerThread->start();
    waitBox.setText("Analysis in progress");
    waitBox.exec();
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
    ui->charsetWidget->setHidden(false);
    ui->lengthWidget->show();
    ui->ThreadlineEdit->setDisabled(false);
    ui->topLineEdit->setDisabled(false);
    ui->SimplelineEdit->setDisabled(false);
    ui->AdvancedlineEdit->setDisabled(false);
    ui->RegexlineEdit->setDisabled(false);
    ui->lengthLineEdit->setDisabled(false);
    ui->lowerLineEdit->setDisabled(false);
    ui->upperLineEdit->setDisabled(false);
    ui->specialLineEdit->setDisabled(false);
    ui->digitLineEdit->setDisabled(false);
    ui->startButton->setDisabled(false);
    ui->threadsCheckBox->setDisabled(false);
    ui->topCheckBox->setDisabled(false);
    ui->regexCheckBox->setDisabled(false);
    ui->maxAdvancedCheckBox->setDisabled(false);
    ui->maxSimpleCheckBox->setDisabled(false);


    QBarSeries * barLength = new QBarSeries();
    QPieSeries * pieCharset = new QPieSeries();
    double percentageTotal, percentageSecurity, total_counter, total_filter;

    stats->initGraphicalStats(barLength, pieCharset, percentageTotal, percentageSecurity, total_counter, total_filter);



    /* HISTOGRAM FOR LENGTH */
    QChart *chartL = new QChart();
    chartL->addSeries(barLength);
    chartL->setTitle("Length");

    QChartView *chartViewL = new QChartView(chartL);
    chartViewL->setRenderHint(QPainter::Antialiasing);

    layoutLength = new QVBoxLayout();
    layoutLength->addWidget(chartViewL);
    ui->lengthWidget->setLayout(layoutLength);


    /* PIECHART FOR CHARSET */
    pieCharset->setLabelsVisible();

    QPieSlice *slice = pieCharset->slices().at(0);
    slice->setExploded();
    slice->setLabelVisible();
    slice->setPen(QPen(Qt::darkGreen, 2));
    slice->setBrush(Qt::green);

    QChart *chartC = new QChart();
    chartC->addSeries(pieCharset);
    chartC->setTitle("Charset");
    chartC->legend()->hide();

    QChartView *chartViewC = new QChartView(chartC);
    chartViewC->setRenderHint(QPainter::Antialiasing);

    layoutCharset = new QVBoxLayout();
    layoutCharset->addWidget(chartViewC);
    ui->charsetWidget->setLayout(layoutCharset);


    /* LABELS */

    ui->AnalyzedLabel->setText("Number of analyzed passwords: "
                               + QString::number(total_filter)
                               + " on a total of "
                               + QString::number(total_counter)
                               + " passwords (" + QString::number(percentageTotal, 'f', 2) + "%)");


    ui->securityLabel->setText("--> " + QString::number(percentageSecurity, 'f', 2)
                               + "% of the passwords respects the security rules");

    stats->print_stats();

    ui->resultLabel->setText("For more detailed statistics, check the file \"result.txt\"");
}


