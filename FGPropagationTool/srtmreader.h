#ifndef SRTMREADER_H
#define SRTMREADER_H

#include <QString>
#include <QVector>
#include <QFile>
#include <QDir>
#include <QDebug>
#include "flightgearprefs.h"
#include "databaseapi.h"
#include <cmath>
#include <fstream>
#include <stdlib.h>

/**
 * @brief A simple SRTM hgt file reader
 */
using namespace std;
class SRTMReader
{

public:
    explicit SRTMReader(DatabaseApi *db);
    void setCoordinates(double lat, double lon);
    double readHeight();


private:
    double _latitude;
    double _longitude;
    double _latitude_secs;
    double _longitude_secs;
    QString getFilename();
    FlightgearPrefs *_settings;
    QString _last_filename;
    ifstream _file;
    
};


#endif // SRTMREADER_H
