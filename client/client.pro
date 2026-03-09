QT += core gui network widgets

CONFIG += c++11
TEMPLATE = app
TARGET = QtChatClient

SOURCES += \
    main.cpp \
    logindialog.cpp \
    clientwindow.cpp \
    chatclient.cpp \
    ../common/protocol.cpp

HEADERS += \
    logindialog.h \
    clientwindow.h \
    chatclient.h \
    ../common/protocol.h

FORMS += \
    logindialog.ui \
    clientwindow.ui

INCLUDEPATH += ../common

LIBS += -lbcrypt
