#ifndef FGTELNET_H
#define FGTELNET_H
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


#include <QTcpSocket>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QCoreApplication>
#include <QLatin1String>

/**
 * @brief Interface class to network Flightgear props server
 */
class FGTelnet : public QObject
{
    Q_OBJECT
public:
    FGTelnet();
    ~FGTelnet();

    void setProperty(QString prop_name, QString value);

    void runCmd(QString cmd);
    void cd(QString dir);
    void dataMode();
    void promptMode();
    void disconnectFromFGFS();
    unsigned inline status() {return _status;}

public slots:
    void processData();
    void connectToFGFS();
    void getProperty(QString prop_name);

signals:
    void connectionFailure();
    void connectedToFGFS();
    void haveProperty(QString prop);

private:
    QTcpSocket *_socket;
    unsigned _connection_tries;
    unsigned _status;


private slots:
    void connectionSuccess();
    void connectionFailed(QAbstractSocket::SocketError error);

};

#endif // FGTELNET_H
