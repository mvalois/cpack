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
    QString filename = ui->fileLine->text();
    if(filename.isEmpty()){
        QMessageBox(QMessageBox::Critical, "File empty", "You have not provided a filename !").exec();
        return;
    }
    if(! QFile::exists(filename)){
        QMessageBox(QMessageBox::Critical, "No such file", "The file " + filename + " does not exist !").exec();
        return;
    }
    stats = Statsgen(filename.toUtf8().constData());

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

    int other_perc = stoi(ui->OtherThreshold->text().toStdString());
    perc_other = other_perc ? other_perc : 2;

    int charset_nb = stoi(ui->CharsetToDisplay->text().toStdString());
    display_charsets = charset_nb ? charset_nb : 5;

    int max_simple = stoi(ui->MaxSimpleMaskSpin->text().toStdString());
    display_simples = max_simple ? max_simple : 5;

    int max_advanced = stoi(ui->MaxAdvMaskSpin->text().toStdString());
    display_advanced = max_advanced ? max_advanced : 10;

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

    ui->settingsWidget->setEnabled(false);
    ui->progressBar->setValue(0);
    ui->progressBar->setEnabled(true);

    delete layoutLength;
    delete layoutCharset;
    delete layoutSimple;
    delete layoutAdvanced;

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

double MainWindow::initGraphicalStats(QBarSeries * barLength, QPieSeries * pieCharset, QPieSeries* pieSimple, QPieSeries* pieAdvanced, double & percentageTotal, double & percentageSecurity) {
    const ThreadData& results = stats.getResults();
    double total = results.total_counter;
    double filter = results.total_filter;
    percentageTotal = percentage(filter, total);
    percentageSecurity = percentage(results.sr.nbSecurePassword, total);

    /* LENGTH HISTOGRAM */
    map<uint64_t, int, greater<uint64_t>> reverseL = flip_map<int>(results.length);
    double percentageL;
    double maxPercLength = 0;
    uint64_t nbHideL = 0;
    barLength->clear();

    for(pair<uint64_t, int> itL : reverseL) {

        percentageL = percentage(itL.first,  total);
        maxPercLength = percentageL > maxPercLength ? percentageL : maxPercLength;
        if (percentageL >= perc_other) {
            QBarSet *set = new QBarSet(QString::number(itL.second));
            *set << percentageL;
            barLength->append(set);
        } else {
            nbHideL += itL.first;
        }
    }

    QBarSet *set = new QBarSet("Other lengths");
    *set << percentage(nbHideL, total);
    barLength->append(set);


    /* CHARSET PIECHART */
    map<uint64_t, string, greater<uint64_t>> reverseC = flip_map(results.charactersets);
    uint64_t top = 0;
    uint64_t nbHideC = 0;
    pieCharset->clear();

    for(pair<uint64_t, string> itC : reverseC) {
        top++;
        if (top <= display_charsets) {
            pieCharset->append(QString::fromStdString(itC.second), itC.first);
        } else {
            nbHideC += itC.first;
        }
    }
    pieCharset->append("Other charsets", nbHideC);

    /* SIMPLE PIECHART */
    map<uint64_t, string, greater<uint64_t>> reverseS = flip_map(results.simplemasks);
    uint64_t top_simple = 0;
    uint64_t nbHideS = 0;
    pieSimple->clear();

    for(pair<uint64_t, string> itS : reverseS) {
        top_simple++;
        if (top_simple <= display_simples) {
            pieSimple->append(QString::fromStdString(itS.second), itS.first);
        } else {
            nbHideS += itS.first;
        }
    }
    pieSimple->append("Other Masks", nbHideS);

    /* ADVANCED PIECHART */
    map<uint64_t, string, greater<uint64_t>> reverseA = flip_map(results.advancedmasks);
    uint64_t top_advanced = 0;
    uint64_t nbHideA = 0;
    pieAdvanced->clear();

    MapIterator<uint64_t, string> itA;
    for(pair<uint64_t, string> itA : reverseA) {
        top_advanced++;
        if (top_advanced <= display_advanced) {
            pieAdvanced->append(QString::fromStdString(itA.second), itA.first);
        } else {
            nbHideA += itA.first;
        }
    }
    pieAdvanced->append("Other Masks", nbHideA);
    return maxPercLength;
}

QVBoxLayout* MainWindow::drawPieChart(QPieSeries* qps, QVBoxLayout* layout, const string& title){
    qps->setLabelsVisible();

    QPieSlice* slice = qps->slices().at(0);
    slice->setExploded();
    slice->setLabelVisible();
    slice->setPen(QPen(Qt::darkGreen, 2));
    slice->setBrush(Qt::green);

    QChart* chart = new QChart();
    chart->addSeries(qps);
    chart->setTitle(QString::fromStdString(title));
    chart->legend()->hide();

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    layout = new QVBoxLayout();
    layout->addWidget(chartView);
    return layout;
}

void MainWindow::handleResults()
{
    const ThreadData& results = stats.getResults();
    progressThread->terminate();
    progressThread->wait();
    ui->progressBar->setValue(100);
    ui->settingsWidget->setEnabled(true);

    QBarSeries * barLength = new QBarSeries();
    QPieSeries * pieCharset = new QPieSeries();
    QPieSeries * pieSimple = new QPieSeries();
    QPieSeries * pieAdvanced = new QPieSeries();
    double percentageTotal, percentageSecurity, maxPercLength;

    maxPercLength = initGraphicalStats(barLength, pieCharset, pieSimple, pieAdvanced, percentageTotal, percentageSecurity);

    /* HISTOGRAM FOR LENGTH */
    QChart *chartL = new QChart();
    chartL->addSeries(barLength);
    chartL->setTitle("Length");

    QChartView *chartViewL = new QChartView(chartL);
    chartViewL->setRenderHint(QPainter::Antialiasing);

    layoutLength = new QVBoxLayout();
    layoutLength->addWidget(chartViewL);
    if(ui->lengthWidget->layout() == nullptr)
        ui->lengthWidget->setLayout(layoutLength);

    QValueAxis *axisY = new QValueAxis();
    chartL->addAxis(axisY, Qt::AlignLeft);
    barLength->attachAxis(axisY);
    axisY->setRange(0, (int) (maxPercLength + (5 - (int) maxPercLength % 5)));
    axisY->setTickCount((int) (maxPercLength / 5) + 2);


    /* PIECHART FOR CHARSET */
    layoutCharset = drawPieChart(pieCharset, layoutCharset, "Charset");
    if(ui->charsetWidget->layout() == nullptr)
        ui->charsetWidget->setLayout(layoutCharset);

    /* PIECHART FOR SIMPLE */
    layoutSimple = drawPieChart(pieSimple, layoutSimple, "Simple masks");
    if(ui->simpleMasksWidget->layout() == nullptr)
        ui->simpleMasksWidget->setLayout(layoutSimple);

    /* PIECHART FOR ADVANCED */
    layoutAdvanced = drawPieChart(pieAdvanced, layoutAdvanced, "Advanced masks");
    if(ui->advancedMasksWidget->layout() == nullptr)
        ui->advancedMasksWidget->setLayout(layoutAdvanced);

    /* LABELS */

    ui->AnalyzedLabel->setText("Number of analyzed passwords: "
                               + QString::number(results.total_filter, 'g', 2)
                               + " on a total of "
                               + QString::number(results.total_counter, 'g', 2)
                               + " passwords (" + QString::number(percentageTotal) + "%)");


    ui->securityLabel->setText("--> " + QString::number(percentageSecurity)
                               + "% of the passwords respects the security rules");

    // stats.print_stats();
}