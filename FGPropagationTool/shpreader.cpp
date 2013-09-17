#include "shpreader.h"


ShpReader::ShpReader()
{
    _terrain_types.insert(new QString(""), new QString("")); // must add definitions
    _point = new OGRPoint;
}

ShpReader::~ShpReader()
{
    delete _point;
    _terrain_types.clear();
}

void ShpReader::setCoordinates(double &lat, double &lon)
{
    _point->setX(lat);
    _point->setY(lon);
}

QString ShpReader::getTerrainType()
{
    QString type;
    QMapIterator<QString*,QString*> it(_terrain_types);
    while(it.hasNext())
    {
        it.next();
        type = this->openShapefile(*(it.key()),*(it.value()));
        if(type == "none")
            continue;
        else
            return type;
    }
    return QString("none");
}


QString ShpReader::openShapefile(QString &name, QString &terrain_type)
{

    OGRRegisterAll();

    OGRDataSource       *poDS;

    poDS = OGRSFDriverRegistrar::Open( name.toStdString().c_str(), FALSE );
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
            if(_point->Within(poGeometry))
                return terrain_type;
        }
        else
            qDebug() << "Geometry is fubar";

        OGRFeature::DestroyFeature( poFeature );
    }

    OGRDataSource::DestroyDataSource( poDS );
    return QString("none");
}
