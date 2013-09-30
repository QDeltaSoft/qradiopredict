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
