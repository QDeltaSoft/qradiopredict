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

#include "flightgearprefs.h"

FlightgearPrefs::FlightgearPrefs()
{
    _fgfs_bin = "";
    _fgdata_path = "";
    _scenery_path = "";
    _aircraft = "";
    _airport = "";
    _use_clutter = 0;
    _use_antenna_pattern = 0;
    _itm_radio_performance = 1;
    _windowX = 1024;
    _windowY = 768;
    _aprs_server = "euro.aprs2.net";
    _aprs_filter_range = 200;
    _srtm_path = "";
    _shapefile_path = "";
    _plot_range = 30;
}
