#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include <QJsonArray>

class ChatClient;

namespace Ui {
class ClientWindow;
}

class ClientWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ClientWindow(ChatClient *client, const QString &username, QWidget *parent = 0);
    ~ClientWindow();

    void loadHistory(const QJsonArray &history);

private slots:
    void sendMessage();
    void onMessageReceived(const QString &sender, const QString &text, const QString &timestamp);
    void onNavChanged(int row);

private:
    void appendMessage(const QString &sender, const QString &text, const QString &timestamp);

    Ui::ClientWindow *ui;
    ChatClient *m_client;
    QString m_username;
};

#endif // CLIENTWINDOW_H
