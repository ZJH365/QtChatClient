#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QJsonArray>

class ChatClient;

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(ChatClient *client, QWidget *parent = 0);
    ~LoginDialog();

    QString username() const;

private slots:
    void doLogin();
    void onConnected();
    void onLoginResult(bool ok, const QString &reason, const QJsonArray &history);
    void onError(const QString &errorText);

private:
    Ui::LoginDialog *ui;
    ChatClient *m_client;
    QString m_username;
};

#endif // LOGINDIALOG_H
