#include "logindialog.h"
#include "clientwindow.h"
#include "chatclient.h"

#include <QApplication>
#include <QJsonArray>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ChatClient client;
    QJsonArray history;

    QObject::connect(&client, &ChatClient::loginResult,
                     [&](bool ok, const QString &, const QJsonArray &h) {
        if (ok) {
            history = h;
        }
    });

    LoginDialog login(&client);
    if (login.exec() != QDialog::Accepted) {
        return 0;
    }

    ClientWindow w(&client, login.username());
    w.loadHistory(history);
    w.show();

    return app.exec();
}
