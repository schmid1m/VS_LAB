#ifndef MYQTSOCKET_H
#define MYQTSOCKET_H

#include <QObject>
#include <QUdpSocket>

#include "PacketLib.h"
#include "Macros.h"
#include "internalMacros.h"

class MyQtSocket : public QObject
{
    Q_OBJECT
public:
    MyQtSocket(QObject *parent = 0);
    uint8_t Read(msg *packet, uint32_t* addr, uint16_t* port);
    static uint32_t Send(uint8_t* data, uint32_t data_len, uint32_t addr, uint16_t port);
    static QUdpSocket my_sock;

private:

signals:

public slots:
};

#endif // MYQTSOCKET_H
