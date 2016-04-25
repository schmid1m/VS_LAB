/**************************************************************
**  File        : PacketLib.h                                **
**  Version     : 2.4                                        **
**  Created     : 19.04.2016                                 **
**  Last change : 25.04.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#ifndef PACKET_LIB_H
#define PACKET_LIB_H

#include <stdint.h>

/// @brief A structure for the message header
/// You can easily type cast the first 8 Byte of a message to this struct.
/// The internal structure holds all the values then.
typedef struct msg_header
{
	uint8_t priority;						///< The priority of the message (0 = HIGH, 255 = LOW)
	uint8_t version;						///< The current version of the script @sa PROTOCOL_VERSION
	uint8_t mode;							///< The mode of the message (sender type) @sa MODE_STATUS @sa MODE_SERVER @sa MODE_CLIENT
	uint8_t func:4;							///< The called function of this message @sa FNC_POLYNOME @sa FNC_DECRYPT @sa FNC_UNLOCK @sa FNC_BROADCAST @sa FNC_STATUS
    uint8_t type:4;							///< The message Type @sa MSG_REQUEST @sa MSG_RESPONSE @sa MSG_ERROR
	uint16_t length;						///< The Length of the message data field
	uint16_t reserved;						///< Reserved @sa VALUE_RESERVED
}__attribute__((__packed__)) msg_header;

/// @brief Set polynome request
/// This function tries to set the polynome if the server is free or the current client has lower priority
typedef struct dat_polynom_request
{
	int16_t clientID;						///< The ID of the requesting client
	uint16_t generator;						///< The generator polynome
}__attribute__((__packed__)) dat_polynom_request;

/// @brief Decrypt data
/// Request to decrypt data. Polynome has to be set first. @sa dat_polynom_request
typedef struct dat_decrypt_request
{
	int16_t clientID;						///< The ID of the requesting client
    uint16_t blockID;   					///< A (random) Block ID to tell the packets apart
	uint16_t firstElement;					///< First element of the data structure (16 Bit Chunks)
}__attribute__((__packed__)) dat_decrypt_request;

/// @brief Return decrypted data
/// Returns the data from successfull decryption
typedef struct dat_decrypt_response
{
    int16_t clientID;						///< The ID of the requesting client
    uint16_t blockID;   					///< The Block ID set by the client
	uint8_t firstElement;					///< First element of the data structure (8 Bit Chunks)
}__attribute__((__packed__)) dat_decrypt_response;

/// @brief Unlocks the server
/// After the server is not needed anymore you can unlock it with this function.
/// The server can then be used by another slave @sa dat_polynom_request
typedef struct dat_unlock_request
{
	int16_t clientID;						///< The ID of the current client
	uint16_t reserved;						///< Reserved @sa VALUE_RESERVED
}__attribute__((__packed__)) dat_unlock_request;

/// @brief Broadcast response
/// Each server responds to a broadcast sending its IP address
typedef struct dat_broadcast_response
{
    uint32_t serverIP;      				///< 4 times 1 Byte IP V4 address
}__attribute__((__packed__)) dat_broadcast_response;

/// @brief Response to a status request
/// Servers respond with their current status
typedef struct dat_status_response
{
	int16_t clientID;						///< The ID of the currently connected client
	uint16_t reserved;						///< Reserved @sa VALUE_RESERVED
	uint32_t wordCount;						///< Amount of Decrypted data words for this client
}__attribute__((__packed__)) dat_status_response;

/// @brief Error frame
/// 
typedef struct error
{
    uint8_t errCode;						///< The error code of the occurring error @sa ERR_PACKETLENGTH @sa ERR_INVALIDVERSION @sa ERR_INVALIDMODE @sa ERR_NOSUCHFUNCTION @sa ERR_INVALIDTYPE @sa ERR_DATA @sa ERR_SERVERINUSE @sa ERR_FUNCTIONTIMEOUT @sa ERR_FUNCTIONEXEC @sa ERR_DECRYPT @sa ERR_ALLOC @sa ERR_UNKNOWN
    uint16_t blockID;   					///< Block ID where the error occurred (for ERR_DECRYPT and ERR_SERVERINUSE). Else 0.
}__attribute__((__packed__)) error;

/// @brief Structure for a message
/// This structure holds pointers for the message header and the data structure
typedef struct msg
{
    msg_header* header;						///< A pointer to the header of the structure. @sa msg_header
    void* data;								///< A pounter to the data field of the structure. Nullpointer for no data field. @sa dat_polynom_request @sa dat_decrypt_request @sa dat_decrypt_response @sa dat_unlock_request @sa dat_broadcast_response @sa dat_status_response @sa error
}__attribute__((__packed__)) msg;

/// FUNCTION PROTOTYPES ///

/// \brief Check a packet for internal errors
/// \param[in] packet : The packet structure
/// \return The error code that occurred @sa NO_ERROR @sa ERR_PACKETLENGTH @sa ERR_INVALIDVERSION @sa ERR_INVALIDMODE @sa ERR_NOSUCHFUNCTION @sa ERR_INVALIDTYPE @sa ERR_DATA @sa ERR_SERVERINUSE @sa ERR_FUNCTIONTIMEOUT @sa ERR_FUNCTIONEXEC @sa ERR_DECRYPT @sa ERR_ALLOC @sa ERR_UNKNOWN
uint8_t check_packet(msg* packet);

uint8_t send_msg(msg* packet);

/*********************************/
/* OLD FUNCTIONS.... TO BE MOVED */
/*********************************/

/*
/// @brief Send a response to a broadcast from a client
/// @param[in] socketdesc : socket descriptor of the socket that should be used
/// @param[in] serverSocket : pointer to the server socket struct
/// @param[in] clientAddress : pointer to the client socket struct
/// \return ERROR if sending message isn't successful otherwise you'll get SUCCESS
int server_broadcast_rsp(int socketdesc, struct sockaddr_in *serverSocket, struct sockaddr_in *clientAddress);


int server_status_rsp(int socketdesc, struct sockaddr_in *statusAddress, ...);*/

/*********************************/
/* OLD FUNCTIONS.... TO BE MOVED */
/*********************************/

#endif // PACKET_LIB_H
