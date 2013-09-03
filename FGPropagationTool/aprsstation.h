#ifndef APRSSTATION_H
#define APRSSTATION_H

#include <QString>

class AprsStation
{
public:
    AprsStation();
    QString callsign;
    QString adressee;
    QString via;
    QString payload;
    double latitude;
    double longitude;
};

#endif // APRSSTATION_H
