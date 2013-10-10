// Written by Adrian Musceac YO8RZZ at gmail dot com, started August 2013.
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
