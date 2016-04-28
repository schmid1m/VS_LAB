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

extern uint8_t *rec_buf;

// server socket
static int socketDscp = 0;                  // socket descriptor
static struct sockaddr_in	my_addr;		// my own address information
static struct sockaddr_in	target_addr;	// target address information

/// \defgroup general General API functions
/// API Functions that apply to both sides, client and server
/// @{

/// \brief Receive a message
/// This function receives a message if there is one present on the socket.
/// It also does a syntax check on the message to find badly generated packets.
/// @author <Author Name Here>
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

/// \brief Deletes a packet structure and its subfields
/// @author Michel Schmidt
/// \param packet : the message to delete
/// \return ERROR if the packet was not valid; SUCCESS if not
uint8_t free_msg(msg* packet);

/// @}

#endif // VSLAB_COMMON_API_H
