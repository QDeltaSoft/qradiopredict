#include "scenerymanager.h"
#include <QDebug>

SceneryManager::SceneryManager(DatabaseApi *db)
{
    _srtmreader = new SRTMReader(db);
    _shpreader = new ShpReader(db);
}

bool SceneryManager::get_elevation_m(const SGGeod &probe, double &elevation_m, string &material)
{

    _srtmreader->setCoordinates(probe.getLatitudeDeg(),probe.getLongitudeDeg());
    elevation_m = _srtmreader->readHeight();
    //material = "None";
    //return true;
    _shpreader->setCoordinates(probe.getLatitudeDeg(),probe.getLongitudeDeg());
    QString mat = _shpreader->getTerrainType();
    material = mat.toStdString();

    return true;
}
