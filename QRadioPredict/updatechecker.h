#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QStringList>

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
    void connectionError();
    void readyWrite();
    void processData();

private:
    QTcpSocket *_socket;
    
};

#endif // UPDATECHECKER_H
