#ifndef APRSSTATION_H
#define APRSSTATION_H

#include <QString>

/**
 * @brief Object holding the properties of an APRS station
 */
class AprsStation
{
public:
    AprsStation();
    QString callsign;
    QString adressee;
    QString via;
    QString symbol;
    QString payload;
    QString message;
    double latitude;
    double longitude;
    uint time_seen;
    QString getImage();
};

#endif // APRSSTATION_H
