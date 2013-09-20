#ifndef GROUNDSTATION_H
#define GROUNDSTATION_H

#include <QString>

/**
 * @brief Struct holding the properties of a ground station
 */

class GroundStation
{
public:
    unsigned id;
    unsigned id_session;
    QString name;
    double longitude;
    double latitude;
    double heading_deg;
    double pitch_deg;
    double elevation_feet;
    double frequency;
    unsigned beacon_delay;
    unsigned transmission_type;
    unsigned polarization;
    double rx_antenna_height;
    QString rx_antenna_type;
    double rx_antenna_gain;
    double rx_line_losses;
    double rx_sensitivity;
    double tx_power_watt;
    double tx_antenna_height;
    QString tx_antenna_type;
    double tx_antenna_gain;
    double tx_line_losses;
    unsigned enabled;
    unsigned created_on;
    unsigned last_update;
    GroundStation();
};

#endif // GROUNDSTATION_H
