#include "groundstation.h"

GroundStation::GroundStation()
{
    id  = 0;
    id_session = 0;
    name = "";
    longitude = 0.0;
    latitude = 0.0;
    heading_deg = 0.0;
    pitch_deg = 0.0;
    elevation_feet = 0.0;
    frequency = 140.0;
    beacon_delay = 2;
    transmission_type =1;
    polarization = 1;
    rx_antenna_height = 0.0;
    rx_antenna_gain = 0.0;
    rx_line_losses = 0.0;
    rx_sensitivity = -120.0;
    rx_antenna_type = "";
    tx_antenna_gain = 0.0;
    tx_antenna_height = 0.0;
    tx_line_losses = 0.0;
    tx_antenna_type = "";
    tx_power_watt  = 1.0;
    enabled = 0;
    created_on = 0;
    last_update  = 0;
}
