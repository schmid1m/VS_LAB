/**************************************************************
**  File        : Macros.h                                   **
**  Version     : 2.4                                        **
**  Created     : 18.04.2016                                 **
**  Last change : 25.04.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#ifndef VSLAB_MACROS_H
#define VSLAB_MACROS_H

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
#define ERR_NO_PACKET               254 ///< No Packet was on the socket
#define ERR_UNKNOWN                 255 ///< An error occurred that does not match any of the other ones (this should never happen)

// General Definitions
#define ERROR                        -1 ///< An error occurred during excecution
#define SUCCESS                       1 ///< Function ran without problems

// Function ID Definitions
typedef enum{
    POLYNOME_REQ,
    POLYNOME_RSP,
    DECRYPT_REQ,
    DECRYPT_RSP,
    UNLOCK_REQ,
    UNLOCK_RSP,
    BROADCAST_REQ,
    BROADCAST_RSP,
    STATUS_REQ,
    STATUS_RSP,
    ERROR_RSP
}FID;

#endif // VSLAB_MACROS_H
