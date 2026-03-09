#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QMap>
#include <QJsonArray>

class ClientSession;

class ChatServer : public QObject
{
    Q_OBJECT
public:
    explicit ChatServer(QObject *parent = 0);

    bool start(quint16 port);
    void stop();

signals:
    void logMessage(const QString &text);

private slots:
    void onNewConnection();
    void onPacket(ClientSession *session, const QJsonObject &obj);
    void onSessionDisconnected(ClientSession *session);

private:
    void loadUsers();
    void loadHistory();
    void saveHistory();
    void sendLoginResult(ClientSession *session, bool ok, const QString &reason);
    void broadcastMessage(const QJsonObject &chatMsg);

    QTcpServer m_server;
    QList<ClientSession*> m_sessions;
    QMap<QString, QString> m_users;
    QJsonArray m_history;
};

#endif // CHATSERVER_H
