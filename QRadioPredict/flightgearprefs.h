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

#ifndef FLIGHTGEARPREFS_H
#define FLIGHTGEARPREFS_H

#include <QString>

/**
 * @brief Object holding Application preferences as retrieved from the database
 */
class FlightgearPrefs
{
public:
    FlightgearPrefs();
    QString _fgfs_bin;
    QString _fgdata_path;
    QString _scenery_path;
    QString _aircraft;
    QString _airport;
    unsigned _use_clutter;
    unsigned _use_antenna_pattern;
    unsigned _itm_radio_performance;
    unsigned _windowX;
    unsigned _windowY;
    QString _aprs_server;
    QString _aprs_settings;
    int _aprs_filter_range;
    QString _srtm_path;
    QString _shapefile_path;
    int _plot_range; // km
    unsigned _use_ITWOM;
    unsigned _scale_with_distance;
    double _init_latitude;
    double _init_longitude;
};

#endif // FLIGHTGEARPREFS_H
