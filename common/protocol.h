#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>
#include <QByteArray>
#include <QJsonObject>

namespace Protocol {

const int MaxPacketSize = 2 * 1024 * 1024;

QByteArray encryptJson(const QJsonObject &obj);
QJsonObject decryptJson(const QByteArray &cipherText, bool *ok);

QByteArray buildPacket(const QJsonObject &obj);
bool tryReadPacket(QByteArray &buffer, QByteArray &packetData);

QString currentTimestamp();

}

#endif // PROTOCOL_H
