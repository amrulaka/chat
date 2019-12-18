#ifndef FENSERVEUR_H
#define FENSERVEUR_H

//#include <QtNetwork>
#include <QMainWindow>
#include <QFile>
#include <QMap>

class QTcpServer;
class QTcpSocket;

namespace Ui
{
class MainWindow;
}

enum class UserCommand
{

    error,
    newAccount,
    connectRequest,
    sendRequest,
    disconnectRequest,
    receivedMessagesList,
    sentMessagesList,
    deleteMessage,
    other
};

class ServerWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow* ui;

    int m_nbConnectedClients;
    int m_nbTCPConnections;
    QTcpServer* m_serveur;
    QList<QTcpSocket*> m_clients;
    QMap<QTcpSocket*, QString> m_users;
    quint16 m_messageSize;

public:
    ServerWindow(QWidget* parent = nullptr);
    ~ServerWindow();

private:
    void DrawInterface();
    // TCP methods
    void NotifyOthers(const QString& UserName, bool newConnection);
    void sendUsersList(QTcpSocket* NewUser) const;
    void sendData(const QString& message, QTcpSocket* client) const;
    void sendToAll(const QString& message) const;
    void subscribe(const QString msg, QTcpSocket* userSocket);
    void connectUser(const QString msg, QTcpSocket* userSocket);
    void handleMessage(const QString msg, QTcpSocket* userSocket);
    void sendSavedMessages(const QString msg, QTcpSocket* userSocket, UserCommand);
    UserCommand getCommand(const QString txt) const;

    // File & Log methods
    bool appendToFile(QFile&, const QString data) const;
    bool checkCredentials(const QString text) const;
    void deleteMessage(const QString& msg, QTcpSocket *userSocket);
    bool findUser(const QString userLogin) const;
    void log(const QString) const;
    int newMessagesCount(QString& userName);

private slots:
    void nouvelleConnexion();
    void donneesRecues();
    void deconnexionClient();
    void DumpConnectedClients();
};

#endif  // FENSERVEUR_H
