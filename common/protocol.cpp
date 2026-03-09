#include "protocol.h"

#include <QJsonDocument>
#include <QDataStream>
#include <QDateTime>

#include <windows.h>
#include <bcrypt.h>

#pragma comment(lib, "bcrypt.lib")

namespace {

QByteArray aesKey()
{
    return QByteArray("0123456789abcdef0123456789abcdef"); // 32 bytes
}

QByteArray aesIv()
{
    return QByteArray("abcdef0123456789"); // 16 bytes
}

QByteArray aesCrypt(const QByteArray &input, bool encrypt)
{
    BCRYPT_ALG_HANDLE algHandle = NULL;
    BCRYPT_KEY_HANDLE keyHandle = NULL;
    QByteArray output;

    NTSTATUS status = BCryptOpenAlgorithmProvider(&algHandle, BCRYPT_AES_ALGORITHM, NULL, 0);
    if (status < 0) {
        return QByteArray();
    }

    status = BCryptSetProperty(algHandle,
                               BCRYPT_CHAINING_MODE,
                               reinterpret_cast<PUCHAR>(const_cast<wchar_t*>(BCRYPT_CHAIN_MODE_CBC)),
                               static_cast<ULONG>(wcslen(BCRYPT_CHAIN_MODE_CBC) * sizeof(wchar_t)),
                               0);
    if (status < 0) {
        BCryptCloseAlgorithmProvider(algHandle, 0);
        return QByteArray();
    }

    DWORD keyObjectSize = 0;
    DWORD cbResult = 0;
    status = BCryptGetProperty(algHandle,
                               BCRYPT_OBJECT_LENGTH,
                               reinterpret_cast<PUCHAR>(&keyObjectSize),
                               sizeof(keyObjectSize),
                               &cbResult,
                               0);
    if (status < 0) {
        BCryptCloseAlgorithmProvider(algHandle, 0);
        return QByteArray();
    }

    QByteArray keyObject;
    keyObject.resize(static_cast<int>(keyObjectSize));
    QByteArray key = aesKey();
    QByteArray iv = aesIv();

    status = BCryptGenerateSymmetricKey(algHandle,
                                        &keyHandle,
                                        reinterpret_cast<PUCHAR>(keyObject.data()),
                                        keyObjectSize,
                                        reinterpret_cast<PUCHAR>(key.data()),
                                        static_cast<ULONG>(key.size()),
                                        0);
    if (status < 0) {
        BCryptCloseAlgorithmProvider(algHandle, 0);
        return QByteArray();
    }

    DWORD outSize = 0;
    if (encrypt) {
        status = BCryptEncrypt(keyHandle,
                               reinterpret_cast<PUCHAR>(const_cast<char*>(input.constData())),
                               static_cast<ULONG>(input.size()),
                               NULL,
                               reinterpret_cast<PUCHAR>(iv.data()),
                               static_cast<ULONG>(iv.size()),
                               NULL,
                               0,
                               &outSize,
                               BCRYPT_BLOCK_PADDING);
    } else {
        status = BCryptDecrypt(keyHandle,
                               reinterpret_cast<PUCHAR>(const_cast<char*>(input.constData())),
                               static_cast<ULONG>(input.size()),
                               NULL,
                               reinterpret_cast<PUCHAR>(iv.data()),
                               static_cast<ULONG>(iv.size()),
                               NULL,
                               0,
                               &outSize,
                               BCRYPT_BLOCK_PADDING);
    }

    if (status < 0) {
        BCryptDestroyKey(keyHandle);
        BCryptCloseAlgorithmProvider(algHandle, 0);
        return QByteArray();
    }

    output.resize(static_cast<int>(outSize));

    if (encrypt) {
        status = BCryptEncrypt(keyHandle,
                               reinterpret_cast<PUCHAR>(const_cast<char*>(input.constData())),
                               static_cast<ULONG>(input.size()),
                               NULL,
                               reinterpret_cast<PUCHAR>(iv.data()),
                               static_cast<ULONG>(iv.size()),
                               reinterpret_cast<PUCHAR>(output.data()),
                               outSize,
                               &outSize,
                               BCRYPT_BLOCK_PADDING);
    } else {
        status = BCryptDecrypt(keyHandle,
                               reinterpret_cast<PUCHAR>(const_cast<char*>(input.constData())),
                               static_cast<ULONG>(input.size()),
                               NULL,
                               reinterpret_cast<PUCHAR>(iv.data()),
                               static_cast<ULONG>(iv.size()),
                               reinterpret_cast<PUCHAR>(output.data()),
                               outSize,
                               &outSize,
                               BCRYPT_BLOCK_PADDING);
    }

    BCryptDestroyKey(keyHandle);
    BCryptCloseAlgorithmProvider(algHandle, 0);

    if (status < 0) {
        return QByteArray();
    }

    output.resize(static_cast<int>(outSize));
    return output;
}

QByteArray aesEncrypt(const QByteArray &plain)
{
    return aesCrypt(plain, true);
}

QByteArray aesDecrypt(const QByteArray &cipher)
{
    return aesCrypt(cipher, false);
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
