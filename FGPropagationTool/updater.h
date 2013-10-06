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


#ifndef UPDATER_H
#define UPDATER_H

#include "databaseapi.h"
#include "fgtelnet.h"
#include "signal.h"
#include <math.h>
#include <QObject>
#include <QTime>
#include <QDateTime>
#include <QVector>

/**
 * @brief Thread object which polls the radio result in a separate thread
 */
class Updater : public QObject
{
    Q_OBJECT
public:
    Updater( DatabaseApi *d);
    ~Updater();

public slots:
    void startUpdate();
    void setReceived(QString prop_data);
    void stop();

signals:
    void haveMobilePosition(double lon, double lat);
    void haveSignalReading(double lon, double lat, unsigned id, QString name, double freq, Signal *s);
    void finished();
    void getProperty(QString prop_data);

private:
    FGTelnet *_fg;
    DatabaseApi *_db;
    unsigned _received;
    QString _prop_data;
    unsigned _run;
};

#endif // UPDATER_H
