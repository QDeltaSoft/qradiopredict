#ifndef UPDATER_H
#define UPDATER_H

#include "databaseapi.h"
#include "fgtelnet.h"
#include "signal.h"
#include <QObject>
#include <QTime>
#include <QDateTime>
#include <QVector>

class Updater : public QObject
{
    Q_OBJECT
public:
    Updater(FGTelnet *t, DatabaseApi *db);

public slots:
    void startUpdate();

signals:
    void haveMobilePosition(double lon, double lat);
    void haveSignalReading(unsigned id, QString name, double freq, Signal *s);
    void finished();

private:
    FGTelnet *_fg;
    DatabaseApi *_db;

};

#endif // UPDATER_H
