/**************************************************************
**  File        : commonAPI.h                                **
**  Version     : 2.4                                        **
**  Created     : 25.04.2016                                 **
**  Last change : 25.04.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#ifndef VSLAB_COMMON_API_H
#define VSLAB_COMMON_API_H

#include "Macros.h"
#include "PacketLib.h"

uint8_t recv_msg(msg* packet);
FID get_msg_type(msg* packet);

#endif // VSLAB_COMMON_API_H
