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


#include "fgtelnet.h"
#include <exception>
#include <iostream>

static QString CRLF ="\r\n";

FGTelnet::FGTelnet()
{
    _socket = new QTcpSocket;
    QObject::connect(_socket,SIGNAL(error(QAbstractSocket::SocketError )),this,SLOT(connectionFailed(QAbstractSocket::SocketError)));
    QObject::connect(_socket,SIGNAL(connected()),this,SLOT(connectionSuccess()));
    QObject::connect(_socket,SIGNAL(readyRead()),this,SLOT(processData()));
    _connection_tries=0;
    _status=0;
    //this->connectToFGFS();
}

FGTelnet::~FGTelnet()
{
    _socket->disconnectFromHost();
    delete _socket;
}


void FGTelnet::connectionSuccess()
{
    qDebug("Connection to Flightgear established.");
    this->dataMode();
    _status=1;
    _connection_tries=0;
    emit connectedToFGFS();
}


void FGTelnet::connectionFailed(QAbstractSocket::SocketError error)
{
    _status=0;
    qDebug("OOps! Could not connect to FGFS. Trying again.");
    _connection_tries++;
    if(_connection_tries < 10)
    {
        this->connectToFGFS();
    }
    else
    {
        qDebug("Giving up! Flightgear is not running.");
        emit connectionFailure();
    }

}


void FGTelnet::connectToFGFS()
{
    if(_status==1) return;
    _socket->connectToHost("localhost", 5500);

}

void FGTelnet::disconnectFromFGFS()
{
    _socket->disconnectFromHost();
    _status=0;
    _connection_tries=0;
    emit connectionFailure();
}


void FGTelnet::setProperty(QString prop_name, QString value)
{
    QString command = "set " + prop_name + " " + value + CRLF;
    _socket->write(command.toUtf8());
    _socket->flush();

}

void FGTelnet::processData()
{
    if (_status !=1) return;

    QString line;

    bool endOfLine = false;

    while ((!endOfLine))
    {
        if(_status==1)
        {
            char ch;
            if(_socket->bytesAvailable()>0)
            {
                int bytesRead = _socket->read(&ch, sizeof(ch));
                if (bytesRead == sizeof(ch))
                {
                    //cnt++;

                    if ((ch == '\r'))
                    {
                        endOfLine = true;


                    }
                    else
                    {
                        line.append( ch );
                    }
                }
            }
            else
            {
                continue;
            }
        }

    }


    emit haveProperty(line);
}

void FGTelnet::getProperty(QString prop_name)
{

    QString command = "get " + prop_name +CRLF;
    int check = _socket->write(command.toLatin1());
    if(check==-1)
    {
        qDebug() << "Command was not written, an error has occured";
    }
    else if(check < qstrlen(command.toLatin1()))
    {
        qDebug() << "Fewer than required bytes written to socket";
    }

    _socket->flush();

}

void FGTelnet::cd(QString dir)
{
    qDebug() << "dir: " << dir;
    QString command = "cd " + dir + " " +CRLF;
    _socket->write(command.toUtf8());
    _socket->flush();
}


void FGTelnet::runCmd(QString cmd)
{
    QString command = "run " + cmd + CRLF;
    _socket->write(command.toUtf8());
    _socket->flush();

}

void FGTelnet::dataMode()
{
    QString command = "data " + CRLF;
    _socket->write(command.toUtf8());
    _socket->flush();

}

void FGTelnet::promptMode()
{
    QString command = "prompt " + CRLF;
    _socket->write(command.toUtf8());
    _socket->flush();

}
