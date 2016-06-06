#ifndef CWRAPPER_H
#define CWRAPPER_H

#include "PacketLib.h"

#ifdef __cplusplus
 #define EXTERNC extern "C"
 #else
 #define EXTERNC
 #endif

 EXTERNC uint32_t qt_send(uint8_t* data, uint32_t data_len, uint32_t addr, uint16_t port);

#undef EXTERNC
#endif // CWRAPPER_H
