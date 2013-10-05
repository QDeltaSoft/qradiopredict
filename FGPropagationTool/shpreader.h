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
