#include "srtmreader.h"
#include <cmath>
#include <fstream>
#include <stdlib.h>

using namespace std;

SRTMReader::SRTMReader(DatabaseApi *db)
{
    QVector<FlightgearPrefs *> prefs = db->select_prefs();
    if(prefs.size()>0)
    {
        _settings = prefs[0];
    }
    else
        _settings = 0;

    _latitude = 0;
    _longitude = 0;
    _latitude_secs = 0;
    _longitude_secs = 0;
}

void SRTMReader::setCoordinates(double &lat, double &lon)
{
    _latitude = lat;
    _longitude = lon;
}


double SRTMReader::readHeight()
{
    QString srtm_dir = _settings->_srtm_path;
    srtm_dir.append(QDir::separator());
    unsigned temp_row  = (unsigned) round(_latitude_secs /3);
    unsigned temp_column = (unsigned) round(_longitude_secs/3);
    unsigned row = 1201 - temp_row - 1;
    // SRTM 3 is 1201x1201, 2 bytes per sample, we have to read from lower left
    unsigned pos = (row * 1201 + (temp_column - 1)) * 2;
    QString filename = this->getFilename();
    srtm_dir.append(filename);
    if(filename != _last_filename)
    {

    }
    char *height_buf = new char[2];
    ifstream file (srtm_dir.toStdString().c_str(), ifstream::binary);
    if(file)
    {
        file.seekg(pos);
        char *buf = new char[2];
        file.read(buf,2);
        height_buf[0]=buf[1];
        height_buf[1]=buf[0];
        delete[] buf;
    }
    double height = atof(height_buf);
    delete[] height_buf;
    if (height != -32768.0)
        return height;
    else
        return 0.0;

}

QString SRTMReader::getFilename()
{
    QString filename;
    if(_latitude >=0) filename.append("N");
    else filename.append("S");
    unsigned lat_deg = (unsigned) floor(fabs(_latitude));
    unsigned lon_deg = (unsigned) floor(fabs(_longitude));
    _latitude_secs = _latitude - lat_deg;
    _longitude_secs = _longitude - lon_deg;
    filename.append(QString::number(lat_deg));
    if(_longitude >=0) filename.append("E");
    else filename.append("W");
    QString lon = QString::number(lon_deg);
    if (lon.length() < 3) filename.append("0");
    filename.append(lon);
    filename.append(".hgt");
    return filename;
}
