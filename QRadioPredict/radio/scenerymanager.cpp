#include "scenerymanager.h"
#include <QDebug>

SceneryManager::SceneryManager(DatabaseApi *db, FlightgearPrefs *settings)
{
    _srtmreader = new SRTMReader(db);

    _settings = settings;
    if(_settings->_use_clutter == 1)
    {
        _shpreader = new ShpReader(db);
    }
}

SceneryManager::~SceneryManager()
{
    delete _srtmreader;
    if(_settings->_use_clutter == 1)
    {
        delete _shpreader;
    }
}

void SceneryManager::preloadTiles(double lon, double lat)
{
    _srtmreader->loadTiles(lon,lat);
}

void SceneryManager::unloadTiles()
{
    _srtmreader->unloadTiles();
}

bool SceneryManager::get_elevation_m(const SGGeod &probe, double &elevation_m, string &material)
{

    _srtmreader->setCoordinates(probe.getLatitudeDeg(),probe.getLongitudeDeg());
    elevation_m = _srtmreader->readHeightCache();

    if(_settings->_use_clutter == 1)
    {
        _shpreader->setCoordinates(probe.getLatitudeDeg(),probe.getLongitudeDeg());
        QString mat = _shpreader->getTerrainTypeFromRaster();
        material = mat.toStdString();
    }
    else
    {
        material = "None";
    }

    return true;
}
