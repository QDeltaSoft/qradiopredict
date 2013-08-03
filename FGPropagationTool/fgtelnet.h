#ifndef FGTELNET_H
#define FGTELNET_H

#include <QTcpSocket>

class FGTelnet : public QObject
{
    Q_OBJECT
public:
    FGTelnet();
    ~FGTelnet();

    void setProperty(QString prop_name, QString value);
    QString getProperty(QString prop_name);
    void runCmd(QString cmd);
    unsigned inline status() {return _status;}

public slots:

    void connectToFGFS();

signals:
    void connectionFailure();
    void connectedToFGFS();

private:
    QTcpSocket _socket;
    unsigned _connection_tries;
    unsigned _status;


private slots:
    void connectionSuccess();
    void connectionFailed(QAbstractSocket::SocketError error);

};

#endif // FGTELNET_H