#ifndef FLIGHTGEARPREFS_H
#define FLIGHTGEARPREFS_H

#include <QString>

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
    QString _srtm_path;
    QString _shapefile_path;
};

#endif // FLIGHTGEARPREFS_H
