#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H

#include <QWidget>
#include <QAbstractSocket>

class QPushButton;
class QLineEdit;
class QTcpSocket;
class SubscriptionWindow;
class ChatWindow;

class ConnectionWindow : public QWidget
{
    Q_OBJECT
public:
    ConnectionWindow();
    ConnectionWindow(const ConnectionWindow&) = delete;
    ConnectionWindow& operator=(const ConnectionWindow&) = delete;
    ~ConnectionWindow();

    void readData(const QString& receivdMessage);
    void displayError(const QString& error);

public slots:
    void displayWindow();

private slots:
    void connection();
    void subscription();

private:
    QPushButton* m_quitter_PB;
    QPushButton* m_connect_PB;
    QPushButton* m_inscription_PB;
    QPushButton* m_configuration_PB;
    QPushButton* m_userlogo_PB;
    QLineEdit* m_login_LE;
    QLineEdit* m_password_LE;
    QTcpSocket* m_socket;
    SubscriptionWindow* m_subsWindow;

    void drawInterface();
};
#endif  // FENETRE1_H
