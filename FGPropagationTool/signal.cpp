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
