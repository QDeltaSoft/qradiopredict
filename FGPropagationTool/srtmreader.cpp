// Written by Adrian Musceac YO8RZZ, started August 2013.
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

#include "srtmreader.h"


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
    _last_filename = "";
    _tiles = new QMap<QString, char*>;
}

SRTMReader::~SRTMReader()
{
    delete _tiles;
}

void SRTMReader::loadTiles(double lon, double lat)
{

    int lat_deg = (int) floor(fabs(lat));
    int lon_deg = (int) floor(fabs(lon));
    int min_lat = lat_deg-3;
    int max_lat = lat_deg+3;
    int min_lon = lon_deg-3;
    int max_lon = lon_deg+3;

    for(int i=min_lat;i<=max_lat;i++)
    {
        for(int j=min_lon;j<=max_lon;j++)
        {
            QString srtm_dir = _settings->_srtm_path;
            srtm_dir.append(QDir::separator());
            QString filename;
            if(i >=0) filename.append("N");
            else filename.append("S");
            QString lat = QString::number(i);
            if (lat.length() < 2) filename.append("0");
            filename.append(QString::number(i));
            if(j >=0) filename.append("E");
            else filename.append("W");
            QString lon = QString::number(j);
            if (lon.length() < 3) filename.append("0");
            if (lon.length() < 2) filename.append("0");
            filename.append(lon);
            filename.append(".hgt");
            srtm_dir.append(filename);
            char *buffer = new char[2884802];
            ifstream file;
            file.open (srtm_dir.toStdString().c_str(), ifstream::binary);
            if(file.is_open())
            {
                file.read(buffer,2884802);
            }
            else
            {
                qDebug() << "SRTM path: " << srtm_dir << " unable to open";
                continue;
            }

            _tiles->insert(filename,buffer);
        }
    }
}

void SRTMReader::unloadTiles()
{
    QMapIterator<QString, char*> it(*_tiles);
    while(it.hasNext())
    {
        it.next();
        delete[] it.value();
    }
    _tiles->clear();
}

void SRTMReader::setCoordinates(double lat, double lon)
{
    _latitude = lat;
    _longitude = lon;
}

// would be memory storage read
double SRTMReader::readHeight()
{
    QString filename = this->getFilename();
    unsigned temp_row  = (unsigned) round(_latitude_secs *3600/3);
    unsigned temp_column = (unsigned) round(_longitude_secs *3600/3);
    unsigned row = 1201 - temp_row - 1;
    // SRTM 3 is 1201x1201, 2 bytes per sample, we have to read from lower left
    unsigned pos = (row * 1201 + (temp_column - 1)) * 2;
    char *buffer = _tiles->value(filename,NULL);
    if(!buffer)
        return 0.0;
    union {
        unsigned char height_buf[2];
        short height;
    } conv;

    conv.height_buf[0]=buffer[pos+1];
    conv.height_buf[1]=buffer[pos];

    if (conv.height != -32768)
        return (double) conv.height;
    else
        return 0.0;
}

// classic method, works well but is slow
double SRTMReader::readHeight2()
{
    QString filename = this->getFilename();
    QString srtm_dir = _settings->_srtm_path;
    srtm_dir.append(QDir::separator());
    unsigned temp_row  = (unsigned) round(_latitude_secs *3600/3);
    unsigned temp_column = (unsigned) round(_longitude_secs *3600/3);
    unsigned row = 1201 - temp_row - 1;
    // SRTM 3 is 1201x1201, 2 bytes per sample, we have to read from lower left
    unsigned pos = (row * 1201 + (temp_column - 1)) * 2;

    srtm_dir.append(filename);
    if(filename != _last_filename)
    {
        if(_last_filename!="")
        {
            _file.close();
        }
        _file.open (srtm_dir.toStdString().c_str(), ifstream::binary);
    }
    _last_filename = filename;
    // TODO: this is not portable across archs, due to the little/big endian issue
    union {
        unsigned char height_buf[2];
        short height;
    } conv;


    if(_file.is_open())
    {
        _file.seekg(pos);
        char *buf = new char[2];
        _file.read(buf,2);
        conv.height_buf[0]=buf[1];
        conv.height_buf[1]=buf[0];
        delete[] buf;
    }
    else
    {
        qDebug() << "SRTM path: " << srtm_dir << " unable to open";
        return 0.0;
    }


    if (conv.height != -32768)
        return (double) conv.height;
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
    QString lat = QString::number(lat_deg);
    if (lat.length() < 2) filename.append("0");
    filename.append(QString::number(lat_deg));
    if(_longitude >=0) filename.append("E");
    else filename.append("W");
    QString lon = QString::number(lon_deg);
    if (lon.length() < 3) filename.append("0");
    if (lon.length() < 2) filename.append("0");
    filename.append(lon);
    filename.append(".hgt");
    return filename;
}

