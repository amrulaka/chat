#ifndef CONSTS
#define CONSTS

#include <QString>

// Server
//=======Network========
const QString _serverToken("[Server]");
const QString _instantMessage("instantMessage");
const QString _newMessage("newMessage");
const QString _savedMessage("savedMessage");
const QString _serverOk("ok");
const QString _serverError("Error");
// server commands
const QString _usersConnected("addUser");
const QString _usersDIsconnected("rmUser");

// Client
//=======Network========
const QString _clientToken("[Client]");
const QString _connect("Connect");
const QString _newAccount("Register");
const QString _sendMessage("SendMessage");
const QString _ReceivedMessages("ReceivedMessages");
const QString _SentMessages("SentMessages");
const QString _deleteMessage("deleteMessage");

const QString _tcpSeparator("<TCPSeparator>");
//=======Data========
const QString _textSeparator("<DataSeparator>");
const QString _newLine("\n");

const QString _DateTimeFormat("dd/MM/yyyy hh:mm:ss");

#endif  // CONSTS
