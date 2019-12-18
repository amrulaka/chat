#include <QStyleFactory>
#include <QMessageBox>
#include <QTableWidget>
#include <QListWidgetItem>
#include <QtNetwork>

#include "connectionmanager.h"
#include "consts.h"
#include "editpasswordwindow.h"
#include "connectionwindow.h"
#include "ui_chatwindow.h"
#include "chatwindow.h"

ChatWindow::ChatWindow(const QString& userName) : QMainWindow(), ui(new Ui::ChatWindow), m_userName(userName)
{
    drawInterface();
    m_sentMessages.clear();
    m_receivedMessages.clear();
    ConnectionManager::Instance()->setReference(this, 2);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
}

ChatWindow::~ChatWindow()
{
    ui->tableWidget->clearContents();
    delete ui;
}

void ChatWindow::closeEvent(QCloseEvent*)
{
    disconnect();
}

void ChatWindow::disconnect()
{
    m_sentMessages.clear();
    m_receivedMessages.clear();
    ConnectionManager::Instance()->kill();
    this->close();
    emit hideWindow();
}

void ChatWindow::chatPage()
{
    ui->tableWidget->clearContents();
    ui->pages->setCurrentWidget(ui->page1);
    ui->messagenonlu->setText("0");
}

void ChatWindow::sendMessage()
{
    QString message = _sendMessage + _tcpSeparator + ui->Destinataire->text() + _tcpSeparator + ui->msg->text();
    if (sendData(message))
    {
        ui->discussion->append(ui->Destinataire->text() + ":  " + ui->msg->text());
        ui->msg->clear();
    }
    else
    {
        // TODO: display why the message wasn't sent
        ui->discussion->append(QString::fromLatin1("Moi: Message non envoyé"));
    }
    ui->msg->setFocus();
}

/**
 * @brief getMessages sends a server request to retrieve stored messages.
 * @param type MessageType: received or sent messages
 * @param since QDateTime specifies the date of the reception of message
 * @param number int the max number of messages to get.
 * Gets all the messsages if it's set to -1
 */
void ChatWindow::getMessages(MessageType type, QDateTime since, int number)
{
    QString message;
    if (type == receivedMessage)
        message = _ReceivedMessages;
    else if (type == sentMessage)
        message = _SentMessages;

    message += _tcpSeparator + since.toString(Qt::ISODate) + _tcpSeparator + QString::number(number);
    sendData(message);
}

void ChatWindow::ShowMessages(MessageType type)
{
    QDateTime since;

    switch (type)
    {
        case receivedMessage:
            if (!m_receivedMessages.isEmpty())
                since = m_receivedMessages.lastKey();
            break;

        case sentMessage:
            if (!m_sentMessages.isEmpty())
                since = m_sentMessages.lastKey();
            break;

        default:
            break;
    }

    getMessages(type, since, -1);
    ui->page1->hide();
    ui->pages->setCurrentWidget(ui->page2);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setEnabled(true);
    ui->copier->setEnabled(true);
    ui->supprimer->setEnabled(true);
    // TODO: Show a loading bar
}

void ChatWindow::ClearMessageTable()
{
    while (ui->tableWidget->rowCount() != 0)
        ui->tableWidget->removeRow(0);
}

void ChatWindow::FillMessageTable(bool receivedMessages)
{
    ClearMessageTable();
    QMap<QDateTime, QString>* table = nullptr;

    if (receivedMessages)
    {
        ui->tableWidget->horizontalHeaderItem(2)->setText("Source");
        table = &m_receivedMessages;
    }
    else
    {
        ui->tableWidget->horizontalHeaderItem(2)->setText("Destination");
        table = &m_sentMessages;
    }

    int rowCount = ui->tableWidget->rowCount();
    QMap<QDateTime, QString>::iterator it;

    for (it = table->begin(); it != table->end(); ++it)
    {
        qDebug() << it.value();
        QStringList data_list = it.value().split(_textSeparator);
        QTableWidgetItem* item1 = new QTableWidgetItem(it.key().date().toString("dd/MM/yyyy"));
        QTableWidgetItem* item2 = new QTableWidgetItem(it.key().time().toString("hh:mm:ss"));
        QTableWidgetItem* item3 = new QTableWidgetItem(data_list[0]);
        QTableWidgetItem* item4 = new QTableWidgetItem(data_list[1]);

        ui->tableWidget->insertRow(rowCount);
        ui->tableWidget->setItem(rowCount, 0, item1);
        ui->tableWidget->setItem(rowCount, 1, item2);
        ui->tableWidget->setItem(rowCount, 2, item3);
        ui->tableWidget->setItem(rowCount, 3, item4);
        rowCount++;
    }
}

void ChatWindow::readData(const QString& receivdMessage)
{
    qDebug() << "readData = " + receivdMessage;
    if (receivdMessage.isEmpty())
        return;

    QStringList result = receivdMessage.split(_tcpSeparator);
    if (result[0] != _serverToken)
        return;

    if (result[1] == _addUser)
    {
        for (int i = 2; i < result.size(); i++)
        {
            if (!result[i].isEmpty())
            {
                QListWidgetItem* item = new QListWidgetItem(QPixmap(":/img/user.png"), result[i]);
                ui->listWidget->addItem(item);
            }
        }
    }
    else if (result[1] == _rmUser)
    {
        for (int i = 2; i < result.size(); i++)
        {
            QList<QListWidgetItem*> childs = ui->listWidget->findItems(result[i], Qt::MatchExactly);
            if (!childs.isEmpty())
            {
                Q_ASSERT(childs.size() == 1);
                ui->listWidget->removeItemWidget(childs[0]);
                delete childs[0];
            }
        }
    }
    else if (result[1] == _deleteMessage)
    {
        if (! ui->page2->isVisible())
            return;
        if (result[2] == _serverOk)
            ui->tableWidget->removeRow(ui->tableWidget->row(ui->tableWidget->selectedItems()[0]));

        ui->tableWidget->setEnabled(true);
        ui->copier->setEnabled(true);
        ui->supprimer->setEnabled(true);

    }
    else if (result[1] == _instantMessage)
    {
        ui->discussion->append(">" + result[2] + ": " + result[3] + "\n");
    }
    else if (result[1] == _SentMessages)
    {
        for (int i = 2; i < result.size(); i++)
        {
            QStringList message = result[i].split(_textSeparator);
            QDateTime key = QDateTime::fromString(message[0], Qt::ISODate);
            QString value = message[1] + _textSeparator + message[2];
            if (key.isValid())
                m_sentMessages.insert(key, value);
            // stop the loading logo
        }
        FillMessageTable(false);
    }
    else if (result[1] == _ReceivedMessages)
    {
        for (int i = 2; i < result.size(); i++)
        {
            QStringList message = result[i].split(_textSeparator);
            // key is invalid
            QDateTime key = QDateTime::fromString(message[0], Qt::ISODate);
            QString value = message[1] + _textSeparator + message[2];
            if (key.isValid())
                m_receivedMessages.insert(key, value);
        }
        FillMessageTable(true);
    }
    else if (result[1] == _newMessage)
    {
        QPalette p;
        p.setColor(QPalette::Foreground, QColor::fromRgb(10, 01, 250, 255));
        ui->messagenonlu->setPalette(p);
        ui->messagenonlu->setText(result[2]);
    }

}

bool ChatWindow::sendData(const QString& message) const
{
    return ConnectionManager::Instance()->sendData(message);
}

/**
 * @brief changeDestination(QListWidgetItem *item)
 * Used to change the destination of the message whenever the user selects a contact from the connected people.
 * @param QListWidgetItem *item
 */

void ChatWindow::changeDestination(QListWidgetItem *item)
{
    qDebug()<< "change Destination: ";
    if (! item)
        return;
    ui->Destinataire->setText(item->text());
}

 void ChatWindow::reuseMessage()
{
     QList<QTableWidgetItem *> selectedItem = ui->tableWidget->selectedItems();
     if (selectedItem.size() < 4)
         return;

    ui->Destinataire->setText(selectedItem.at(2)->text());
    ui->msg->setText(selectedItem.at(3)->text());
    ui->pages->setCurrentWidget(ui->page1);
}

 void ChatWindow::DeleteMessage()
{
     QList<QTableWidgetItem *> selectedItem = ui->tableWidget->selectedItems();
     if (selectedItem.size() < 4)
         return;

     bool receivedMessage = ui->tableWidget->horizontalHeaderItem(2)->text().compare("Source") == 0;
     QString message = _deleteMessage + _tcpSeparator;

     if (receivedMessage)
         message += "Received";
     else
         message += "Sent";

     message += _tcpSeparator + selectedItem.at(0)->text() + _tcpSeparator + selectedItem.at(1)->text();

     if (sendData(message))
     {
         ui->tableWidget->setEnabled(false);
         ui->supprimer->setEnabled(false);
         ui->copier->setEnabled(false);
     }
}

 void ChatWindow::changePassword()
{
    PasswordWindow *pw = new PasswordWindow(this, m_userName);
    pw->show();
}

void ChatWindow::displayError(const QString& error)
{
    QMessageBox::information(this, "A7kili Client", error);
}

void ChatWindow::drawInterface()
{
    setFixedSize(461, 463);
    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(QImage(":/img/registration_test.jpg")));
    this->setPalette(palette);
    this->setWindowIcon(QIcon(":/img/app_icon.png"));

    // reglages des widgets
    ui->setupUi(this);
    ui->pages->setCurrentWidget(ui->page1);
    ui->discussion->setReadOnly(true);
    ui->date->setText("<FONT COLOR=#ffffff>" + QDate::currentDate().toString("dd.MM.yyyy") + "</FONT>");
    ui->date->setFont(QFont("Comic Sans MS", 18, QFont::Bold, true));

    // afficher le nom de l'utilisateur
    ui->nomutilisateur->setText("<FONT COLOR=#EBDDE2>" + m_userName + "</FONT>");
    ui->nomutilisateur->setFont(QFont("Comic Sans MS", 18, QFont::Bold, true));
    ui->Lnew_messages->setStyleSheet("QLabel { color : white; }");
    ui->Lnew_messages->setAlignment(Qt::AlignRight);
    ui->messagenonlu->setStyleSheet("QLabel { color : white; }");
    ui->messagenonlu->setAlignment(Qt::AlignRight);
    ui->msgLabel->setStyleSheet("QLabel { color : white; }");
    ui->desLabel->setStyleSheet("QLabel { color : white; }");

    // pour l'image de l'utilisateur
//    if (QFile::exists("user.png"))
//       ui->imageutilisateur->setIcon(QIcon("user.png"));
//    else
//        ui->imageutilisateur->setIcon(QIcon(":/img/user_profile.png"));

//    ui->imageutilisateur->setFlat(true);

    //select the entire row when a cell is selected
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui->actionDeconnection, SIGNAL(triggered()), this, SLOT(disconnect()));
    connect(ui->actionChat, SIGNAL(triggered()), this, SLOT(chatPage()));
    connect(ui->actionMessages_recus, SIGNAL(triggered()), this, SLOT(ShowReceivedMessages()));
    connect(ui->actionMessages_envoy_s, SIGNAL(triggered()), this, SLOT(ShowSentMessages()));
    connect(ui->envoyer, SIGNAL(clicked()), this, SLOT(sendMessage()));
    connect(ui->msg, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
    connect(ui->listWidget, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(changeDestination(QListWidgetItem *)));
    connect(ui->actionChanger_mot_de_passe,SIGNAL(triggered()),this,SLOT(changePassword()));
    connect(ui->supprimer,SIGNAL(clicked()),this,SLOT(DeleteMessage()));
    connect(ui->copier, SIGNAL(clicked()), this, SLOT(reuseMessage()));
}
