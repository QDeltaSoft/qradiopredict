#ifndef UTIL_H
#define UTIL_H

#include <QPointF>

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
