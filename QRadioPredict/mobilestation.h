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

#ifndef MOBILESTATION_H
#define MOBILESTATION_H

#include <QString>

/**
 * @brief Struct holding the properties of a mobile station
 */
class MobileStation
{
public:
    int id;
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
