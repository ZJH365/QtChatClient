#include "clientwindow.h"
#include "ui_clientwindow.h"
#include "chatclient.h"

#include <QJsonObject>

ClientWindow::ClientWindow(ChatClient *client, const QString &username, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
    , m_client(client)
    , m_username(username)
{
    ui->setupUi(this);

    ui->labelNick->setText(username);
    ui->labelAvatar->setText("🙂");

    connect(ui->btnSend, SIGNAL(clicked(bool)), this, SLOT(sendMessage()));
    connect(m_client, SIGNAL(messageReceived(QString,QString,QString)),
            this, SLOT(onMessageReceived(QString,QString,QString)));
    connect(ui->listNav, SIGNAL(currentRowChanged(int)), this, SLOT(onNavChanged(int)));
    ui->listNav->setCurrentRow(0);
}

ClientWindow::~ClientWindow()
{
    delete ui;
}

void ClientWindow::loadHistory(const QJsonArray &history)
{
    for (int i = 0; i < history.size(); ++i) {
        const QJsonObject obj = history.at(i).toObject();
        appendMessage(obj.value("sender").toString(),
                      obj.value("text").toString(),
                      obj.value("timestamp").toString());
    }
}

void ClientWindow::sendMessage()
{
    const QString text = ui->editMessage->text().trimmed();
    if (text.isEmpty()) {
        return;
    }
    m_client->sendChat(text);
    ui->editMessage->clear();
}

void ClientWindow::onMessageReceived(const QString &sender, const QString &text, const QString &timestamp)
{
    appendMessage(sender, text, timestamp);
}

void ClientWindow::onNavChanged(int row)
{
    if (row == 0) {
        ui->stackedWidget->setCurrentIndex(0);
    } else {
        ui->stackedWidget->setCurrentIndex(1);
    }
}

void ClientWindow::appendMessage(const QString &sender, const QString &text, const QString &timestamp)
{
    ui->textChat->appendPlainText(QString("[%1] %2: %3").arg(timestamp).arg(sender).arg(text));
}
