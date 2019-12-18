#include <iostream>
#include "connectionmanager.h"
#include "subscriptionwindow.h"
#include "connectionwindow.h"
#include "chatwindow.h"

ConnectionManager* ConnectionManager::m_manager = nullptr;
// TODO connected and disconnected signals

ConnectionManager::ConnectionManager()
{
    m_tcpSocket = new QTcpSocket();
    QObject::connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(connectedToHost()));
    QObject::connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(serverDisconnected()));
    QObject::connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));  // Data is received
    QObject::connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this,
                     SLOT(handleError(QAbstractSocket::SocketError)));  // Socket error handler

    m_blockSize = 0;
    m_connected = false;
}

void ConnectionManager::connect(QObject* _ref, int type, const QString& adr, quint16 _port)
{
    m_ref = _ref;
    m_refType = type;
    m_ipAdr = adr;
    m_port = _port;
    m_tcpSocket->abort();
    m_tcpSocket->connectToHost(m_ipAdr, m_port);
    m_tcpSocket->waitForConnected(1000);
}

void ConnectionManager::setReference(QObject* obj, int type)
{
    if (obj && (0 <= type && type < 3))
    {
        m_ref = obj;
        m_refType = type;
    }
}

// TODO: Avoid infinite loop
static void escapeSpaces(QByteArray& data)
{
    int size = data.size();
    int i = 0;
    int j = 0;
    while (1)
    {
        while (j < size && data[j] == '\0')
            j++;

        if (j == size)
        {
            data.resize(i);
            return;
        }

        if (i != j)
            data[i] = data[j];

        if (i < size)
            i++;
        j++;
    }
}

void ConnectionManager::readMessageCall(const QString& receivdMessage) const
{
    switch (m_refType)
    {
        case 0:
        {
            SubscriptionWindow* obj = qobject_cast<SubscriptionWindow*>(m_ref);
            obj->readData(receivdMessage);
            break;
        }

        case 1:
        {
            ConnectionWindow* obj = qobject_cast<ConnectionWindow*>(m_ref);
            obj->readData(receivdMessage);
            break;
        }
        case 2:
        {
            ChatWindow* obj = qobject_cast<ChatWindow*>(m_ref);
            obj->readData(receivdMessage);
            break;
        }
        default:
            break;
    }
}

void ConnectionManager::dataReceived()
{
    QDataStream in(m_tcpSocket);
    // in.setVersion(QDataStream::Qt_4_0);
    if (m_blockSize == 0)
    {
        if (m_tcpSocket->bytesAvailable() < (int)sizeof(quint16))
            return;

        in >> m_blockSize;
    }

    if (m_tcpSocket->bytesAvailable() < m_blockSize)
        return;

    m_blockSize = 0;
    // we can read the data

    QString receivdMessage;
    in >> receivdMessage;
    readMessageCall(receivdMessage);

    qint64 bufferSize = m_tcpSocket->size();
    if (bufferSize != 0)
    {
        int qint16size = sizeof(qint16);
        QByteArray arr = m_tcpSocket->readAll();
        int datasize = arr.at(0) * 256 + arr.at(1);
        escapeSpaces(arr);
        arr.remove(0, qint16size);
        QByteArray data = arr.left(datasize);
        QString message(data);
        readMessageCall(message);
    }
}

bool ConnectionManager::sendData(const QString& message) const
{
    if (!m_tcpSocket || !m_connected)
        return false;

    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);
    // out.setVersion(QDataStream::Qt_4_0);
    // message.prepend("register,");
    out << (quint16)0;
    out << message;
    out.device()->seek(0);
    out << (quint16)(paquet.size() - sizeof(quint16));
    int res = m_tcpSocket->write(paquet);
    return (res > -1);
}

void ConnectionManager::connectedToHost()
{
    m_connected = true;
}

void ConnectionManager::serverDisconnected() const
{
    qDebug() << "Server Dionnected";
}

//TODO: use dynamic_cast to find the object's type
void ConnectionManager::handleError(QAbstractSocket::SocketError socketError)
{
    QString text;
    switch (socketError)
    {
        case QAbstractSocket::RemoteHostClosedError:
            break;

        case QAbstractSocket::HostNotFoundError:
            text = "The host was not found. Please check the host name and port settings.";
            switch (m_refType)
            {
                case 0:
                {
                    SubscriptionWindow* obj = qobject_cast<SubscriptionWindow*>(m_ref);
                    obj->displayError(text);
                    break;
                }

                case 1:
                {
                    ConnectionWindow* obj = qobject_cast<ConnectionWindow*>(m_ref);
                    obj->displayError(text);
                    break;
                }

                case 2:
                {
                    ChatWindow* obj = qobject_cast<ChatWindow*>(m_ref);
                    obj->displayError(text);
                    break;
                }

                default:
                    break;
            }

            break;

        case QAbstractSocket::ConnectionRefusedError:
            text = "The connection was refused by the peer. Make sure the server is running, "
                   "and check that the host name and port "
                   "settings are correct.";
            switch (m_refType)
            {
                case 0:
                {
                    SubscriptionWindow* obj = qobject_cast<SubscriptionWindow*>(m_ref);
                    m_tcpSocket->abort();
                    obj->displayError(text);
                    break;
                }
                case 1:
                {
                    ConnectionWindow* obj = qobject_cast<ConnectionWindow*>(m_ref);
                    obj->displayError(text);
                    break;
                }

                case 2:
                {
                    ChatWindow* obj = qobject_cast<ChatWindow*>(m_ref);
                    obj->displayError(text);
                    break;
                }

                default:
                    break;
            }

            break;

        default:
            text = "The following error occurred: " + m_tcpSocket->errorString() + " .";
            switch (m_refType)
            {
                case 0:
                {
                    SubscriptionWindow* obj = qobject_cast<SubscriptionWindow*>(m_ref);
                    obj->displayError(text);
                    break;
                }
                case 1:
                {
                    ConnectionWindow* obj = qobject_cast<ConnectionWindow*>(m_ref);
                    obj->displayError(text);
                    break;
                }
                case 2:
                {
                    ChatWindow* obj = qobject_cast<ChatWindow*>(m_ref);
                    obj->displayError(text);
                    break;
                }
                default:
                    break;
            }
            break;
    }
}

ConnectionManager::~ConnectionManager()
{
    m_tcpSocket->disconnect();
    delete m_tcpSocket;
}
