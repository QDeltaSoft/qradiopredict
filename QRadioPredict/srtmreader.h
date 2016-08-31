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

#ifndef SRTMREADER_H
#define SRTMREADER_H

#include <QString>
#include <QVector>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QMap>
#include <QMapIterator>
#include "flightgearprefs.h"
#include "databaseapi.h"
#include <cmath>
#include <fstream>
#include <stdlib.h>

/**
 * @brief A simple SRTM hgt file reader
 *      The SRTM file should be 1 degree wide, 1 degree high,
 *      and be exactly 2884802 bytes long
 */
using namespace std;
class SRTMReader
{

public:
    explicit SRTMReader(DatabaseApi *db);
    ~SRTMReader();
    void setCoordinates(double lat, double lon);
    double readHeight();
    double readHeightCache();
    void loadTiles(double lon, double lat);
    void unloadTiles();


private:
    double _latitude;
    double _longitude;
    double _latitude_secs;
    double _longitude_secs;
    QString getFilename();
    FlightgearPrefs *_settings;
    QString _last_filename;
    ifstream _file;
    char *_buffer;
    QMap<QString, char*> *_tiles;
    
};


#endif // SRTMREADER_H
