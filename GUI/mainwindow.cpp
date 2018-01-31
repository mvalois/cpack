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


