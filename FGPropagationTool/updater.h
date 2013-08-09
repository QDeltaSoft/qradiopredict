#ifndef UPDATER_H
#define UPDATER_H

#include "databaseapi.h"
#include "fgtelnet.h"
#include "signal.h"
#include <math.h>
#include <QObject>
#include <QTime>
#include <QDateTime>
#include <QVector>

class Updater : public QObject
{
    Q_OBJECT
public:
    Updater( DatabaseApi *d);
    ~Updater();

public slots:
    void startUpdate();

signals:
    void haveMobilePosition(double lon, double lat);
    void haveSignalReading(double lon, double lat, unsigned id, QString name, double freq, Signal *s);
    void finished();

private:
    FGTelnet *_fg;
    DatabaseApi *_db;

};

#endif // UPDATER_H
