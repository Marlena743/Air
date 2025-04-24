// src/api/networkmanager.h
#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();
    
    bool isNetworkAvailable();
    void checkNetworkConnection();
    
signals:
    void networkStatusChanged(bool available);
    
private slots:
    void onNetworkCheckResult();
    
private:
    QTcpSocket *socket;
    QTimer *checkTimer;
    bool networkAvailable;
};

#endif // NETWORKMANAGER_H