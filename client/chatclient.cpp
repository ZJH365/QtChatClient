#include "chatclient.h"
#include "protocol.h"

#include <QJsonArray>
#include <QJsonObject>

ChatClient::ChatClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_socket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(&m_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(&m_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(&m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
            SLOT(onSocketError(QAbstractSocket::SocketError)));
}

void ChatClient::connectToServer(const QString &host, quint16 port)
{
    m_socket.connectToHost(host, port);
}

void ChatClient::login(const QString &username, const QString &password)
{
    QJsonObject req;
    req["type"] = "login";
    req["username"] = username;
    req["password"] = password;
    m_socket.write(Protocol::buildPacket(req));
}

void ChatClient::sendChat(const QString &text)
{
    QJsonObject req;
    req["type"] = "chat";
    req["text"] = text;
    m_socket.write(Protocol::buildPacket(req));
}

void ChatClient::onReadyRead()
{
    m_buffer.append(m_socket.readAll());
    QByteArray payload;
    while (Protocol::tryReadPacket(m_buffer, payload)) {
        bool ok = false;
        const QJsonObject obj = Protocol::decryptJson(payload, &ok);
        if (!ok) {
            continue;
        }

        const QString type = obj.value("type").toString();
        if (type == "login_result") {
            emit loginResult(obj.value("ok").toBool(),
                             obj.value("reason").toString(),
                             obj.value("history").toArray());
        } else if (type == "chat_broadcast") {
            emit messageReceived(obj.value("sender").toString(),
                                 obj.value("text").toString(),
                                 obj.value("timestamp").toString());
        }
    }
}

void ChatClient::onSocketError(QAbstractSocket::SocketError)
{
    emit error(m_socket.errorString());
}
