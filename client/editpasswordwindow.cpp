#include <QLabel>
#include <QMessageBox>
#include <QTextStream>
#include <QLineEdit>
#include <QPushButton>

#include "editpasswordwindow.h"

PasswordWindow::PasswordWindow(QWidget* parent, const QString& userName) : QWidget()
{
    drawInterface(userName);
    if (parent)
        this->move(parent->window()->frameGeometry().topLeft() + parent->window()->rect().center() - rect().center());
}

//TODO: changePassword()
//void PasswordWindow::changePassword()
//{
//    if ((m_oldPwd->text().isEmpty()) || (m_newPwd->text().isEmpty()))
//        QMessageBox::critical(this, "Erreur", "Le champ <strong>ancien mot de passe</strong><br />ou le "
//                                              "champ<strong>nouveau mot de passe</strong><br />est vide!!!");

//}

void PasswordWindow::drawInterface(const QString& userName)
{
    setWindowTitle("Ganti Password");
    this->setWindowIcon(QIcon(":/img/tchat.jpg"));
    setFixedSize(258, 130);
    setGeometry(600, 200, 258, 130);

    m_name = userName;
    QLabel* l2 = new QLabel("Password Sekarang:", this);
    l2->setGeometry(70, 30, 150, 19);

    m_oldPwd = new QLineEdit(this);
    m_oldPwd->setGeometry(70, 50, 113, 20);

    QLabel* l3 = new QLabel("Password Baru:", this);
    l3->setGeometry(70, 70, 200, 20);

    m_newPwd = new QLineEdit(this);
    m_newPwd->setEchoMode(QLineEdit::Password);
    m_newPwd->setGeometry(70, 90, 113, 20);

    QPushButton* applyPB = new QPushButton("Submit", this);
    applyPB->setGeometry(210, 100, 46, 23);

    QPushButton* cancelPB = new QPushButton("Back", this);
    cancelPB->setGeometry(10, 100, 46, 23);

    QObject::connect(cancelPB, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(applyPB, SIGNAL(clicked()), this, SLOT(changePassword()));
}
