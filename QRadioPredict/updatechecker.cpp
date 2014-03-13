#include "updatechecker.h"

UpdateChecker::UpdateChecker(QObject *parent) :
    QObject(parent)
{
    _socket = new QTcpSocket;
    QObject::connect(_socket,SIGNAL(error(QAbstractSocket::SocketError)),
                     this,SLOT(connectionError()));
    QObject::connect(_socket,SIGNAL(connected()),this,
                     SLOT(readyWrite()));
}


UpdateChecker::~UpdateChecker()
{
    delete _socket;
}

void UpdateChecker::connectionError()
{

}

void UpdateChecker::readyWrite()
{

}
