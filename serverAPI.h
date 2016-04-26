/**************************************************************
**  File        : serverAPI.h                                **
**  Version     : 2.4                                        **
**  Created     : 25.04.2016                                 **
**  Last change : 25.04.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#ifndef VSLAB_SERVER_API_H
#define VSLAB_SERVER_API_H

#include "Macros.h"
#include "PacketLib.h"

/// \defgroup server Server Functions
/// API Functions that only apply to the server
/// @{

/// \brief Initiates the lib with the permanent server data
/// \return Error code planed as return value
int init_server(/*socket, Server IP, testserver IP*/);

/// \brief Send a generator polynome response
/// Confirm the successfull setting of the generator polynome
/// @author Simon Lauser
/// \param[in] target_client_ip : the IP address of the target client
/// \return The error code that occurred.
/// @sa macros
uint8_t send_gp_rsp(uint32_t target_client_ip);

/// \brief Send the decrypted data
/// Return the decrypted data to the client
/// @author Simon Lauser
/// \param[in] BID : The Block ID of this Block
/// \param[in] clientID : The Client ID of the requesting client
/// \param[in] data : The data to send
/// \param[in] data_len : The length of the data field
/// \param[in] target_client_ip : the IP address of the target client
/// \return The error code that occurred.
/// @sa macros
uint8_t send_dec_rsp(uint16_t BID, int16_t clientID, uint8_t* data, uint32_t data_len, uint32_t target_client_ip);

/// \brief Send the unlock confirmation
/// @author <Author Name Here>
/// \param[in] target_client_ip : the IP address of the target client
/// \return The error code that occurred.
/// @sa macros
uint8_t send_unlock_rsp(uint32_t target_client_ip);

/// \brief Send a broadcast response
/// @author <Author Name Here>
/// \param[in] target_client_ip : the IP address of the target client
/// \return The error code that occurred.
/// @sa macros
uint8_t send_brdcst_rsp(uint32_t target_client_ip);

/// \brief Send a status response
/// Send the current status to the status script
/// @author <Author Name Here>
/// \param[in] CID : The client ID
/// \param[in] sequence_number : The sequence number for the current client
/// \return The error code that occurred.
/// @sa macros
uint8_t send_status_rsp(uint16_t CID, uint32_t sequence_number);

/// \brief Send an error message
/// @author <Author Name Here>
/// \param[in] err_code : the error that occurred
/// \param[in] BID : The Block ID of this Block
/// \param[in] target_client_ip : the IP address of the target client
/// \param[in] fid : the function ID that was called
/// \return The error code that occurred during excecution.
/// @sa macros
/// @sa FID
uint8_t send_error_rsp(uint8_t err_code, uint32_t BID, uint32_t target_client_ip, FID fid);

/// \brief Extract the generator polynome
/// Extract the generator polynome from the packet
/// @author <Author Name Here>
/// \param[in] packet : the packet to extract
/// \param[out] gp : the generator polynome
/// \param[out] CID : the client ID
/// \param[out] prio : the priority of the client
/// \param[out] src_client_ip : the IP of the source client
/// \return The error code that occurred.
/// @sa macros
uint8_t extract_gp_req(msg* packet, uint16_t* gp, uint16_t* CID, uint8_t* prio, uint32_t* src_client_ip);

/// \brief Extract data to decrypt
/// @author <Author Name Here>
/// \param[in] packet : the packet to extract
/// \param[out] CID : the client ID
/// \param[out] BID : the Block ID of this Block
/// \param[out] data : the data to decrypt
/// \param[out] data_len : the amount of data words to decrypt
/// \param[out] src_client_ip : the IP of the source client
/// \return The error code that occurred.
/// @sa macros
uint8_t extract_dec_req(msg* packet, uint16_t* CID, uint16_t* BID, uint16_t* data, uint32_t* data_len, uint32_t* src_client_ip);

/// \brief Extract the unlock command
/// extract the command to unlock the server
/// @author <Author Name Here>
/// \param[in] packet : the packet to extract
/// \param[out] CID : the client ID
/// \param[out] src_client_ip : the IP of the source client
/// \return The error code that occurred.
/// @sa macros
uint8_t extract_unlock_req(msg* packet, uint16_t* CID, uint32_t* src_client_ip);

/// \brief Extract a broadcast request
/// @author <Author Name Here>
/// \param[in] packet : the packet to extract
/// \param[out] src_client_ip : the IP of the source client
/// \return The error code that occurred.
/// @sa macros
uint8_t extract_brdcst_req(msg* packet, uint32_t* src_client_ip);

/// \brief Extract a status request
/// @author <Author Name Here>
/// \param[in] packet : the packet to extract
/// \return The error code that occurred.
/// @sa macros
uint8_t extract_status_req(msg* packet);

/// @}

#endif // VSLAB_SERVER_API_H
