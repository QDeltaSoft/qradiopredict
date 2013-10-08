// Written by Adrian Musceac YO8RZZ, started August 2013.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef GROUNDSTATION_H
#define GROUNDSTATION_H

#include <QString>

/**
 * @brief Struct holding the properties of a ground station
 */

class GroundStation
{
public:
    int id;
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
