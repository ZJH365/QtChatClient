#include "serverwindow.h"
#include "ui_serverwindow.h"
#include "chatserver.h"

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerWindow)
    , m_server(new ChatServer(this))
{
    ui->setupUi(this);
    connect(ui->btnStart, SIGNAL(clicked(bool)), this, SLOT(onStartClicked()));
    connect(ui->btnStop, SIGNAL(clicked(bool)), this, SLOT(onStopClicked()));
    connect(m_server, SIGNAL(logMessage(QString)), this, SLOT(appendLog(QString)));

    ui->spinPort->setValue(8989);
    ui->btnStop->setEnabled(false);
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

void ServerWindow::onStartClicked()
{
    if (m_server->start(ui->spinPort->value())) {
        ui->btnStart->setEnabled(false);
        ui->btnStop->setEnabled(true);
    }
}

void ServerWindow::onStopClicked()
{
    m_server->stop();
    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);
}

void ServerWindow::appendLog(const QString &text)
{
    ui->txtLog->appendPlainText(text);
}
