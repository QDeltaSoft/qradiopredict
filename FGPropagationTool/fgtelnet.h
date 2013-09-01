#ifndef FGTELNET_H
#define FGTELNET_H

#include <QTcpSocket>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QCoreApplication>
#include <QLatin1String>

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
