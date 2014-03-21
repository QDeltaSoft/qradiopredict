#include "updatechecker.h"

UpdateChecker::UpdateChecker(QObject *parent) :
    QObject(parent)
{
    _socket = new QTcpSocket;
    QObject::connect(_socket,SIGNAL(error(QAbstractSocket::SocketError)),
                     this,SLOT(connectionError(QAbstractSocket::SocketError)));
    QObject::connect(_socket,SIGNAL(connected()),this,
                     SLOT(readyWrite()));
    QObject::connect(_socket,SIGNAL(readyRead()),this,SLOT(processData()));

}


UpdateChecker::~UpdateChecker()
{
    delete _socket;
}

void UpdateChecker::connectionError(QAbstractSocket::SocketError error)
{
    if(error == QAbstractSocket::RemoteHostClosedError)
        return;
    qDebug() << "Error checking for updates." << error;
    emit updateCheckerError();
}

void UpdateChecker::connectToServer()
{
    _socket->connectToHost("qradiopredict.sourceforge.net", 80);
}

void UpdateChecker::readyWrite()
{
    _socket->write("GET /version HTTP/1.0\nHost: qradiopredict.sourceforge.net\n\n");
    _socket->flush();
}

void UpdateChecker::processData()
{

    QByteArray buf;
    buf.append(_socket->readAll());

    bool endOfLine = false;

    while ((!endOfLine))
    {

        char ch;
        if(_socket->bytesAvailable()>0)
        {
            int bytesRead = _socket->read(&ch, sizeof(ch));
            if (bytesRead == sizeof(ch))
            {
                //cnt++;
                buf.append( ch );
                if(_socket->bytesAvailable()==0)
                {
                    endOfLine = true;

                }

            }
        }
        else
        {
            break;
        }


    }
    QString message(buf);
    QStringList parts = message.split("###",QString::SkipEmptyParts);
    if(parts.length() < 2)
    {
        qDebug() << "No version file found";
        emit noUpdateAvailable();
    }
    QString version= parts[1];
    qDebug() << version;
    _socket->close();
    QStringList tokens = version.split(".",QString::SkipEmptyParts);
    quint8 major_v = tokens[0].toInt();
    quint8 minor_v = tokens[1].toInt();
    quint8 rev_v = tokens[2].toInt();
    if(major_v > VERSION_MAJOR)
    {
        emit updateAvailable(version);
        return;
    }
    if(minor_v > VERSION_MINOR)
    {
        emit updateAvailable(version);
        return;
    }
    if(rev_v > VERSION_REV)
    {
        emit updateAvailable(version);
        return;
    }
    emit noUpdateAvailable();

}
