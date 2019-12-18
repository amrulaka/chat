#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class QTableWidget;
class QTableWidgetItem;
class QDateTime;
class QString;
class QListWidgetItem;

enum MessageType
{
    receivedMessage,
    sentMessage
};

namespace Ui
{
class ChatWindow;
}

class ChatWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::ChatWindow* ui;
    QString m_userName;
    // used due to the async communication between client and server
    QMap<QDateTime, QString> m_sentMessages;
    QMap<QDateTime, QString> m_receivedMessages;

    void drawInterface();
    void FillMessageTable(bool receivedMessages);
    void ClearMessageTable();
    bool sendData(const QString& message) const;
    void getMessages(MessageType type, QDateTime since, int number);

public:
    explicit ChatWindow(const QString& userName);
    ChatWindow(const ChatWindow&) = delete;
    ChatWindow& operator=(const ChatWindow&) = delete;
    ~ChatWindow();

    void setName(const QString& name) { m_userName = name; }
    void closeEvent(QCloseEvent* event) override;

    // Callback used by the Connection Manager
    void readData(const QString& receivdMessage);
    void displayError(const QString&);

private slots:
    void disconnect();
    void ShowReceivedMessages() { ShowMessages(MessageType::receivedMessage); }
    void ShowSentMessages() { ShowMessages(MessageType::sentMessage); }
    void ShowMessages(MessageType);
    void sendMessage();
    void chatPage();
    void changeDestination(QListWidgetItem * item);
    void reuseMessage();
    void DeleteMessage();
    void changePassword();

signals:
    void hideWindow();
};

#endif  // MAINWINDOW_H
