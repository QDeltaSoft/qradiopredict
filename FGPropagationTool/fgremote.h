#ifndef FGREMOTE_H
#define FGREMOTE_H

#include <QPointF>
#include "fgtelnet.h"
#include "databaseapi.h"

class FGRemote
{
public:
    FGRemote(FGTelnet * t, DatabaseApi * db);
    void set_mobile(QPointF pos);
    void set_ground(QPointF pos);
    void set_fp(QPointF pos);

private:
    FGTelnet *_fg;
    DatabaseApi * _db;

};

#endif // FGREMOTE_H
