QT += core gui network widgets

CONFIG += c++11
TEMPLATE = app
TARGET = QtChatServer

SOURCES += \
    main.cpp \
    serverwindow.cpp \
    chatserver.cpp \
    clientsession.cpp \
    ../common/protocol.cpp

HEADERS += \
    serverwindow.h \
    chatserver.h \
    clientsession.h \
    ../common/protocol.h

FORMS += \
    serverwindow.ui

INCLUDEPATH += ../common

LIBS += -lbcrypt
