#include "scenerymanager.h"

SceneryManager::SceneryManager(DatabaseApi *db)
{
    _srtmreader = new SRTMReader(db);
    _shpreader = new ShpReader(db);
}

bool SceneryManager::get_elevation_m(const SGGeod &probe, double &elevation_m, string &material)
{
    _srtmreader->setCoordinates(probe.getLatitudeDeg(),probe.getLongitudeDeg());
    elevation_m = _srtmreader->readHeight();
    if(!material.find("#"))
    {
        _shpreader->setCoordinates(probe.getLatitudeDeg(),probe.getLongitudeDeg());
        QString mat = _shpreader->getTerrainType();
        material = mat.toStdString();
    }
    return true;
}
