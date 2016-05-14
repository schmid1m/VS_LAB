/**************************************************************
**  File        : commonAPI.h                                **
**  Version     : 2.6                                        **
**  Created     : 25.04.2016                                 **
**  Last change : 10.05.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#ifndef VSLAB_COMMON_API_H
#define VSLAB_COMMON_API_H

#include "Macros.h"
#include "PacketLib.h"

/// \defgroup general General API functions
/// API Functions that apply to both sides, client and server
/// @{

/// \brief Receive a message
/// This function receives a message if there is one present on the Server.
/// It also does a syntax check on the message to find badly generated packets.
/// @author Cornelius Bott
/// \param[out] packet : The received packet
/// \param[out] src_ip : Source-IP-Address
/// \return The error code of the message. ERR_NO_PACKET if there is no message.
/// @sa msg
/// @sa macros
uint8_t recv_msg(msg* packet, uint32_t* src_ip);

/// \brief get_msg_type
/// This function returns the type of the current message.
/// @author Michel Schmidt
/// \param[in] packet : The packet to get the type of.
/// \return The error code that occurred.
/// @sa FID
/// @sa macros
FID get_msg_type(msg* packet);

/// \brief Deletes the subfields of a msg type (But not the msg itself!)
/// @author Michel Schmidt
/// \param[in] packet : the message to delete
/// \return ERROR if the packet was not valid; SUCCESS if not
uint8_t free_msg(msg* packet);

/// \brief Frees the data stream allocated by extract_dec_req / _rsp
/// @author Philipp Duller
/// \param[in] ptr : the data field to be deleted
/// \return NO_ERROR
uint8_t free_data(uint8_t* ptr);

/// @}

#endif // VSLAB_COMMON_API_H
