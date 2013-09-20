#ifndef SIGNAL_H
#define SIGNAL_H

#include <QString>

/**
 * @brief Struct holding the properties of a radio signal
 */
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
    double polarization_attenuation;
    double field_strength_uv_meter;
    double tx_pattern_gain;
    double rx_pattern_gain;
    double distance;
    double rx_height;
    double tx_height;
    double tx_erp;
    QString prop_mode;
    QString station_name;
    double frequency;
};

#endif // SIGNAL_H
