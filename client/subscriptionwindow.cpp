#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QCoreApplication>
#include <QByteArray>
#include <QBitArray>
#include <QString>
#include <QDebug>




#include "consts.h"
#include "subscriptionwindow.h"
#include "connectionmanager.h"


//TODO: Create a string list of the different error messages
// And managing the user locale
SubscriptionWindow::SubscriptionWindow(QWidget* parent) : QWidget(parent)
{
    drawInterface();
}

void SubscriptionWindow::closeWindow()
{
    this->close();
}

void SubscriptionWindow::sendData(const QString& message) const
{
    // TODO: display an error message in case the message wasn't sent
    ConnectionManager::Instance()->sendData(message);
}

void SubscriptionWindow::readData(const QString& receivdMessage)
{
    qDebug() << "readData = " + receivdMessage;
    if (receivdMessage.isEmpty())
    {
        m_valide_PB->setEnabled(true);
        return;
    }

    QStringList result = receivdMessage.split(_tcpSeparator);
    if (result[0] == _serverToken)
    {
        if (result[1] == _newAccount && result[2] == _serverOk)
        {
            QMessageBox::information(this, "info", QString::fromUtf8("Pendaftaran Berhasil!"));
            m_name_LE->clear();
            m_password_LE->clear();
            m_valide_PB->setEnabled(true);
            closeWindow();
        }
        else
        {
            QMessageBox::information(this, "Error", QString::fromLatin1("Pendaftaran Gagal!"));
            m_valide_PB->setEnabled(true);
        }
    }
    qDebug() << "/readData";
}

void SubscriptionWindow::newAccountRequest()
{
    qDebug() << "newAccount";
    if (m_name_LE->text().isEmpty() || m_password_LE->text().isEmpty())
        QMessageBox::critical(this, "Error", "Masukan Username dan Password Dengan Benar");

    else
    {
        ConnectionManager::Instance()->connect(this, 0, "127.0.0.1", 9090);
        QByteArray string;
        string.append(m_password_LE->text());
        QCryptographicHash hasher(QCryptographicHash::Sha1);
        hasher.addData(string);
        QString pwd = hasher.result().toHex();
        QString msg = _newAccount + _tcpSeparator + m_name_LE->text() + _tcpSeparator + pwd;

        if (ConnectionManager::Instance()->sendData(msg))
            m_valide_PB->setEnabled(false);
    }
    qDebug() << "/newAccount";
}

void SubscriptionWindow::displayError(const QString& error)
{
    QMessageBox::information(this, "A7kili Client", error);
    m_valide_PB->setEnabled(true);
}

void SubscriptionWindow::drawInterface()
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
//    m_userlogo_PB = new QPushButton(this);
//    m_userlogo_PB->setText("LOGIN");
//    m_userlogo_PB->setStyleSheet(" color : white; ");
//    m_userlogo_PB->setIconSize(QSize(140, 110));
//    m_userlogo_PB->setGeometry(57, 1, 141, 111);
//    m_userlogo_PB->setFlat(true);

    QLabel* login_label = new QLabel("username :", this);
    login_label->setStyleSheet("QLabel { color : white; }");
    login_label->setGeometry(70, 30, 100, 29);
    m_name_LE = new QLineEdit(this);
    m_name_LE->setGeometry(70, 30, 130, 20);




    QLabel* password_label = new QLabel("Password :", this);
    QString base64_encode(QLabel string);
    password_label->setStyleSheet("QLabel { color : white; }");
    password_label->setGeometry(70, 60, 100, 20);
    m_password_LE = new QLineEdit(this);
//    const QString text= m_password_LE;
    m_password_LE->setEchoMode(QLineEdit::Password);

    QCryptographicHash hasher(QCryptographicHash::Sha1);
//    hasher.addData(m_password_LE);

    m_password_LE->setGeometry(70, 80, 130, 20);



    m_valide_PB = new QPushButton("Submit", this);
    m_valide_PB->setGeometry(63, 105, 70, 25);

    m_annuler_PB = new QPushButton("Back", this);
    m_annuler_PB->setGeometry(135, 105, 70, 25);

    QObject::connect(m_annuler_PB, SIGNAL(clicked()), this, SLOT(closeWindow()));
    QObject::connect(m_valide_PB, SIGNAL(clicked()), this, SLOT(newAccountRequest()));
}
