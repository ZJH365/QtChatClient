#include "clientsession.h"
#include "protocol.h"

ClientSession::ClientSession(QTcpSocket *socket, QObject *parent)
    : QObject(parent)
    , m_socket(socket)
{
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

QTcpSocket *ClientSession::socket() const
{
    return m_socket;
}

QString ClientSession::username() const
{
    return m_username;
}

void ClientSession::setUsername(const QString &name)
{
    m_username = name;
}

bool ClientSession::isLoggedIn() const
{
    return !m_username.isEmpty();
}

void ClientSession::sendJson(const QJsonObject &obj)
{
    m_socket->write(Protocol::buildPacket(obj));
}

void ClientSession::onReadyRead()
{
    m_buffer.append(m_socket->readAll());
    QByteArray payload;
    while (Protocol::tryReadPacket(m_buffer, payload)) {
        bool ok = false;
        const QJsonObject obj = Protocol::decryptJson(payload, &ok);
        if (ok) {
            emit packetReceived(this, obj);
        }
    }
}

void ClientSession::onDisconnected()
{
    emit disconnected(this);
}
