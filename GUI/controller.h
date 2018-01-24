#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "statsgen.h"
#include "utils.h"

#include <QObject>
#include <QThread>
#include <unordered_map>

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;

public:
    Controller(std::string filename) {
        Statsgen * stats = new Statsgen();

        stats->setFilename(filename);
        stats->setTop(10);
        stats->setNbThread(3);

        stats->moveToThread(&workerThread);
        connect(&workerThread, SIGNAL(finished()), stats, SLOT(deleteLater()));
        connect(this, SIGNAL(started()), stats, SLOT(generate_stats()));
        connect(stats, SIGNAL(resultReady(std::unordered_map<int,double>)), this, SLOT(handleResults(std::unordered_map<int,double>)));
        workerThread.start();
    }

    ~Controller() {
        workerThread.quit();
        workerThread.wait();
    }

public slots:
    void handleResults(const std::unordered_map<int, double> & stats_length) {
        std::wcout << "\nStatistics relative to length: \n" << std::endl;
        int count = 0;
        showMap(stats_length, 10, 14000000, false, count);
    }
};

#endif // CONTROLLER_H
