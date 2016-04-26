/**************************************************************
**  File        : clientAPI.h                                **
**  Version     : 2.4                                        **
**  Created     : 25.04.2016                                 **
**  Last change : 25.04.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#ifndef VSLAB_CLIENT_API_H
#define VSLAB_CLIENT_API_H

#include "Macros.h"
#include "PacketLib.h"

/// \defgroup client Client Functions
/// API Functions that only apply to the client
/// @{

/// \brief Initiates the lib with the permanent client data
/// @author Philipp Duller
/// \param p_cID : the client ID
/// \param p_prio : the client priority
/// \param p_bca : the broadcast address
/// \return Error code planed as return value
int init_client(int16_t p_cID, uint8_t p_prio, uint32_t p_bca);

/// \brief Send a generator polynome
/// This function sets a generator polynome to lock a server
/// @author Simon Lauser
/// \param[in] gp : the generator polynome
/// \param[in] target_server_ip : the IP address of the target server
/// \return The error code that occurred.
/// @sa macros
uint8_t send_gp_req(uint16_t gp, uint32_t target_server_ip);

/// \brief Send a decryption request
/// Requests the decryption of a block
/// @author Simon Lauser
/// \param[in] BID : the id of the block to decrypt
/// \param[in] data : the data to decrypt
/// \param[in] data_len : the amount of words in data
/// \param[in] target_server_ip : the IP address of the target server
/// \return The error code that occurred.
/// @sa macros
uint8_t send_dec_req(uint16_t BID, uint16_t *data, uint32_t data_len, uint32_t target_server_ip);

/// \brief Send an unlock request
/// Unlock a connected server
/// @author Philipp Duller
/// \param[in] target_server_ip : the IP address of the target server
/// \return The error code that occurred.
/// @sa macros
uint8_t send_unlock_req(uint32_t target_server_ip);

/// \brief Send a broadcast request
/// @author Philipp Duller
/// \return The error code that occurred.
/// @sa macros
uint8_t send_brdcst_req();

/// \brief Extract a generator polynome response
/// Extract the data from the polynome extract response
/// @author Philipp Duller
/// \param[in] packet : the packet to extract
/// \param[out] src_server_ip : the IP of the sourch server
/// \return The error code that occurred.
/// @sa macros
uint8_t extract_gp_rsp(msg* packet);

/// \brief Extract the decrypted data response
/// This function extracts the decrypted data from the message
/// @author Philipp Duller
/// \param[in] packet : the packet to extract
/// \param[out] BID : the block ID of the decrypted packet
/// \param[out] data : the decrypted data
/// \param[out] data_len : the length of the decrypted data
/// \param[out] src_server_ip : the IP of the sourch server
/// \return The error code that occurred.
/// @sa macros
uint8_t extract_dec_rsp(msg* packet, uint16_t* BID, uint8_t** data, uint32_t* data_len);

/// \brief Extracts the unlock confirmation
/// This extracts the unlock confirmation
/// @author Philipp Duller
/// \param[in] packet : the packet to extract
/// \param[out] src_server_ip : the IP of the sourch server
/// \return The error code that occurred.
/// @sa macros
uint8_t extract_unlock_rsp(msg* packet);

/// \brief This extracts broadcast response
/// @author Philipp Duller
/// \param[in] packet : the packet to extract
/// \param[out] src_server_ip : the IP of the sourch server
/// \return The error code that occurred.
/// @sa macros
uint8_t extract_brdcst_rsp(msg* packet);

/// \brief Extract an error message
/// Extract an error message from a server
/// @author Philipp Duller
/// \param[in] packet : the packet to extract
/// \param[out] error_code : The error code that occurred
/// \param[out] BID : the block ID of the decrypted packet (if present)
/// \param[out] src_server_ip : the IP of the sourch server
/// \return The error code that occurred.
/// @sa macros
uint8_t extract_error_rsp(msg* packet, uint8_t* error_code, uint16_t* BID);

/// @}
#endif // VSLAB_CLIENT_API_H
