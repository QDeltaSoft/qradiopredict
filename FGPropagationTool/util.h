#ifndef UTIL_H
#define UTIL_H

#include <QPointF>
#include <QString>
#include <QProcess>
#include <QStringList>
#include <QVector>
#include <cmath>
#include "databaseapi.h"
#include "flightgearprefs.h"
#include "mobilestation.h"

class Util
{
public:
    Util();
    ~Util();

    static QPointF convertToLL(QPointF pos, double zoom);
    static QPointF convertToXY(QPointF ll, double zoom);
    static void startFlightgear();
};

#endif // UTIL_H
