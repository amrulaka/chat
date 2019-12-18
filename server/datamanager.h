#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QString>
#include <QFile>
#include <QDateTime>

enum MessageType
{

    ReceivedMessage = 0,
    SentMessage = 1,
    NewMessage = 2,
    LogMessage = 3
};

//Singleton
class DataManager
{
    static DataManager* m_dataManager;

private:
    DataManager() {}
    ~DataManager() {}

public:
    static DataManager* Instance()
    {
        if (!m_dataManager)
            m_dataManager = new DataManager();

        return m_dataManager;
    }

    static void Kill()
    {
        if (m_dataManager)
            delete m_dataManager;
    }

    bool saveMessage(const QString& UserName, const QString& msg, MessageType type) const;
    bool getMessages(const QString& UserName, QStringList& msgs, MessageType type, const QDateTime& from = QDateTime(),
                     int number = -1) const;
    int MessagesCount(const QString& UserName, MessageType type) const;
    bool deleteMessage(const QString &userName, const QString& DateTime, MessageType type);

private:
    bool appendToFile(QFile& file, const QString& data) const;
    bool getReceivedMessages(const QString& UserName, QStringList& msgs, const QDateTime& from = QDateTime(),
                             int number = -1) const;
    bool getSentMessages(const QString& UserName, QStringList& msgs, const QDateTime& from = QDateTime(),
                         int number = -1) const;
    bool isUnreadMessage(const QString& message) const;
};

#endif  // DATAMANAGER_H
