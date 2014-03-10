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


#ifndef APRS_H
#define APRS_H

#include <QObject>
#include <QTcpSocket>
#include <QPointF>
#include <QString>
#include <QStringList>

#include <QRegExp>
#include "aprsstation.h"
#include <QTime>
#include <QDateTime>

/**
 * @brief APRS connection interface
 *      We are using the standard APRS network protocol
 *      All communication takes place with the tier two servers,
 *      on port 14580
 *      Raw data is parsed and sent to be displayed on the map
 */

class Aprs : public QObject
{
    Q_OBJECT
public:
    explicit Aprs( QString aprs_server);
    ~Aprs();
    void connectToAPRS();
    void disconnectAPRS();
    void setFilter(QPointF &pos, int &range);
    void filterCallsign(QString callsign);
    void filterPrefix(QString prefix);
    
signals:
    void connectedToAPRS();
    void connectionFailure();
    void aprsData(AprsStation *st);
    void rawAprsData(QString data);
    
public slots:
    void connectionSuccess();
    void connectionFailed(QAbstractSocket::SocketError error);
    void processData();

private:
    void authenticate();
    QString _hostname;
    QTcpSocket *_socket;
    quint8 _connection_tries;
    unsigned _status;
    unsigned _authenticated;
    QTime _delaytime;
    
};

#endif // APRS_H
