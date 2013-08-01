#ifndef MOBILESTATION_H
#define MOBILESTATION_H

#include <QString>

class MobileStation
{
public:
    unsigned id;
    unsigned id_session;
    QString name;
    double longitude;
    double latitude;
    double current_longitude;
    double current_latitude;
    double heading_deg;
    double elevation_feet;
    double speed;
    double frequency;
    double tx_power_watt;
    unsigned terrain_following;
    unsigned created_on;
    MobileStation();
};

#endif // MOBILESTATION_H
