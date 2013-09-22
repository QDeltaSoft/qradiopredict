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

}

ShpReader::~ShpReader()
{

    QMapIterator<QString*,QString*> it(_terrain_types);
    while(it.hasNext())
    {
        it.next();
        delete it.key();
        delete it.value();
    }
    _terrain_types.clear();
}

void ShpReader::setCoordinates(double lat, double lon)
{
    _latitude = lat;
    _longitude = lon;
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
        if(type == "none")
        {
            continue;
        }
        else
            return type;
    }
    return QString("none");
}


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
        return QString("none");
    }

    OGRLayer  *poLayer;

    poLayer = poDS->GetLayer( 0 );
    if(poLayer == NULL)
    {
        qDebug() << "Shapefile layer is fubar: " << poLayer->GetName();
        OGRDataSource::DestroyDataSource( poDS );
        return QString("none");
    }


    OGRFeature *poFeature;

    poLayer->ResetReading();
    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {

        OGRGeometry *poGeometry;

        poGeometry = poFeature->GetGeometryRef();
        if( poGeometry != NULL)
        {

            // this code relies on binary export; multipolygons are skipped
            int buf_size = poGeometry->WkbSize();
            unsigned char *buffer = new unsigned char[buf_size];
            OGRwkbByteOrder b (wkbXDR);
            OGRwkbGeometryType type = poGeometry->getGeometryType();
            poGeometry->exportToWkb(b,buffer);
            OGRLinearRing *ring;
            OGRPolygon *poly = 0;
            if(type==wkbPolygon)
            {
                poly = new OGRPolygon;
                poly->importFromWkb(buffer,buf_size);
                ring = poly->getExteriorRing();
            }
            else if(type==wkbMultiPolygon)
            {
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
                delete[] buffer;
                OGRFeature::DestroyFeature( poFeature );
                continue;
            }
            int line_size = ring->getNumPoints();
            double *xCoord = new double[line_size];
            double *yCoord = new double[line_size];
            ring->getPoints(xCoord,1,yCoord,1);
            qDebug() << "num points: " << line_size;
            if(pointInPoly(line_size,xCoord,yCoord,_longitude,_latitude))
            {
                qDebug() << "pIp: " << terrain_type;
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

