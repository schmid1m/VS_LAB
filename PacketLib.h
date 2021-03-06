/**************************************************************
**  File        : PacketLib.h                                **
**  Version     : 2.7                                        **
**  Created     : 19.04.2016                                 **
**  Last change : 26.05.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#ifndef PACKET_LIB_H
#define PACKET_LIB_H

#include <stdint.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>

#ifdef  SHOW_API_ERRORS
#define DEBUG_PRINTF(x)        printf(x);
#define DEBUG_PRINTF1(x,y)     printf(x,y);
#define DEBUG_PRINTF2(x,y,z)   printf(x,y,z);
#define DEBUG_PRINTF3(x,y,z,a) printf(x,y,z,a);
#else  // SHOW_API_ERRORS
#define DEBUG_PRINTF(x)
#define DEBUG_PRINTF1(x,y)
#define DEBUG_PRINTF2(x,y,z)
#define DEBUG_PRINTF3(x,y,z,a)
#endif // SHOW_API_ERRORS

/// \defgroup structs Structures
/// Data Structures for internal use
/// @{

extern int socketDscp;                      // socket descriptor
extern struct sockaddr_in   my_addr;        // my own address information
extern struct sockaddr_in   target_addr;    // target address information

/// @brief A structure for the message header
/// You can easily type cast the first 8 Byte of a message to this struct.
/// The internal structure holds all the values then.
typedef struct msg_header
{
    uint8_t priority;                       ///< The priority of the message (0 = HIGH, 255 = LOW)
    uint8_t version;                        ///< The current version of the script @sa PROTOCOL_VERSION
    uint8_t mode;                           ///< The mode of the message (sender type) @sa MODE_STATUS @sa MODE_SERVER @sa MODE_CLIENT
    uint8_t type:4;                         ///< The message Type @sa MSG_REQUEST @sa MSG_RESPONSE @sa MSG_ERROR
    uint8_t func:4;                         ///< The called function of this message @sa FNC_POLYNOME @sa FNC_DECRYPT @sa FNC_UNLOCK @sa FNC_BROADCAST @sa FNC_STATUS
    uint16_t length;                        ///< The Length of the message data field
    uint16_t reserved;                      ///< Reserved @sa VALUE_RESERVED
}__attribute__((__packed__)) msg_header;

/// @brief Set polynome request
/// This function tries to set the polynome if the server is free or the current client has lower priority
typedef struct dat_gp_request
{
    int16_t clientID;                       ///< The ID of the requesting client
    uint16_t generator;                     ///< The generator polynome
}__attribute__((__packed__)) dat_gp_request;

/// @brief Decrypt data
/// Request to decrypt data. Polynome has to be set first. @sa dat_polynom_request
typedef struct dat_decrypt_request
{
    int16_t clientID;                       ///< The ID of the requesting client
    uint16_t blockID;                       ///< A (random) Block ID to tell the packets apart
    uint16_t firstElement;                  ///< First element of the data structure (16 Bit Chunks)
}__attribute__((__packed__)) dat_decrypt_request;

/// @brief Return decrypted data
/// Returns the data from successfull decryption
typedef struct dat_decrypt_response
{
    int16_t clientID;                       ///< The ID of the requesting client
    uint16_t blockID;                       ///< The Block ID set by the client
    uint8_t firstElement;                   ///< First element of the data structure (8 Bit Chunks)
}__attribute__((__packed__)) dat_decrypt_response;

/// @brief Unlocks the server
/// After the server is not needed anymore you can unlock it with this function.
/// The server can then be used by another slave @sa dat_polynom_request
typedef struct dat_unlock_request
{
    int16_t clientID;                       ///< The ID of the current client
    uint16_t reserved;                      ///< Reserved @sa VALUE_RESERVED
}__attribute__((__packed__)) dat_unlock_request;

/// @brief Response to a status request
/// Servers respond with their current status
typedef struct dat_status_response
{
    int16_t clientID;                       ///< The ID of the currently connected client
    uint16_t reserved;                      ///< Reserved @sa VALUE_RESERVED
    uint32_t wordCount;                     ///< Amount of Decrypted data words for this client
}__attribute__((__packed__)) dat_status_response;

/// @brief Error frame
/// An error message frame
/// @sa macros
typedef struct error
{
    uint8_t errCode;                        ///< The error code of the occurring error @sa NO_ERROR @sa ERR_PACKETLENGTH @sa ERR_INVALIDVERSION @sa ERR_INVALIDMODE @sa ERR_NOSUCHFUNCTION @sa ERR_INVALIDTYPE @sa ERR_HEADER_DATA @sa ERR_DATA @sa ERR_SERVERINUSE @sa ERR_FUNCTIONTIMEOUT @sa ERR_FUNCTIONEXEC @sa ERR_DECRYPT @sa ERR_ALLOC @sa ERR_NO_PACKET @sa ERR_UNKNOWN
    uint16_t blockID;                       ///< Block ID where the error occurred (for ERR_DECRYPT and ERR_SERVERINUSE). Else 0.
}__attribute__((__packed__)) error;

/// @brief Structure for a message
/// This structure holds pointers for the message header and the data structure
typedef struct msg
{
    msg_header* header;                     ///< A pointer to the header of the structure. @sa msg_header
    void* data;                             ///< A pounter to the data field of the structure. Nullpointer for no data field. @sa dat_polynom_request @sa dat_decrypt_request @sa dat_decrypt_response @sa dat_unlock_request @sa dat_broadcast_response @sa dat_status_response @sa error
}__attribute__((__packed__)) msg;

/// @}

/// \defgroup functions Internal Functions
/// Functions for internal use only
/// @{

/// \brief check_pointers
/// @author Michel Schmidt
/// \param packet : the packet pointers to check
/// \return ERR_INVALID_PTR or NO_ERROR
/// @sa ERR_INVALID_PTR @sa NO_ERROR
uint8_t check_pointers(msg* packet);

/// \brief Check a packet for internal errors
/// @author Michel Schmidt
/// \param[in] packet : The packet structure
/// \return The error code that occurred
/// @sa macros
uint8_t check_packet(msg* packet);

/// \brief Sends a message via UDP
/// @author Stefan Scharrenbach
/// \param[in] packet: The packet to send
/// \param[in] target_ip: The IP to send to
/// \param[in] target_port: The Port to send to
/// \return The error code that occurred
/// @sa msg
/// @sa macros
uint8_t send_msg(msg* packet, uint32_t target_ip, uint16_t target_port);

/// @}

#endif // PACKET_LIB_H
