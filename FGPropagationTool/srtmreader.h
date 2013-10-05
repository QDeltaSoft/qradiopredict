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
    double readHeight2();
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
