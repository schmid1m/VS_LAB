/**************************************************************
**  File        : internalMacros.h                           **
**  Version     : 2.4                                        **
**  Created     : 25.04.2016                                 **
**  Last change : 25.04.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#ifndef VSLAB_INT_MACROS_H
#define VSLAB_INT_MACROS_H

/// \defgroup internal Macros for internal use only
/// @{

// client socket
static int clScktDscp = 0; // socket descriptor
static struct sockaddr_in	client;	// address information
static struct sockaddr_in	client_target;	// address information
#define SERVER_PORT		11111
#define SERVER_UNICAST_ADDRESS	"141.47.69.14"

// server socket
static int svScktDscp = 0; // socket descriptor
static struct sockaddr_in	server;	// address information
static struct sockaddr_in	server_target;	// address information

// Protocol specific definitions
#define HEADER_LENGTH                 8 ///< The header length in bytes
#define VALUE_RESERVED                0 ///< Standard value for reserved fields
#define MAX_PACKET_LENGTH         60000 ///< The maximal packet length (60kB)
#define NO_BLOCK_ID                   0 ///< No block ID is present
#define SERVER_PRIO                   0 ///< Priority of server messages

// Version definition
#define PROTOCOL_VERSION             14 ///< The version of the protocol

// Mode definitions (Source type)
#define MODE_STATUS                   1 ///< The status script is the message source
#define MODE_SERVER                   2 ///< The message originated from a server
#define MODE_CLIENT                   3 ///< A client sent the message

// Function definitions
#define FNC_GP                		  0 ///< Sets the polynome in the server
#define FNC_DECRYPT                   1 ///< Decrypts a chunk of the file
#define FNC_UNLOCK                    2 ///< Unlocks the server to make it available for other clients
#define FNC_BROADCAST                 5 ///< Broadcast to discover all available servers
#define FNC_STATUS                    6 ///< Status request for that node

// Message type definitions
#define MSG_REQUEST                   3 ///< Request the specified function
#define MSG_RESPONSE                  4 ///< Response to an earlier request
#define MSG_ERROR                    15 ///< An error occurred decoding or executing

/// @}

#endif // VSLAB_INT_MACROS_H
