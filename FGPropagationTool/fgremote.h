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


#ifndef FGREMOTE_H
#define FGREMOTE_H

#include <QPointF>
#include <QThread>
#include <QTime>
#include "fgtelnet.h"
#include "databaseapi.h"
#include <QVector>
#include "groundstation.h"
#include "flightplanpoints.h"
#include "mobilestation.h"


/**
 * @brief Accessory functions which set up the Flightgear environment
 *      through the network interface.
 */
class FGRemote : public QObject
{
    Q_OBJECT
public:
    FGRemote(FGTelnet * t, DatabaseApi * db);
    ~FGRemote();
    void set_mobile(unsigned id=0);
    void set_ground(unsigned id=0);
    void set_fp(unsigned id =0);
    void sendAllData();

signals:
    void readyUpdate();
private:
    FGTelnet *_fg;
    DatabaseApi * _db;

};

#endif // FGREMOTE_H
