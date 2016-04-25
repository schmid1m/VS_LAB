/**************************************************************
**  File        : clientAPI.c                                **
**  Version     : 2.4                                        **
**  Created     : 25.04.2016                                 **
**  Last change : 25.04.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#include "internalMacros.h"
#include "commonAPI.h"
#include "clientAPI.h"

int init_client(/*Client ID,Prio,socket*/)
{

}

uint8_t send_gp_req(uint16_t gp, uint32_t target_server_ip)
{

}

uint8_t send_dec_req(uint16_t BID, uint16_t *data, uint32_t data_len, uint32_t target_server_ip)
{

}

uint8_t send_unlock_req(uint32_t target_server_ip)
{

}

uint8_t send_brdcst_req()
{

}

uint8_t extract_gp_rsp(msg* packet, uint32_t* src_server_ip)
{

}

uint8_t extract_dec_rsp(msg* packet, uint16_t* BID, uint8_t* data, uint32_t* data_len, uint32_t* src_server_ip)
{

}

uint8_t extract_unlock_rsp(msg* packet, uint32_t* src_client_ip)
{

}

uint8_t extract_brdcst_rsp(msg* packet, uint32_t* src_server_ip)
{

}

uint8_t extract_error_rsp(msg* packet, uint8_t* error_code, uint16_t* BID, uint32_t* src_server_ip)
{

}
