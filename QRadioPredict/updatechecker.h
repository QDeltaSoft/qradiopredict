#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QStringList>
#include <QFile>

class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    explicit UpdateChecker(QObject *parent = 0);
    ~UpdateChecker();
    void connectToServer();
signals:
    void updateCheckerError();
    void noUpdateAvailable();
    void updateAvailable(QString version);
    
public slots:

private slots:
    void connectionError(QAbstractSocket::SocketError error);
    void readyWrite();
    void processData();

private:
    QTcpSocket *_socket;
    static const quint8 VERSION_MAJOR = 0;
    static const quint8 VERSION_MINOR = 8;
    static const quint8 VERSION_REV = 8;
    
};

#endif // UPDATECHECKER_H
