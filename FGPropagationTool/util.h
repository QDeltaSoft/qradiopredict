#ifndef UTIL_H
#define UTIL_H

#include <QPointF>
#include <QString>
#include <QProcess>
#include <QStringList>
#include <cmath>

class Util
{
public:
    Util();
    ~Util();

    static QPointF convertToLL(QPointF pos, double zoom);
    static QPointF convertToXY(QPointF ll, double zoom);
    static void startFlightgear(const QString &binary="", const QString &fgroot="", const QString &scenery="",
                                const QString &aircraft="", const QString &airport="",
                                const double &lon=-9999.0, const double &lat=-9999.0);
};

#endif // UTIL_H
