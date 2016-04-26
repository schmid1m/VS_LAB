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

static int16_t clientID;
static int16_t prio;

int init_client(/*Client ID,Prio,socket*/)
{

}

uint8_t send_gp_req(uint16_t gp, uint32_t target_server_ip)
{

	msg temp_msg;
	uint8_t error_code;

	temp_msg.header = malloc(sizeof(msg_header));
	temp_msg.data = malloc(sizeof(dat_gp_request));

	temp_msg.header->func = FNC_GP;
	temp_msg.header->length = sizeof(dat_gp_request);
	temp_msg.header->mode = MODE_CLIENT;
	temp_msg.header->priority = prio;
	temp_msg.header->reserved = VALUE_RESERVED;
	temp_msg.header->type = MSG_REQUEST;
	temp_msg.header->version = PROTOCOL_VERSION;

	((dat_gp_request*)temp_msg.data)->clientID = clientID;
	((dat_gp_request*)temp_msg.data)->generator = gp;

	//check packet before sending
	error_code = check_packet(temp_msg);

	if(error_code != NO_ERROR)
	{
		return error_code;
	}

	error_code = send_msg(temp_msg);
	free(temp_msg.header);
	free(temp_msg.data);
	return error_code;
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
