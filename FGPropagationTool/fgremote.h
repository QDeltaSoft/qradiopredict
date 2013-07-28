#ifndef FGREMOTE_H
#define FGREMOTE_H

#include <QPointF>
#include "fgtelnet.h"

class FGRemote
{
public:
    FGRemote(FGTelnet * t);
    void set_mobile(QPointF pos);
    void set_ground(QPointF pos);
    void set_fp(QPointF pos);

private:
    FGTelnet *_fg;

};

#endif // FGREMOTE_H
