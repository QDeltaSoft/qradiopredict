#ifndef SCENERYMANAGER_H
#define SCENERYMANAGER_H

#include <string>
#include "sg/compiler.h"
#include "sg/sg_geodesy.hxx"
#include "../srtmreader.h"
#include "../shpreader.h"
#include "../databaseapi.h"

using namespace std;
class SceneryManager
{
public:
    SceneryManager(DatabaseApi *db);
    bool get_elevation_m(const SGGeod &probe, double &elevation_m, string &material);

private:
    SRTMReader *_srtmreader;
    ShpReader *_shpreader;

};

#endif // SCENERYMANAGER_H
