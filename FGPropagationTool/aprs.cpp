#include "aprs.h"

Aprs::Aprs(QObject *parent) :
    QObject(parent)
{
    _socket = new QTcpSocket;
    QObject::connect(_socket,SIGNAL(error(QAbstractSocket::SocketError )),this,SLOT(connectionFailed(QAbstractSocket::SocketError)));
    QObject::connect(_socket,SIGNAL(connected()),this,SLOT(connectionSuccess()));
    QObject::connect(_socket,SIGNAL(readyRead()),this,SLOT(processData()));
    _connection_tries=0;
    _status=0;
    _authenticated = 0;
    this->connectToAPRS();
}


Aprs::~Aprs()
{
    _socket->disconnectFromHost();
    delete _socket;
}


void Aprs::connectionSuccess()
{
    qDebug("Connection to APRS established.");

    _status=1;
    _connection_tries=0;

    this->authenticate();

    emit connectedToAPRS();
}


void Aprs::connectionFailed(QAbstractSocket::SocketError error)
{
    _status=0;
    qDebug("OOps! Could not connect to APRS. Trying again.");
    _connection_tries++;
    if(_connection_tries < 10)
    {
        this->connectToAPRS();
    }
    else
    {
        qDebug("Giving up! APRS is not reachable.");
        emit connectionFailure();
    }

}


void Aprs::connectToAPRS()
{
    if(_status==1) return;
    _socket->connectToHost("euro.aprs2.net", 14580);

}

void Aprs::authenticate()
{
    _socket->write("user XASTIR pass -1 filter r/46.00/26.00/200\r\n");
    _socket->flush();
    qDebug() << "Sent APRS login";

}

void Aprs::processData()
{
    if (_status!=1) return;
    QString response;

    while(_socket->bytesAvailable()>0)
    {
        QByteArray a = _socket->read(1024);
        response.append(a);
    }
    qDebug() << response;
    if(response.contains("verified"))
    {
        _authenticated=1;
        return;
    }
    if(response.startsWith("#"))
    {
        //discard comment
        return;
    }
    else
    {
        //we have a position report

    }

}

void Aprs::queryall(QPointF &pos)
{
    if ((_status!=1)) return;
    QString query = "?APRS?";
    //QString query = "filter r/"+QString::number(pos.ry())+"/"+QString::number(pos.rx())+"/0200\r\n";
    qDebug() << query;
    _socket->write(query.toLatin1());
    _socket->flush();

}
