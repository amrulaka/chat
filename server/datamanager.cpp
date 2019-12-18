#include <QDir>
#include <QTextStream>
#include <QBuffer>
#include "consts.h"
#include "datamanager.h"

DataManager* DataManager::m_dataManager = nullptr;
const static QString sentMessagesFile = "Sent.txt";
const static QString receivedMessagesFile = "Received.txt";
const static QString LogFile = "Log.txt";

bool DataManager::saveMessage(const QString& UserName, const QString& msg, MessageType type) const
{
    bool res = false;
    QDir dir(".");
    QString fileName;

    switch (type)
    {
        case ReceivedMessage:
            fileName = receivedMessagesFile;
            break;
        case SentMessage:
            fileName = sentMessagesFile;
            break;
        case LogMessage:
            fileName = LogFile;
        default:
            break;
    }

    QFile file(fileName);
    if (!dir.exists("./" + UserName))
    {
        dir.mkdir(UserName);
    }

    QDir::setCurrent("./" + UserName);
    res = appendToFile(file, msg);
    QDir::setCurrent("..");
    return res;
}

bool DataManager::getMessages(const QString& UserName, QStringList& msgs, MessageType type, const QDateTime& from,
                              int number) const
{
    switch (type)
    {
        case ReceivedMessage:
            return getReceivedMessages(UserName, msgs, from, number);
            break;
        case SentMessage:
            return getSentMessages(UserName, msgs, from, number);
            break;
        default:
            break;
    }
    return false;
}

// TODO: avoid looking throw all the messages(with database it would be easier)
int DataManager::MessagesCount(const QString& UserName, MessageType type) const
{
    QDir dir(".");
    QFile file(receivedMessagesFile);

    if (!dir.exists("./" + UserName))
    {
        return -1;
    }
    QDir::setCurrent("./" + UserName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QDir::setCurrent("..");
        return -1;
    }

    int messagescount = 0;

    while ((!file.atEnd()))
    {
        QString line = QString::fromLatin1(file.readLine());
        line.remove(line.size() - 1, 1);  // remove the \n
        QStringList stringlist = line.split(_textSeparator);
        if (type == NewMessage)
        {
            if (stringlist.size() == 4)
            {
                if (stringlist[3] == "1")
                    messagescount++;
            }
        }
        else if (type == ReceivedMessage)
            messagescount++;
    }

    file.close();
    QDir::setCurrent("..");
    return messagescount;
}

//TODO: finish the implementation
bool DataManager::deleteMessage(const QString& userName, const QString& dateTime, MessageType type)
{
    //On stocke dans une hashmap la date et l'heure
    //et on supprime la ligne qui convient

    QDir dir(".");
    QString fileName;

    switch (type)
    {
        case ReceivedMessage:
            fileName = receivedMessagesFile;
            break;
        case SentMessage:
            fileName = sentMessagesFile;
            break;
    }

    QFile file(fileName);

    if (!dir.exists("./" + userName))
        return false;

    QDir::setCurrent("./" + userName);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QDir::setCurrent("..");
        return false;
    }

    std::map<QString, QString> receivedMessages;

    while ((!file.atEnd()))
    {
        QString line = QString::fromLatin1(file.readLine());

        QString key = line.split(_textSeparator).at(0);
        receivedMessages[key] = line;
    }

    if (receivedMessages[dateTime] != nullptr){
        receivedMessages.erase(dateTime);

        file.resize(0);
        QTextStream flux(&file);
        for (auto it = receivedMessages.begin(); it != receivedMessages.end(); ++it){
            flux << it->second;
        }
    }

    file.close();
    QDir::setCurrent("..");
    return true;
}

bool DataManager::appendToFile(QFile& file, const QString& data) const
{
    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream flux(&file);
        flux << data;
        file.close();
        return true;
    }
    else
        return false;
}

bool DataManager::getReceivedMessages(const QString& UserName, QStringList& msgs, const QDateTime& from,
                                      int number) const
{
    QDir dir(".");
    QFile file(receivedMessagesFile);

    if (!dir.exists("./" + UserName))
        return false;

    QDir::setCurrent("./" + UserName);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QDir::setCurrent("..");
        return false;
    }

    bool infinity = (number < 0);
    bool validDate = from.isValid();
    bool maxcount = false;
    QBuffer buff;

    buff.open(QBuffer::WriteOnly);
    while ((!file.atEnd()) && !maxcount)
    {
        QString line = QString::fromLatin1(file.readLine());
        if (validDate)
        {
            QDateTime datetime = QDateTime::fromString(line.split(_textSeparator).at(0), Qt::ISODate);
            if (datetime.date() >= from.date())
                msgs.append(line);
            // else stop searching
        }
        else
            msgs.append(line);

        if (isUnreadMessage(line))
            line.remove(line.size() - (2 + _textSeparator.size()), 1 + _textSeparator.size());
        buff.write(line.toLatin1().data());

        if (!infinity)
        {
            number--;
            if (number == 0)
                maxcount = true;
        }
    }

    file.resize(0);
    file.write(buff.data());
    file.close();

    QDir::setCurrent("..");
    return true;
}

bool DataManager::getSentMessages(const QString& UserName, QStringList& msgs, const QDateTime& from, int number) const
{
    QDir dir(".");
    QFile file(sentMessagesFile);

    if (!dir.exists("./" + UserName))
        return false;

    QDir::setCurrent("./" + UserName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QDir::setCurrent("..");
        return false;
    }

    bool infinity = (number < 0);
    bool validDate = from.isValid();
    bool maxcount = false;

    while ((!file.atEnd()) && !maxcount)
    {
        QString line = QString::fromLatin1(file.readLine());
        if (validDate)
        {
            QDateTime datetime = QDateTime::fromString(line.split(_textSeparator).at(0), Qt::ISODate);
            if (datetime.date() >= from.date())
                msgs.append(line);
            // else stop searching
        }
        else
            msgs.append(line);

        if (!infinity)
        {
            number--;
            if (number == 0)
                maxcount = true;
        }
    }
    file.close();

    QDir::setCurrent("..");
    return true;
}

bool DataManager::isUnreadMessage(const QString& message) const
{
    QStringList stringlist = message.split(_textSeparator);
    return (stringlist.size() == 4 && stringlist[3] == "1\n");
}
