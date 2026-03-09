#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>

class ChatServer;

namespace Ui {
class ServerWindow;
}

class ServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerWindow(QWidget *parent = 0);
    ~ServerWindow();

private slots:
    void onStartClicked();
    void onStopClicked();
    void appendLog(const QString &text);

private:
    Ui::ServerWindow *ui;
    ChatServer *m_server;
};

#endif // SERVERWINDOW_H
