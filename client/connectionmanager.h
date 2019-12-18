#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QtNetwork>
#include <QObject>

class ConnectionManager : public QObject
{
    Q_OBJECT

private:
    bool m_connected;
    QObject* m_ref;
    int m_refType;
    static ConnectionManager* m_manager;
    QString m_ipAdr;
    quint16 m_port;
    QTcpSocket* m_tcpSocket;
    quint16 m_blockSize;

public:
    static ConnectionManager* Instance()
    {
        if (!m_manager)
        {
            m_manager = new ConnectionManager();
        }
        return m_manager;
    }

    static void kill()
    {
        if (m_manager)
        {
            delete m_manager;
            m_manager = nullptr;
        }
    }

    void connect(QObject*, int, const QString&, quint16);
    void setReference(QObject*, int);
    bool sendData(const QString&) const;

private:
    ConnectionManager();
    explicit ConnectionManager(QHostAddress&, quint16);
    ConnectionManager(const ConnectionManager&) = delete;
    void operator=(const ConnectionManager&) = delete;
    ~ConnectionManager();

     void readMessageCall(const QString&) const;

private slots:
    void handleError(QAbstractSocket::SocketError);
    void dataReceived();
    void connectedToHost();
    void serverDisconnected() const;
};

#endif  // CONNECTIONMANAGER_H
