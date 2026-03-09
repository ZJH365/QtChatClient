#include "chatserver.h"
#include "clientsession.h"
#include "protocol.h"

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>

ChatServer::ChatServer(QObject *parent)
    : QObject(parent)
{
    connect(&m_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    loadUsers();
    loadHistory();
}

bool ChatServer::start(quint16 port)
{
    if (m_server.listen(QHostAddress::Any, port)) {
        emit logMessage(QString("Server started: %1").arg(port));
        return true;
    }
    emit logMessage(QString("Start failed: %1").arg(m_server.errorString()));
    return false;
}

void ChatServer::stop()
{
    foreach (ClientSession *s, m_sessions) {
        s->socket()->disconnectFromHost();
        s->deleteLater();
    }
    m_sessions.clear();
    m_server.close();
    saveHistory();
    emit logMessage("Server stopped");
}

void ChatServer::onNewConnection()
{
    while (m_server.hasPendingConnections()) {
        QTcpSocket *socket = m_server.nextPendingConnection();
        ClientSession *session = new ClientSession(socket, this);
        m_sessions.append(session);
        connect(session, SIGNAL(packetReceived(ClientSession*,QJsonObject)), this, SLOT(onPacket(ClientSession*,QJsonObject)));
        connect(session, SIGNAL(disconnected(ClientSession*)), this, SLOT(onSessionDisconnected(ClientSession*)));
        emit logMessage(QString("Client connected: %1").arg(socket->peerAddress().toString()));
    }
}

void ChatServer::onPacket(ClientSession *session, const QJsonObject &obj)
{
    const QString type = obj.value("type").toString();
    if (type == "login") {
        const QString user = obj.value("username").toString();
        const QString pass = obj.value("password").toString();
        if (m_users.value(user) == pass) {
            session->setUsername(user);
            sendLoginResult(session, true, "ok");
            emit logMessage(QString("Login success: %1").arg(user));
        } else {
            sendLoginResult(session, false, "用户名或密码错误");
            emit logMessage(QString("Login failed: %1").arg(user));
        }
        return;
    }

    if (!session->isLoggedIn()) {
        return;
    }

    if (type == "chat") {
        QJsonObject chatMsg;
        chatMsg["type"] = "chat_broadcast";
        chatMsg["sender"] = session->username();
        chatMsg["text"] = obj.value("text").toString();
        chatMsg["timestamp"] = Protocol::currentTimestamp();

        m_history.append(chatMsg);
        while (m_history.size() > 5000) {
            m_history.removeAt(0);
        }
        saveHistory();
        broadcastMessage(chatMsg);
        emit logMessage(QString("Message from %1").arg(session->username()));
    }
}

void ChatServer::onSessionDisconnected(ClientSession *session)
{
    emit logMessage(QString("Client disconnected: %1").arg(session->username()));
    m_sessions.removeAll(session);
    session->deleteLater();
}

void ChatServer::loadUsers()
{
    QFile f("users.json");
    if (!f.exists()) {
        QJsonArray arr;
        QJsonObject a;
        a["username"] = "admin";
        a["password"] = "123456";
        arr.append(a);
        QJsonObject b;
        b["username"] = "demo";
        b["password"] = "demo123";
        arr.append(b);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
            f.close();
        }
    }

    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }
    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    const QJsonArray arr = doc.array();
    for (int i = 0; i < arr.size(); ++i) {
        const QJsonObject item = arr.at(i).toObject();
        m_users[item.value("username").toString()] = item.value("password").toString();
    }
}

void ChatServer::loadHistory()
{
    QFile f("chat_history.json");
    if (!f.exists()) {
        return;
    }
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }
    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    const QDateTime now = QDateTime::currentDateTime();
    const QJsonArray all = doc.array();
    for (int i = 0; i < all.size(); ++i) {
        const QJsonObject obj = all.at(i).toObject();
        const QDateTime ts = QDateTime::fromString(obj.value("timestamp").toString(), "yyyy-MM-dd HH:mm:ss");
        if (ts.isValid() && ts.secsTo(now) <= 24 * 60 * 60) {
            m_history.append(obj);
        }
    }
}

void ChatServer::saveHistory()
{
    QFile f("chat_history.json");
    if (!f.open(QIODevice::WriteOnly)) {
        return;
    }
    f.write(QJsonDocument(m_history).toJson(QJsonDocument::Indented));
    f.close();
}

void ChatServer::sendLoginResult(ClientSession *session, bool ok, const QString &reason)
{
    QJsonObject resp;
    resp["type"] = "login_result";
    resp["ok"] = ok;
    resp["reason"] = reason;

    QJsonArray history;
    for (int i = 0; i < m_history.size(); ++i) {
        history.append(m_history.at(i));
    }
    resp["history"] = history;

    session->sendJson(resp);
}

void ChatServer::broadcastMessage(const QJsonObject &chatMsg)
{
    foreach (ClientSession *s, m_sessions) {
        if (s->isLoggedIn()) {
            s->sendJson(chatMsg);
        }
    }
}
