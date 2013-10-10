// Written by Adrian Musceac YO8RZZ at gmail dot com, started August 2013.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SHPREADER_H
#define SHPREADER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QDebug>
#include "ogrsf_frmts.h"
#include "databaseapi.h"
#include "flightgearprefs.h"

/**
 * @brief A simple shapefile reader class
 *          It will open a shapefile and
 *          search within it's layer the point
 *          The shapefile needs to be 1 degree wide
 */
class ShpReader
{

public:
    ShpReader(DatabaseApi *db);
    ~ShpReader();
    void setCoordinates(double lat, double lon);
    QString getTerrainType();

private:
    QMap<QString*, QString*> _terrain_types;
    bool pointInPoly(int polySize, double XPoints[], double YPoints[], double x, double y);
    QString openShapefile(QString &name, QString &terrain_type);
    QString getFilename();
    FlightgearPrefs *_settings;
    double _latitude;
    double _longitude;
    
};

#endif // SHPREADER_H
