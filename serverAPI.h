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

int init_server(/*socket, Server IP, testserver IP*/);

uint8_t send_gp_rsp(uint32_t target_client_ip);
uint8_t send_dec_rsp(uint16_t BID, uint8_t* data, uint32_t data_len, uint32_t target_client_ip);
uint8_t send_unlock_rsp(uint32_t target_client_ip);
uint8_t send_brdcst_rsp(uint32_t target_client_ip);
uint8_t send_status_rsp(uint16_t CID, uint32_t sequence_number);
uint8_t send_error_rsp(uint8_t err_code, uint32_t blk_ID, uint32_t target_client_ip, FID fid);

uint8_t extract_gp_req(msg* packet, uint16_t* gp, uint16_t* CID, uint8_t* prio, uint32_t* src_client_ip);
uint8_t extract_dec_req(msg* packet, uint16_t* CID, uint16_t* BID, uint16_t* data, uint32_t* data_len, uint32_t* src_client_ip);
uint8_t extract_unlock_req(msg* packet, uint16_t* CID, uint32_t* src_client_ip);
uint8_t extract_brdcst_req(msg* packet, uint32_t* src_client_ip);
uint8_t extract_status_req(msg* packet);

#endif // VSLAB_SERVER_API_H
