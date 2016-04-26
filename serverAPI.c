/**************************************************************
**  File        : serverAPI.c                                **
**  Version     : 2.4                                        **
**  Created     : 25.04.2016                                 **
**  Last change : 25.04.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#include "internalMacros.h"
#include "commonAPI.h"
#include "serverAPI.h"

static prio = 255;

int init_server(/*socket, Server IP, testserver IP*/)
{
	return SUCCESS;
}

uint8_t send_gp_rsp(uint32_t target_client_ip)
{
	msg temp_msg;
	uint8_t error_code;

	temp_msg.header = malloc(sizeof(msg_header));
	temp_msg.data = NULL;

	temp_msg.header->func = FNC_GP;
	temp_msg.header->length = 0;
	temp_msg.header->mode = MODE_SERVER;
	temp_msg.header->priority = prio;
	temp_msg.header->reserved = VALUE_RESERVED;
	temp_msg.header->type = MSG_RESPONSE;
	temp_msg.header->version = PROTOCOL_VERSION;

	//check packet before sending
	error_code = check_packet(&temp_msg);

	if(error_code != NO_ERROR)
	{
		return error_code;
	}

	error_code = send_msg(&temp_msg,target_client_ip);
	free(temp_msg.header);
	return error_code;
}

uint8_t send_dec_rsp(uint16_t BID, int16_t clientID, uint32_t target_client_ip, uint8_t* data, uint32_t data_len)
{
	msg tmp_msg;
	uint8_t error_code;

	tmp_msg.header = malloc(sizeof(msg_header));

	tmp_msg.header->func = FNC_DECRYPT;
	tmp_msg.header->length = sizeof(dat_decrypt_response)+((data_len-1)*sizeof(uint8_t));
	tmp_msg.header->mode = MODE_SERVER;
	tmp_msg.header->priority = prio;
	tmp_msg.header->reserved = VALUE_RESERVED;
	tmp_msg.header->type = MSG_RESPONSE;
	tmp_msg.header->version = PROTOCOL_VERSION;

	tmp_msg.data = malloc(tmp_msg.header->length);
	dat_decrypt_response* tmp_data = (dat_decrypt_response*) tmp_msg.data;
	tmp_data->blockID= BID;
	tmp_data->clientID = clientID;

	for (uint32_t var = 0; var < data_len; var++) {
		(&(tmp_data->firstElement))[var]=data[var];
	}

	//check packet before sending
	error_code = check_packet(tmp_msg);
	if(error_code != NO_ERROR)
	{
		return error_code;
	}

	error_code = send_msg(tmp_msg,target_client_ip);
	free(tmp_msg.header);
	free(tmp_msg.data);
	return error_code;
}

uint8_t send_unlock_rsp(uint32_t target_client_ip)
{
	return SUCCESS;
}

uint8_t send_brdcst_rsp(uint32_t target_client_ip)
{
	return SUCCESS;
}

uint8_t send_status_rsp(uint16_t CID, uint32_t sequence_number)
{
	return SUCCESS;
}

uint8_t send_error_rsp(uint8_t err_code, uint32_t blk_ID, uint32_t target_client_ip, FID fid)
{
	return SUCCESS;
}

uint8_t extract_gp_req(msg* packet, uint16_t* gp, uint16_t* CID, uint8_t* prio, uint32_t* src_client_ip)
{
	return SUCCESS;
}

uint8_t extract_dec_req(msg* packet, uint16_t* CID, uint16_t* BID, uint16_t* data, uint32_t* data_len, uint32_t* src_client_ip)
{
	return SUCCESS;
}

uint8_t extract_unlock_req(msg* packet, uint16_t* CID, uint32_t* src_client_ip)
{
	return SUCCESS;
}

uint8_t extract_brdcst_req(msg* packet, uint32_t* src_client_ip)
{
	return SUCCESS;
}

uint8_t extract_status_req(msg* packet)
{
	return SUCCESS;
}

