#include "logindialog.h"
#include "ui_logindialog.h"
#include "chatclient.h"

#include <QMessageBox>

LoginDialog::LoginDialog(ChatClient *client, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , m_client(client)
{
    ui->setupUi(this);
    connect(ui->btnLogin, SIGNAL(clicked(bool)), this, SLOT(doLogin()));
    connect(m_client, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_client, SIGNAL(loginResult(bool,QString,QJsonArray)), this, SLOT(onLoginResult(bool,QString,QJsonArray)));
    connect(m_client, SIGNAL(error(QString)), this, SLOT(onError(QString)));
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

QString LoginDialog::username() const
{
    return m_username;
}

void LoginDialog::doLogin()
{
    ui->btnLogin->setEnabled(false);
    m_username = ui->editUser->text().trimmed();
    m_client->connectToServer(ui->editHost->text().trimmed(), ui->spinPort->value());
}

void LoginDialog::onConnected()
{
    m_client->login(ui->editUser->text().trimmed(), ui->editPass->text());
}

void LoginDialog::onLoginResult(bool ok, const QString &reason, const QJsonArray &)
{
    ui->btnLogin->setEnabled(true);
    if (ok) {
        accept();
    } else {
        QMessageBox::warning(this, "登录失败", reason);
    }
}

void LoginDialog::onError(const QString &errorText)
{
    ui->btnLogin->setEnabled(true);
    QMessageBox::warning(this, "网络错误", errorText);
}
