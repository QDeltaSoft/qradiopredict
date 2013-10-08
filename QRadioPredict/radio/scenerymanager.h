#ifndef SCENERYMANAGER_H
#define SCENERYMANAGER_H

#include <string>
#include "sg/compiler.h"
#include "sg/sg_geodesy.hxx"
#include "../srtmreader.h"
#include "../shpreader.h"
#include "../databaseapi.h"
#include "../flightgearprefs.h"

using std::string;
/**
 * @brief Proxy class for the real readers
 */
class SceneryManager
{
public:
    SceneryManager(DatabaseApi *db, FlightgearPrefs *settings);
    ~SceneryManager();
    bool get_elevation_m(const SGGeod &probe, double &elevation_m, string &material);
    void preloadTiles(double lon, double lat);
    void unloadTiles();

private:
    SRTMReader *_srtmreader;
    ShpReader *_shpreader;
    FlightgearPrefs *_settings;

};

#endif // SCENERYMANAGER_H
