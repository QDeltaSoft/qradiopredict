#ifndef APRSICON_H
#define APRSICON_H

#include <QString>
#include <QPointF>

class AprsIcon
{
public:
    AprsIcon();
    QString icon;
    QPointF position;
    QString callsign;
    unsigned time_seen;
};

#endif // APRSICON_H
