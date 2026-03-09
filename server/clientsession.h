#ifndef CLIENTSESSION_H
#define CLIENTSESSION_H

#include <QObject>
#include <QTcpSocket>

class ClientSession : public QObject
{
    Q_OBJECT
public:
    explicit ClientSession(QTcpSocket *socket, QObject *parent = 0);

    QTcpSocket *socket() const;
    QString username() const;
    void setUsername(const QString &name);
    bool isLoggedIn() const;

    void sendJson(const QJsonObject &obj);

signals:
    void packetReceived(ClientSession *session, const QJsonObject &obj);
    void disconnected(ClientSession *session);

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket *m_socket;
    QByteArray m_buffer;
    QString m_username;
};

#endif // CLIENTSESSION_H
