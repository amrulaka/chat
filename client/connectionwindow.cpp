#include <QLabel>
#include <QMessageBox>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>

#include "consts.h"
#include "connectionwindow.h"
#include "subscriptionwindow.h"
#include "connectionmanager.h"
#include "chatwindow.h"

ConnectionWindow::ConnectionWindow() : QWidget()
{
    m_subsWindow = new SubscriptionWindow();
    drawInterface();
}

ConnectionWindow::~ConnectionWindow()
{
    if (m_subsWindow)
        delete m_subsWindow;

    ConnectionManager::Instance()->kill();
}

void ConnectionWindow::connection()
{
    QByteArray string;
    string.append(m_password_LE->text());
    QCryptographicHash hasher(QCryptographicHash::Sha1);
    hasher.addData(string);
    QString pwd = hasher.result().toHex();
    if (m_login_LE->text().isEmpty() || pwd.isEmpty())
    {
        QMessageBox::critical(this, "Error", "Username atau password salah");
    }
    else
    {
        ConnectionManager::Instance()->connect(this, 1, "127.0.0.1", 9090);

        QString msg = _connect + _tcpSeparator + m_login_LE->text() + _tcpSeparator + pwd;

        if (ConnectionManager::Instance()->sendData(msg))
            m_connect_PB->setEnabled(false);
    }
}

void ConnectionWindow::subscription()
{
    if (!m_subsWindow)
        return;

    m_subsWindow->show();
}

void ConnectionWindow::readData(const QString& receivdMessage)
{
    if (receivdMessage.isEmpty())
    {
        m_connect_PB->setEnabled(true);
        return;
    }

    QStringList result = receivdMessage.split(_tcpSeparator);

    if (result[0] != _serverToken)
        return;

    if (result[1] == _connect && result[2] == _serverOk)
    {
        // chatWindow is automatically delated after it is closed due to the WA_DeleteOnClose attribute
        ChatWindow* cWindow = new ChatWindow(m_login_LE->text());
        cWindow->show();
        connect(cWindow, SIGNAL(hideWindow()), this, SLOT(displayWindow()));
        this->hide();
    }
    else
    {
        QMessageBox::information(this, "Error", result[2]);
        m_connect_PB->setEnabled(true);
    }
}

void ConnectionWindow::displayError(const QString& error)
{
    QMessageBox::information(this, "A7kili Client", error);
    m_connect_PB->setEnabled(true);
}

void ConnectionWindow::displayWindow()
{
    m_login_LE->clear();
    m_password_LE->clear();
    m_connect_PB->setEnabled(true);
    this->show();
}

void ConnectionWindow::drawInterface()
{
    setWindowTitle("Chat Server");
    this->setWindowIcon(QIcon(":/img/app_icon.png"));
    setFixedSize(260, 280);
    setGeometry(600, 200, 100, 385);

    // background image
    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(QImage(":/img/registration_test.jpg")));
    this->setPalette(palette);

    // userLogo
    m_userlogo_PB = new QPushButton(this);
    m_userlogo_PB->setText("LOGIN");
    m_userlogo_PB->setStyleSheet(" color : white; ");
    m_userlogo_PB->setIconSize(QSize(140, 110));
    m_userlogo_PB->setGeometry(57, 1, 141, 111);
    m_userlogo_PB->setFlat(true);

    // Username
    QLabel* user = new QLabel("Username :", this);
    user->setStyleSheet("QLabel { color : white; }");
    user->setGeometry(1, 120, 255, 23);
    m_login_LE = new QLineEdit(this);
    m_login_LE->setGeometry(1, 145, 255, 20);
    m_login_LE->setFocus();

    // Password
    QLabel* pass = new QLabel("Password :", this);
    QString base64_encode(QLabel string);
    pass->setGeometry(1, 170, 255, 23);
    pass->setStyleSheet("QLabel { color : white; }");
    m_password_LE = new QLineEdit(this);
    m_password_LE->setGeometry(1, 195, 255, 20);
    m_password_LE->setEchoMode(QLineEdit::Password);

    // Login
    m_connect_PB = new QPushButton("Login", this);
    m_connect_PB->setGeometry(60, 230, 75, 30);


    // Register
    m_inscription_PB = new QPushButton("sign up", this);
    m_inscription_PB->setGeometry(130, 230, 75, 30);


    // connexions
    QObject::connect(m_connect_PB, SIGNAL(clicked()), this, SLOT(connection()));  // verifier!!
    QObject::connect(m_inscription_PB, SIGNAL(clicked()), this, SLOT(subscription()));
    QObject::connect(m_password_LE, SIGNAL(returnPressed()), this, SLOT(connection()));
}
