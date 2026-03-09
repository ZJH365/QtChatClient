#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QTcpSocket>

class ChatClient : public QObject
{
    Q_OBJECT
public:
    explicit ChatClient(QObject *parent = 0);

    void connectToServer(const QString &host, quint16 port);
    void login(const QString &username, const QString &password);
    void sendChat(const QString &text);

signals:
    void connected();
    void disconnected();
    void loginResult(bool ok, const QString &reason, const QJsonArray &history);
    void messageReceived(const QString &sender, const QString &text, const QString &timestamp);
    void error(const QString &errorText);

private slots:
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError);

private:
    QTcpSocket m_socket;
    QByteArray m_buffer;
};

#endif // CHATCLIENT_H
