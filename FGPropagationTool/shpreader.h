#ifndef SHPREADER_H
#define SHPREADER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QDebug>
#include "ogrsf_frmts.h"

/**
 * @brief A simple shapefile reader class
 *          It will open a shapefile and
 *          search within it's layer the point
 */
class ShpReader
{

public:
    ShpReader();
    ~ShpReader();
    void setCoordinates(double &lat, double &lon);
    QString getTerrainType();
signals:
    
public slots:

private:
    QMap<QString*, QString*> _terrain_types;
    OGRPoint *_point;
    QString openShapefile(QString &name, QString &terrain_type);
    
};

#endif // SHPREADER_H
