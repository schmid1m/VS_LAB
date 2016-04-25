/**************************************************************
**  File        : Macros.h                                   **
**  Version     : 2.3                                        **
**  Author      : Michel Schmidt (schmid1m@hs-pforzheim.de)  **
**  Created     : 18.04.2016                                 **
**  Last change : 25.04.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#ifndef VSLAB_MACROS_H
#define VSLAB_MACROS_H

// Protocol specific definitions
#define HEADER_LENGTH                 8 ///< The header length in bytes
#define VALUE_RESERVED                0 ///< Standard value for reserved fields
#define MAX_PACKET_LENGTH         60000 ///< The maximal packet length (60kB)

// Version definition
#define PROTOCOL_VERSION             14 ///< The version of the protocol

// Mode definitions (Source type)
#define MODE_STATUS                   1 ///< The status script is the message source
#define MODE_SERVER                   2 ///< The message originated from a server
#define MODE_CLIENT                   3 ///< A client sent the message

// Function definitions
#define FNC_POLYNOME                  0 ///< Sets the polynome in the server
#define FNC_DECRYPT                   1 ///< Decrypts a chunk of the file
#define FNC_UNLOCK                    2 ///< Unlocks the server to make it available for other clients
#define FNC_BROADCAST                 5 ///< Broadcast to discover all available servers
#define FNC_STATUS                    6 ///< Status request for that node

// Message type definitions
#define MSG_REQUEST                   3 ///< Request the specified function
#define MSG_RESPONSE                  4 ///< Response to an earlier request
#define MSG_ERROR                    15 ///< An error occurred decoding or executing

// Error code definitions
#define NO_ERROR                   	  0 ///< No error detected
#define ERR_PACKETLENGTH              1 ///< The packet length is invalid or does not match the actual length
#define ERR_INVALIDVERSION            2 ///< The version does not match the one defined in PACKET_LENGTH
#define ERR_INVALIDMODE               3 ///< The mode does not exist
#define ERR_NOSUCHFUNCTION            4 ///< The requested function does not exist (on this node)
#define ERR_INVALIDTYPE               5 ///< The type is not specified
#define ERR_HEADER_DATA               6 ///< Inconsistent header data. Header is not valid
#define ERR_DATA                      8 ///< Error in the data field detected
#define ERR_SERVERINUSE              16 ///< The server is currently used by another client
#define ERR_FUNCTIONTIMEOUT          32 ///< The called function timed out
#define ERR_FUNCTIONEXEC             33 ///< An error executing this function was detected
#define ERR_DECRYPT                  64 ///< The data could not be decrypted due to an error
#define ERR_ALLOC                   128 ///< Not enough free space to allocate data
#define ERR_UNKNOWN                 255 ///< An error occurred that does not match any of the other ones (this should never happen)

// General Definitions
#define ERROR                        -1 ///< An error occurred during excecution
#define SUCCESS                       1 ///< Function ran without problems

#endif // VSLAB_MACROS_H
