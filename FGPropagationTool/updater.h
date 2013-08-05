#ifndef UPDATER_H
#define UPDATER_H

#include "databaseapi.h"
#include "fgtelnet.h"
#include "signal.h"
#include <QObject>

class Updater : public QObject
{
    Q_OBJECT
public:
    Updater(FGTelnet *t, DatabaseApi *db);

public slots:
    void startUpdate();

signals:
    void haveMobilePosition(double lon, double lat);
    void haveSignalReading(Signal *s);

private:
    FGTelnet *_fg;
    DatabaseApi *_db;

};

#endif // UPDATER_H
