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

#ifndef UTIL_H
#define UTIL_H

#include <QPointF>
#include <QString>
#include <QProcess>
#include <QColor>
#include <QStringList>
#include <QVector>
#include <cmath>
#include "databaseapi.h"
#include "flightgearprefs.h"
#include "mobilestation.h"
#include "sg/sg_geodesy.hxx"

/**
 * @brief Static utility functions, used everywhere
 */
class Util
{
public:
    Util();
    ~Util();

    static QPointF convertToLL(QPointF pos, double zoom);
    static QPointF convertToXY(QPointF ll, double zoom);
    static void startFlightgear(DatabaseApi *db);
    static QVector<SGGeod*>* drawDisk(const SGGeod &center, const double &radius,
                                      const int &step_deg, const double &step_point, const bool &distance_precision = false);
    static QColor getScaleColor(double &signal, int &alpha);
};

#endif // UTIL_H
