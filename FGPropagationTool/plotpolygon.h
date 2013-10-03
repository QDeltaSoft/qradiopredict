#ifndef PLOTPOLYGON_H
#define PLOTPOLYGON_H

#include <QPointF>
#include <QBrush>
#include <QPen>


class PlotPolygon
{
public:
    PlotPolygon();

    QBrush _brush;
    QPen _pen;
    QPointF _lb;
    QPointF _lt;
    QPointF _rb;
    QPointF _rt;

};

#endif // PLOTPOLYGON_H
