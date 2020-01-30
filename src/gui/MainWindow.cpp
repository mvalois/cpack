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

#include <QFileDialog>
#include <QTextStream>
#include <iostream>
#include <QtCharts>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Utils.h"

using namespace std;

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

void MainWindow::setFilename(const string& filename){
    ui->fileLine->setText(QString::fromStdString(filename));
}

void MainWindow::setThreads(const int& nb_threads){
    ui->threadNumberSpin->setValue(nb_threads);
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
    stats = Statsgen((ui->fileLine->text()).toUtf8().constData());

    if(ui->withcountButton->isChecked())
    {
        stats.setWithcount(true);
    }

    try
    {
        stats.setNbThread(stoi(ui->threadNumberSpin->text().toStdString()));
    }
    catch(invalid_argument)
    {
        stats.setNbThread(1);
    }
    catch(out_of_range)
    {
        stats.setNbThread(1);
    }

    if(ui->regexCheckBox->isChecked())
    {
        stats.setRegex(ui->RegexlineEdit->text().toStdString());
    }

    if(ui->maxSimpleCheckBox->isChecked())
    {
        try
        {
            stats.setLimitSimplemask(stoi(ui->MaxSimpleMaxSpin->text().toStdString()));
        }
        catch(invalid_argument)
        {
            stats.setLimitSimplemask(100);
        }
        catch(out_of_range)
        {
            stats.setLimitSimplemask(100);
        }

    }

    if(ui->maxAdvancedCheckBox->isChecked())
    {
        try
        {
            stats.setLimitAdvancedmask(stoi(ui->MaxAdvMaskSpin->text().toStdString()));
        }
        catch(invalid_argument)
        {
            stats.setLimitAdvancedmask(100);
        }
        catch(out_of_range)
        {
            stats.setLimitAdvancedmask(100);
        }

    }

    if(ui->topCheckBox->isChecked())
    {
        try
        {
            stats.setTop(stoi(ui->TopXAnswerSpin->text().toStdString()));
        }
        catch(invalid_argument)
        {
            stats.setTop(10);
        }
        catch(out_of_range)
        {
            stats.setTop(10);
        }

    }

    try
    {
        int length=stoi(ui->MinLenSpin->text().toStdString());
        int special=stoi(ui->MinOccSpecialSpin->text().toStdString());
        int digit=stoi(ui->MinOccDigitsSpin->text().toStdString());
        int upper=stoi(ui->MinOccUpperSpin->text().toStdString());
        int lower=stoi(ui->MinOccLowerSpin->text().toStdString());

        stats.setSecurityRules(length,special,digit,upper,lower);
    }
    catch(invalid_argument)
    {
        stats.setSecurityRules(8,0,1,1,1);
    }
    catch(out_of_range)
    {
        stats.setSecurityRules(8,0,1,1,1);
    }

    ui->fileLine->setDisabled(true);
    ui->Browser->setDisabled(true);
    ui->threadNumberSpin->setDisabled(true);
    ui->TopXAnswerSpin->setDisabled(true);
    ui->MaxSimpleMaxSpin->setDisabled(true);
    ui->MaxAdvMaskSpin->setDisabled(true);
    ui->RegexlineEdit->setDisabled(true);
    ui->MinLenSpin->setDisabled(true);
    ui->MinOccLowerSpin->setDisabled(true);
    ui->MinOccUpperSpin->setDisabled(true);
    ui->MinOccSpecialSpin->setDisabled(true);
    ui->MinOccDigitsSpin->setDisabled(true);
    ui->startButton->setDisabled(true);
    ui->topCheckBox->setDisabled(true);
    ui->regexCheckBox->setDisabled(true);
    ui->maxAdvancedCheckBox->setDisabled(true);
    ui->maxSimpleCheckBox->setDisabled(true);
    ui->progressBar->setValue(0);

    delete layoutLength;
    delete layoutCharset;

    WorkerThread *workerThread = new WorkerThread(stats);
    connect(workerThread, SIGNAL(resultReady()), this, SLOT(handleResults()));
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    workerThread->start();
    ui->progressBar->setEnabled(true);

    delete progressThread;
    progressThread = new ProgressThread(stats, *ui->progressBar);
    progressThread->start();
}



void MainWindow::enableWithCount()
{
    ui->classicButton->setChecked(false);
}

void MainWindow::disableWithCount()
{
    ui->withcountButton->setChecked(false);
}

double MainWindow::initGraphicalStats(QBarSeries * barLength, QPieSeries * pieCharset, double & percentageTotal, double & percentageSecurity) {
    double total = stats.getTotalCounter();
    double filter = stats.getTotalFilter();
    percentageTotal = percentage(filter, total);
    percentageSecurity = percentage(stats.getNbSecurePasswords(), total);

    /* LENGTH HISTOGRAM */
    multimap<uint64_t, int> reverseL = flip_map<int>(stats.getStatsLength());
    double percentageL;
    double maxPercLength = 0;
    uint64_t nbHideL = 0;

    MapIterator<uint64_t, int> itL;
    for(itL = reverseL.end(); itL != reverseL.begin(); itL--) {
        if (itL == reverseL.end()) continue;

        percentageL = percentage(itL->first,  total);
        maxPercLength = percentageL > maxPercLength ? percentageL : maxPercLength;
        if (percentageL >= 2) {
            QBarSet *set = new QBarSet(QString::number(itL->second));
            *set << percentageL;
            barLength->append(set);
        } else {
            nbHideL += itL->first;
        }
    }

    QBarSet *set = new QBarSet("Other lengths");
    *set << percentage(nbHideL, total);
    barLength->append(set);


    /* CHARSET PIECHART */
    multimap<uint64_t, string> reverseC = flip_map<string>(stats.getStatsCharsets());
    int top = 0;
    uint64_t nbHideC = 0;

    MapIterator<uint64_t, string> itC;
    for(itC = reverseC.end(); itC != reverseC.begin(); itC--) {
        if (itC == reverseC.end()) continue;
        top++;
        if (top <= 5) {
            pieCharset->append(QString::fromStdString(itC->second), itC->first);
        } else {
            nbHideC += itC->first;
        }
    }
    pieCharset->append("Other charsets", nbHideC);
    return maxPercLength;
}

void MainWindow::handleResults()
{

    progressThread->terminate();
    progressThread->wait();
    ui->progressBar->setValue(100);
    ui->fileLine->setDisabled(false);
    ui->Browser->setDisabled(false);
    ui->charsetWidget->setHidden(false);
    ui->lengthWidget->show();
    ui->threadNumberSpin->setDisabled(false);
    ui->TopXAnswerSpin->setDisabled(false);
    ui->MaxSimpleMaxSpin->setDisabled(false);
    ui->MaxAdvMaskSpin->setDisabled(false);
    ui->RegexlineEdit->setDisabled(false);
    ui->MinLenSpin->setDisabled(false);
    ui->MinOccLowerSpin->setDisabled(false);
    ui->MinOccUpperSpin->setDisabled(false);
    ui->MinOccSpecialSpin->setDisabled(false);
    ui->MinOccDigitsSpin->setDisabled(false);
    ui->startButton->setDisabled(false);
    ui->topCheckBox->setDisabled(false);
    ui->regexCheckBox->setDisabled(false);
    ui->maxAdvancedCheckBox->setDisabled(false);
    ui->maxSimpleCheckBox->setDisabled(false);


    QBarSeries * barLength = new QBarSeries();
    QPieSeries * pieCharset = new QPieSeries();
    double percentageTotal, percentageSecurity, maxPercLength;

    maxPercLength = initGraphicalStats(barLength, pieCharset, percentageTotal, percentageSecurity);

    /* HISTOGRAM FOR LENGTH */
    QChart *chartL = new QChart();
    chartL->addSeries(barLength);
    chartL->setTitle("Length");

    QChartView *chartViewL = new QChartView(chartL);
    chartViewL->setRenderHint(QPainter::Antialiasing);

    layoutLength = new QVBoxLayout();
    layoutLength->addWidget(chartViewL);
    ui->lengthWidget->setLayout(layoutLength);

    QValueAxis *axisY = new QValueAxis();
    chartL->addAxis(axisY, Qt::AlignLeft);
    barLength->attachAxis(axisY);
    axisY->setRange(0, (int) (maxPercLength + (5 - (int) maxPercLength % 5)));
    axisY->setTickCount((int) (maxPercLength / 5) + 2);


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
                               + QString::number(stats.getTotalFilter(), 'g', 2)
                               + " on a total of "
                               + QString::number(stats.getTotalFilter(), 'g', 2)
                               + " passwords (" + QString::number(percentageTotal) + "%)");


    ui->securityLabel->setText("--> " + QString::number(percentageSecurity)
                               + "% of the passwords respects the security rules");

    stats.print_stats();

    ui->resultLabel->setText("For more detailed statistics, check the file \"result.txt\"");
}