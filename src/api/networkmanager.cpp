// src/api/networkmanager.cpp
#include "networkmanager.h"

NetworkManager::NetworkManager(QObject *parent) : QObject(parent), networkAvailable(false)
{
    socket = new QTcpSocket(this);
    checkTimer = new QTimer(this);
    
    connect(socket, &QTcpSocket::connected, this, [this]() {
        socket->abort();
        if (!networkAvailable) {
            networkAvailable = true;
            emit networkStatusChanged(true);
        }
    });
    
    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        if (networkAvailable) {
            networkAvailable = false;
            emit networkStatusChanged(false);
        }
    });
    
    connect(checkTimer, &QTimer::timeout, this, &NetworkManager::checkNetworkConnection);
    
    // Sprawdzaj co 30 sekund
    checkTimer->start(30000);
    
    // Wykonaj pierwsze sprawdzenie
    checkNetworkConnection();
}

NetworkManager::~NetworkManager()
{
    checkTimer->stop();
}

bool NetworkManager::isNetworkAvailable()
{
    return networkAvailable;
}

void NetworkManager::checkNetworkConnection()
{
    socket->abort();
    socket->connectToHost("api.gios.gov.pl", 80);
}

void NetworkManager::onNetworkCheckResult()
{
    bool newStatus = (socket->state() == QAbstractSocket::ConnectedState);
    if (newStatus != networkAvailable) {
        networkAvailable = newStatus;
        emit networkStatusChanged(networkAvailable);
    }
}