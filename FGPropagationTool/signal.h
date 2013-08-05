#ifndef SIGNAL_H
#define SIGNAL_H

#include <QString>

class Signal
{
public:
    Signal();
    double signal_dbm;
    double signal;
    double field_strength_uv;
    double link_budget;
    double terrain_attenuation;
    double clutter_attenuation;
    QString prop_mode;
    QString station_name;
    double frequency;
};

#endif // SIGNAL_H
