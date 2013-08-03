#ifndef FGREMOTE_H
#define FGREMOTE_H

#include <QPointF>
#include "fgtelnet.h"
#include "databaseapi.h"
#include <QVector>
#include "groundstation.h"
#include "flightplanpoints.h"
#include "mobilestation.h"

class FGRemote
{
public:
    FGRemote(FGTelnet * t, DatabaseApi * db);
    void set_mobile(unsigned id=0);
    void set_ground(unsigned id=0);
    void set_fp(unsigned id =0);

private:
    FGTelnet *_fg;
    DatabaseApi * _db;

};

#endif // FGREMOTE_H
