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


#include "shpreader.h"


ShpReader::ShpReader(DatabaseApi *db)
{
    QVector<FlightgearPrefs *> prefs = db->select_prefs();
    if(prefs.size()>0)
    {
        _settings = prefs[0];
    }
    else
        _settings = 0;

    _terrain_types.insert(new QString("EvergreenBroadCover"), new QString("c312"));
    _terrain_types.insert(new QString("EvergreenForest"), new QString("c312"));
    _terrain_types.insert(new QString("DeciduousBroadCover"), new QString("c141"));
    _terrain_types.insert(new QString("DeciduousForest"), new QString("c311"));
    _terrain_types.insert(new QString("MixedForestCover"), new QString("c313"));
    _terrain_types.insert(new QString("MixedForest"), new QString("c313"));
    _terrain_types.insert(new QString("RainForest"), new QString("c313"));
    _terrain_types.insert(new QString("EvergreenNeedleCover"), new QString("c311"));
    _terrain_types.insert(new QString("WoodedTundraCover"), new QString("c324"));
    _terrain_types.insert(new QString("DeciduousNeedleCover"), new QString("c311"));
    _terrain_types.insert(new QString("ScrubCover"), new QString("c324"));
    _terrain_types.insert(new QString("ShrubCover"), new QString("c322"));
    _terrain_types.insert(new QString("BuiltUpCover"), new QString("c112"));
    _terrain_types.insert(new QString("Urban"), new QString("c133"));
    _terrain_types.insert(new QString("Construction"), new QString("c111"));
    _terrain_types.insert(new QString("Industrial"), new QString("c111"));
    _terrain_types.insert(new QString("Town"), new QString("c112"));
    _terrain_types.insert(new QString("SubUrban"), new QString("c112"));
    _terrain_types.insert(new QString("CropWoodCover"), new QString("c243"));
    _terrain_types.insert(new QString("CropWood"), new QString("c243"));
    _terrain_types.insert(new QString("AgroForest"), new QString("c243"));

    _corine_raster_terrain_types.push_back(QString("none"));
    _corine_raster_terrain_types.push_back(QString("Urban"));
    _corine_raster_terrain_types.push_back(QString("Town"));
    _corine_raster_terrain_types.push_back(QString("Industrial"));
    _corine_raster_terrain_types.push_back(QString("none"));
    _corine_raster_terrain_types.push_back(QString("Industrial"));
    _corine_raster_terrain_types.push_back(QString("Industrial"));
    _corine_raster_terrain_types.push_back(QString("none"));
    _corine_raster_terrain_types.push_back(QString("none"));
    _corine_raster_terrain_types.push_back(QString("Construction"));
    _corine_raster_terrain_types.push_back(QString("DeciduousForest"));
    _corine_raster_terrain_types.push_back(QString("ShrubCover"));
    _corine_raster_terrain_types.push_back(QString("none"));
    _corine_raster_terrain_types.push_back(QString("none"));
    _corine_raster_terrain_types.push_back(QString("none"));
    _corine_raster_terrain_types.push_back(QString("ScrubCover"));
    _corine_raster_terrain_types.push_back(QString("CropWoodCover"));
    _corine_raster_terrain_types.push_back(QString("CropWoodCover"));
    _corine_raster_terrain_types.push_back(QString("none"));
    _corine_raster_terrain_types.push_back(QString("none"));
    _corine_raster_terrain_types.push_back(QString("none"));
    _corine_raster_terrain_types.push_back(QString("CropWood"));
    _corine_raster_terrain_types.push_back(QString("CropWood"));
    _corine_raster_terrain_types.push_back(QString("DeciduousForest"));
    _corine_raster_terrain_types.push_back(QString("EvergreenForest"));
    _corine_raster_terrain_types.push_back(QString("MixedForest"));
    _corine_raster_terrain_types.push_back(QString("none"));
    _corine_raster_terrain_types.push_back(QString("ScrubCover"));
    _corine_raster_terrain_types.push_back(QString("ShrubCover"));
    _corine_raster_terrain_types.push_back(QString("ShrubCover"));

    this->initGDALraster();

}

ShpReader::~ShpReader()
{
    delete _geoSRS;
    delete _dataSRS;
    delete _coordinate_transform;
    GDALClose(_dataset);
    QMapIterator<QString*,QString*> it(_terrain_types);
    while(it.hasNext())
    {
        it.next();
        delete it.key();
        delete it.value();
    }
    _terrain_types.clear();
}

void ShpReader::initGDALraster()
{
    GDALAllRegister();
    QString shp_dir = _settings->_shapefile_path;
    shp_dir.append(QDir::separator()).append("g100_06.tif");
    _dataset = reinterpret_cast<GDALDataset*>(GDALOpen(shp_dir.toStdString().c_str(), GA_ReadOnly));
    double t[6];
    CPLErr error = _dataset->GetGeoTransform(t);
    if (error == CE_Failure)
    {
        qDebug() << "Failed to open landcover dataset: " << shp_dir;
    }

    _rasterband = _dataset->GetRasterBand(1);
    _data_type = _rasterband->GetRasterDataType();
    GDALColorInterp interp = _rasterband->GetColorInterpretation();
    Q_UNUSED(interp);
    _color_table = _rasterband->GetColorTable();

    _geoSRS = new OGRSpatialReference();
    _geoSRS->importFromEPSG( 4326 ); // WGS84

    const char *proj = _dataset->GetProjectionRef();
    _dataSRS = new OGRSpatialReference( proj );

    _coordinate_transform = OGRCreateCoordinateTransformation( _geoSRS, _dataSRS );
}

void ShpReader::setCoordinates(double lat, double lon)
{
    _latitude = lat;
    _longitude = lon;
}

QString ShpReader::getTerrainTypeFromRaster()
{
    double transform[6];
    _dataset->GetGeoTransform(transform);
    double x = _longitude;
    double y = _latitude;
    _coordinate_transform->Transform( 1, &x, &y );
    double px = ((x - transform[0]) / transform[1]);
    double py = ((y - transform[3]) / transform[5]);
    char *data = (char *) CPLMalloc(sizeof(char));

    _rasterband->RasterIO(GF_Read, px, py, 1, 1, data, 1, 1, _data_type , 0, 0);
    // data is a pointer holding the colour index
    const GDALColorEntry *color = _color_table->GetColorEntry(*data);
    Q_UNUSED(color);
    quint8 index = *data;
    QString terrain_type;
    if (index > _corine_raster_terrain_types.size()-1)
    {
        terrain_type ="none";
    }
    else
    {
        terrain_type = _corine_raster_terrain_types.at(index);
    }


    CPLFree(data);

    return terrain_type;
}

QString ShpReader::getTerrainType()
{
    QString type;
    QMapIterator<QString*,QString*> it(_terrain_types);

    while(it.hasNext())
    {
        it.next();
        QString shp_dir = _settings->_shapefile_path;
        QString filename= this->getFilename();
        shp_dir.append(QDir::separator()).append(filename).append("_");
        shp_dir.append(*(it.value()));
        type = this->openShapefile(shp_dir,*(it.key())); 
        if(type == QString("None"))
        {
            continue;
        }
        else
        {
            return type;
        }
    }
    return QString("none");
}

// I'm trying very hard to make this function faster
// It's extremely slow right now
QString ShpReader::openShapefile(QString &name, QString &terrain_type)
{

    OGRRegisterAll();
    QString file = name;
    file.append(".shp");
    OGRDataSource       *poDS;

    poDS = OGRSFDriverRegistrar::Open( file.toStdString().c_str(), FALSE );
    if( poDS == NULL )
    {
        qDebug() << "Shapefile opening failed: " << name;
        return QString("None");
    }

    OGRLayer  *poLayer;

    poLayer = poDS->GetLayer( 0 );
    if(poLayer == NULL)
    {
        qDebug() << "Shapefile layer is fubar: " << poLayer->GetName();
        OGRDataSource::DestroyDataSource( poDS );
        return QString("None");
    }

    OGRPoint pp;
    pp.setX(_longitude);
    pp.setY(_latitude);
    poLayer->SetSpatialFilter(&pp);
    OGRFeature *poFeature;

    poLayer->ResetReading();
    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {

        OGRGeometry *poGeometry;

        poGeometry = poFeature->GetGeometryRef();
        if( poGeometry != NULL)
        {

            // this code relies on binary export; multipolygons are skipped using GEOS

            int buf_size = poGeometry->WkbSize();
            unsigned char *buffer = new unsigned char[buf_size];
            OGRwkbByteOrder b (wkbXDR);
            OGRwkbGeometryType type = poGeometry->getGeometryType();
            poGeometry->exportToWkb(b,buffer);
            OGRLinearRing *ring;
            OGRPolygon *poly = 0;
            bool clockwise = false;
            if(type==wkbPolygon)
            {
                poly = new OGRPolygon;
                poly->importFromWkb(buffer,buf_size);
                ring = poly->getExteriorRing();
                if(ring->isClockwise())
                {
                    clockwise=true;
                }
            }
            else if(type==wkbMultiPolygon)
            {
                OGRPoint point;
                point.setX(_longitude);
                point.setY(_latitude);


                if(point.Within(poGeometry))
                {
                    qDebug() << "Using GEOS for: " << terrain_type;
                    delete [] buffer;
                    OGRFeature::DestroyFeature( poFeature );
                    OGRDataSource::DestroyDataSource( poDS );
                    return terrain_type;
                }
                delete[] buffer;
                OGRFeature::DestroyFeature( poFeature );
                continue;
            }
            else
            {
                delete[] buffer;
                OGRFeature::DestroyFeature( poFeature );
                continue;
            }
            int line_size = ring->getNumPoints();

            double *xCoord = new double[line_size];
            double *yCoord = new double[line_size];
            for(int k=0;k<line_size;++k)
            {
                OGRPoint *p = new OGRPoint;
                ring->getPoint(k,p);
                if(clockwise)
                {
                    xCoord[line_size - k -1] = p->getX();
                    yCoord[line_size - k -1] = p->getY();
                }
                else
                {
                    xCoord[k] = p->getX();
                    yCoord[k] = p->getY();
                }
                delete p;
            }
            //ring->getPoints(xCoord,0,yCoord,0);
            /* reversing the array happens there ^^
            if(clockwise)
            {
                double temp;
                for (int i=0;i<line_size/2;i++)
                {
                        temp=xCoord[i];
                        xCoord[i]=xCoord[line_size-i-1];
                        xCoord[line_size-i-1]=temp;
                }
                for (int i=0;i<line_size/2;i++)
                {
                        temp=yCoord[i];
                        yCoord[i]=yCoord[line_size-i-1];
                        yCoord[line_size-i-1]=temp;
                }
            }
            */

            if(pointInPoly(line_size,xCoord,yCoord,_longitude,_latitude))
            {
                //qDebug() << "pIp: " << terrain_type << " lat: " << _latitude << " lon: " << _longitude;
                delete[] xCoord;
                delete[] yCoord;
                delete[] buffer;
                if(poly)
                    delete poly;
                OGRFeature::DestroyFeature( poFeature );
                OGRDataSource::DestroyDataSource( poDS );
                return terrain_type;
            }

            delete[] xCoord;
            delete[] yCoord;
            delete[] buffer;
            if(poly)
                delete poly;
            OGRFeature::DestroyFeature( poFeature );
            continue;

            /*
            OGRPoint point;
            point.setX(_longitude);
            point.setY(_latitude);


            if(point.Within(poGeometry))
            {
                //qDebug() << terrain_type;
                OGRFeature::DestroyFeature( poFeature );
                OGRDataSource::DestroyDataSource( poDS );
                return terrain_type;
            }
            */

        }
        else
            qDebug() << "Geometry is fubar";

        OGRFeature::DestroyFeature( poFeature );
    }

    OGRDataSource::DestroyDataSource( poDS );

    return QString("None");
}


QString ShpReader::getFilename()
{
    QString filename;
    if(_latitude >=0) filename.append("N");
    else filename.append("S");
    unsigned lat_deg = (unsigned) floor(fabs(_latitude));
    unsigned lon_deg = (unsigned) floor(fabs(_longitude));
    QString lat = QString::number(lat_deg);
    if (lat.length() < 2) filename.append("0");
    filename.append(QString::number(lat_deg));
    if(_longitude >=0) filename.append("E");
    else filename.append("W");
    QString lon = QString::number(lon_deg);
    if (lon.length() < 3) filename.append("0");
    if (lon.length() < 2) filename.append("0");
    filename.append(lon);
    return filename;
}


// based on the pointInPoly algorithm
bool ShpReader::pointInPoly(int polySize, double XPoints[], double YPoints[], double x, double y)
{

    int j = polySize - 1;
    bool isInside = false;

    for (int i = 0; i < polySize; j = i++)
    {
        if ( ( ( (YPoints[i] <= y) && (y < YPoints[j])) ||
              ( (YPoints[j] <= y) && (y < YPoints[i]))) &&
            (x < (XPoints[j] - XPoints[i]) *
             (y - YPoints[i]) / (YPoints[j] - YPoints[i]) + XPoints[i]))
        {
            isInside = !isInside;
        }
    }

    return isInside;

}

