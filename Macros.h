/**************************************************************
**  File        : Macros.h                                   **
**  Version     : 2.6                                        **
**  Created     : 18.04.2016                                 **
**  Last change : 10.05.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#ifndef VSLAB_MACROS_H
#define VSLAB_MACROS_H

#include"internalMacros.h"

/// \defgroup macros Macros
/// Macros and Enumerations used for the API
/// @{
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
#define ERR_LOCK_TIMEOUT             34 ///< The Server lock timed out
#define ERR_DECRYPT                  64 ///< The data could not be decrypted due to an error
#define ERR_ALLOC                   128 ///< Not enough free space to allocate data
#define ERR_INVALID_PTR             129 ///< The given pointer was not valid
#define ERR_NOTFORME                130 ///< Client detected client ID miss match
#define ERR_NO_GP                   131 ///< No GP is set in Server
#define ERR_SEND_ERROR              252 ///< Could not send message
#define ERR_NO_INIT                 253 ///< The API lib was not initialized
#define ERR_NO_PACKET               254 ///< No Packet was on the socket
#define ERR_UNKNOWN                 255 ///< An error occurred that does not match any of the other ones (this should never happen)

// General Definitions
#define ERROR                        -1 ///< An error occurred during excecution
#define SUCCESS                       1 ///< Function ran without problems

// Function ID Definitions
/// @brief An enumeration of all possible functions
/// This is used as function ID reference
typedef enum{
	GP_REQ,                       		///< Function : set polynome; Type : Request
	GP_RSP,                      		///< Function : set polynome; Type : Response
	DECRYPT_REQ,                        ///< Function : decrypt data; Type : Request
    DECRYPT_RSP,                        ///< Function : decrypt data; Type : Response
    UNLOCK_REQ,                         ///< Function : unlock server; Type : Request
    UNLOCK_RSP,                         ///< Function : unlock server; Type : Response
    BROADCAST_REQ,                      ///< Function : broadcast; Type : Request
    BROADCAST_RSP,                      ///< Function : broadcast; Type : Response
    STATUS_REQ,                         ///< Function : status check; Type : Request
    STATUS_RSP,                         ///< Function : status check; Type : Response
    UNKNOWN,                            ///< Unknown function. This should not happen.
    ERROR_RSP                           ///< Function : any; Type : Error
}FID;

/// @}

#endif // VSLAB_MACROS_H
