TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    clientAPI.c \
    PacketLib.c \
    serverAPI.c

HEADERS += \
    clientAPI.h \
    commonAPI.h \
    internalMacros.h \
    Macros.h \
    PacketLib.h \
    serverAPI.h

