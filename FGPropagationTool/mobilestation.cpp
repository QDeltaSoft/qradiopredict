#include "mobilestation.h"

MobileStation::MobileStation()
{
    id = 0;
    id_session = 0;
    name = "";
    longitude = 0.0;
    latitude = 0.0;
    current_latitude = 0.0;
    current_longitude = 0.0;
    heading_deg = 0.0;
    elevation_feet = 0.0;
    speed = 0.0;
    frequency = 140.0;
    tx_power_watt = 1.0;
    terrain_following = 1;
    created_on = 0;
}
