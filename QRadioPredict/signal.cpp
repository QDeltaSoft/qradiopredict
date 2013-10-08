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

#include "signal.h"

Signal::Signal()
{
    signal_dbm = 0.0;
    signal = 0.0;
    field_strength_uv = 0.0;
    link_budget  = 0.0;
    terrain_attenuation = 0.0;
    clutter_attenuation = 0.0;
    polarization_attenuation = 0.0;
    field_strength_uv_meter = 0.0;
    tx_pattern_gain = 0.0;
    rx_pattern_gain = 0.0;
    rx_height = 0.0;
    tx_height = 0.0;
    tx_erp  = 1.0;
    prop_mode="";
    station_name="";
    frequency = 140.0;
}
