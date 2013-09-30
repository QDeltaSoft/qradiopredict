#ifndef UTIL_H
#define UTIL_H

#include <QPointF>
#include <QString>
#include <QProcess>
#include <QColor>
#include <QStringList>
#include <QVector>
#include <cmath>
#include "databaseapi.h"
#include "flightgearprefs.h"
#include "mobilestation.h"
#include "sg/sg_geodesy.hxx"

class Util
{
public:
    Util();
    ~Util();

    static QPointF convertToLL(QPointF pos, double zoom);
    static QPointF convertToXY(QPointF ll, double zoom);
    static void startFlightgear(DatabaseApi *db);
    static QVector<SGGeod*>* drawDisk(const SGGeod &center, const double &radius,
                                      const int &step_deg, const double &step_point);
    static QColor getScaleColor(double &signal, int &alpha);
};

#endif // UTIL_H
