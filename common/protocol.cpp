#include "protocol.h"

#include <QJsonDocument>
#include <QDataStream>
#include <QDateTime>
#include <QDebug>

#include <openssl/evp.h>

namespace {

QByteArray aesKey()
{
    return QByteArray("QtChatClientDemoKeyForAES256_2026!!");
}

QByteArray aesIv()
{
    return QByteArray("QtChatInitVector");
}

QByteArray aesEncrypt(const QByteArray &plain)
{
    QByteArray output;
    output.resize(plain.size() + EVP_MAX_BLOCK_LENGTH);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return QByteArray();
    }

    int outLen1 = 0;
    int outLen2 = 0;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                           reinterpret_cast<const unsigned char*>(aesKey().constData()),
                           reinterpret_cast<const unsigned char*>(aesIv().constData())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    if (EVP_EncryptUpdate(ctx,
                          reinterpret_cast<unsigned char*>(output.data()),
                          &outLen1,
                          reinterpret_cast<const unsigned char*>(plain.constData()),
                          plain.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    if (EVP_EncryptFinal_ex(ctx,
                            reinterpret_cast<unsigned char*>(output.data()) + outLen1,
                            &outLen2) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    EVP_CIPHER_CTX_free(ctx);
    output.resize(outLen1 + outLen2);
    return output;
}

QByteArray aesDecrypt(const QByteArray &cipher)
{
    QByteArray output;
    output.resize(cipher.size() + EVP_MAX_BLOCK_LENGTH);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return QByteArray();
    }

    int outLen1 = 0;
    int outLen2 = 0;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                           reinterpret_cast<const unsigned char*>(aesKey().constData()),
                           reinterpret_cast<const unsigned char*>(aesIv().constData())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    if (EVP_DecryptUpdate(ctx,
                          reinterpret_cast<unsigned char*>(output.data()),
                          &outLen1,
                          reinterpret_cast<const unsigned char*>(cipher.constData()),
                          cipher.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    if (EVP_DecryptFinal_ex(ctx,
                            reinterpret_cast<unsigned char*>(output.data()) + outLen1,
                            &outLen2) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    EVP_CIPHER_CTX_free(ctx);
    output.resize(outLen1 + outLen2);
    return output;
}

}

namespace Protocol {

QByteArray encryptJson(const QJsonObject &obj)
{
    const QByteArray jsonData = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    return aesEncrypt(jsonData);
}

QJsonObject decryptJson(const QByteArray &cipherText, bool *ok)
{
    const QByteArray plain = aesDecrypt(cipherText);
    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(plain, &error);
    const bool isOk = (error.error == QJsonParseError::NoError && doc.isObject());
    if (ok) {
        *ok = isOk;
    }
    if (!isOk) {
        return QJsonObject();
    }
    return doc.object();
}

QByteArray buildPacket(const QJsonObject &obj)
{
    const QByteArray cipher = encryptJson(obj);
    QByteArray packet;
    QDataStream stream(&packet, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_8);
    stream << static_cast<quint32>(cipher.size());
    packet.append(cipher);
    return packet;
}

bool tryReadPacket(QByteArray &buffer, QByteArray &packetData)
{
    if (buffer.size() < static_cast<int>(sizeof(quint32))) {
        return false;
    }

    QDataStream stream(buffer);
    stream.setVersion(QDataStream::Qt_5_8);
    quint32 payloadSize = 0;
    stream >> payloadSize;

    if (payloadSize > MaxPacketSize) {
        buffer.clear();
        return false;
    }

    const int fullSize = sizeof(quint32) + static_cast<int>(payloadSize);
    if (buffer.size() < fullSize) {
        return false;
    }

    packetData = buffer.mid(sizeof(quint32), payloadSize);
    buffer.remove(0, fullSize);
    return true;
}

QString currentTimestamp()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
}

}
