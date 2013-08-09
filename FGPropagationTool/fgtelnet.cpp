#include "fgtelnet.h"
#include <exception>
#include <iostream>

static QString CRLF ="\r\n";

FGTelnet::FGTelnet()
{
    _socket = new QTcpSocket;
    QObject::connect(_socket,SIGNAL(error(QAbstractSocket::SocketError )),this,SLOT(connectionFailed(QAbstractSocket::SocketError)));
    QObject::connect(_socket,SIGNAL(connected()),this,SLOT(connectionSuccess()));
    _connection_tries=0;
    _status=0;
    this->connectToFGFS();
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
    if(error == 0 || error==2)
    {
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
    _status=0;
}


void FGTelnet::connectToFGFS()
{
    if(_status==1) return;
    _socket->connectToHost("localhost", 5500);

}


void FGTelnet::setProperty(QString prop_name, QString value)
{
    QString command = "set " + prop_name + " " + value + CRLF;
    _socket->write(command.toUtf8());
    _socket->flush();

}

QString FGTelnet::getProperty(QString prop_name)
{

    QString command = "get " + prop_name +CRLF;
    _socket->write(command.toLatin1());
    _socket->flush();


    while(!_socket->canReadLine()){}


    QString line;
    bool endOfLine = false;
    try
    {
        while ((!endOfLine))
        {
            if(_status==1)
            {
                char ch;
                int bytesRead = _socket->read(&ch, sizeof(ch));
                if (bytesRead == sizeof(ch))
                {
                    //cnt++;

                    if ((ch == '\r'))
                    {
                        endOfLine = true;
                        while(_socket->bytesAvailable()>0)
                        {
                            char f;
                            _socket->read(&f, sizeof(f));
                        }
                    }
                    else
                    {
                        line.append( ch );
                    }
                }
            }

        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }


    return line.toLatin1();


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
