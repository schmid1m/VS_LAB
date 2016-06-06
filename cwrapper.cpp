#include "cwrapper.h"

#include "myqtsocket.h"

uint32_t qt_send(uint8_t* data, uint32_t data_len, uint32_t addr, uint16_t port)
{
    return MyQtSocket::Send(data, data_len, addr, port);
}
