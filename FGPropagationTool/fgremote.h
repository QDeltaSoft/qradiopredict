#ifndef FGREMOTE_H
#define FGREMOTE_H

#include <QPointF>
#include <QThread>
#include <QTime>
#include "fgtelnet.h"
#include "databaseapi.h"
#include <QVector>
#include "groundstation.h"
#include "flightplanpoints.h"
#include "mobilestation.h"


/**
 * @brief Accessory functions which set up the Flightgear environment
 */
class FGRemote : public QObject
{
    Q_OBJECT
public:
    FGRemote(FGTelnet * t, DatabaseApi * db);
    ~FGRemote();
    void set_mobile(unsigned id=0);
    void set_ground(unsigned id=0);
    void set_fp(unsigned id =0);
    void sendAllData();

signals:
    void readyUpdate();
private:
    FGTelnet *_fg;
    DatabaseApi * _db;

};

#endif // FGREMOTE_H
