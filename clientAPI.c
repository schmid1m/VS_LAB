/**************************************************************
**  File        : clientAPI.c                                **
**  Version     : 2.4                                        **
**  Created     : 25.04.2016                                 **
**  Last change : 25.04.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#include "internalMacros.h"
#include "Macros.h"
#include "commonAPI.h"
#include "clientAPI.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

static int16_t clientID 		 = -1;
static uint8_t prio 			 = 255;
static uint32_t broadcastAddress = 0;
static uint8_t initialized 		 = 0;
static struct addrinfo = {AI_PASSIVE; AF_INET; SOCK_UDP; 0; 14;	"CLIENT"; 0; 0};

int init_client(int16_t p_cID, uint8_t p_prio, uint32_t p_bca)
{
	clientID = p_cID;
	prio = p_prio;
	broadcastAddress = p_bca;
	initialized = 1; // true

	// init socket //
	clScktDscp=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (clScktDscp < 0)
	{
		initialized = 0;
		return ERROR;
	}

	// initialize client structure
	client.sin_family = AF_INET;					// Ethernet
	client.sin_addr.s_addr = htonl(INADDR_ANY);		// automatically insert own address
	client.sin_port = htons(0);						// set vslab server port
	memset(&(client.sin_zero), 0x00, 8);		// set remaining bytes to 0x0
	// initialize server structure
	client_target.sin_family = AF_INET;			// Ethernet
	client_target.sin_addr.s_addr = inet_addr(SERVER_UNICAST_ADDRESS);
	client_target.sin_port = htons(SERVER_PORT);
	memset(&(client_target.sin_zero), 0x00, 8);
	// bind socket
	if (bind(clScktDscp, (struct sockaddr *)&client, sizeof(struct sockaddr)) < 0)
	{
		close(clScktDscp);
		initialized = 0;
		return ERROR;
	}

	return SUCCESS;

	// TODO: deinit_client schreiben und den Socket schließen ;-)
}

uint8_t send_gp_req(uint16_t gp, uint32_t target_server_ip)
{
	msg temp_msg;
	uint8_t error_code;

	if(!initialized) return ERR_NO_INIT;

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
    error_code = check_packet(&temp_msg);

	if(error_code != NO_ERROR)
	{
		return error_code;
	}

    error_code = send_msg(&temp_msg,target_server_ip);
	free(temp_msg.header);
	free(temp_msg.data);
	return error_code;
}

uint8_t send_dec_req(uint16_t BID, uint16_t *data, uint32_t data_len, uint32_t target_server_ip)
{
	msg tmp_msg;
	uint8_t error_code;

	if(!initialized) return ERR_NO_INIT;

	tmp_msg.header = malloc(sizeof(msg_header));

	tmp_msg.header->func = FNC_DECRYPT;
	tmp_msg.header->length = sizeof(dat_decrypt_request)+((data_len-1)*sizeof(uint16_t));
	tmp_msg.header->mode = MODE_CLIENT;
	tmp_msg.header->priority = prio;
	tmp_msg.header->reserved = VALUE_RESERVED;
	tmp_msg.header->type = MSG_REQUEST;
	tmp_msg.header->version = PROTOCOL_VERSION;

	tmp_msg.data = malloc(tmp_msg.header->length);
	dat_decrypt_request* tmp_data = (dat_decrypt_request*) tmp_msg.data;
	tmp_data->blockID= BID;
	tmp_data->clientID = clientID;

    for (uint32_t var = 0; var < data_len; var++) {
        (&(tmp_data->firstElement))[var]=data[var];
	}

	//check packet before sending
    error_code = check_packet(&tmp_msg);
	if(error_code != NO_ERROR)
	{
		return error_code;
	}

    error_code = send_msg(&tmp_msg,target_server_ip);
	free(tmp_msg.header);
	free(tmp_msg.data);
	return error_code;

}

uint8_t send_unlock_req(uint32_t target_server_ip)
{
	msg tmp;
	uint8_t retVal;

	if(!initialized) return ERR_NO_INIT;

	// Allocate memory
    tmp.header = (msg_header*) malloc(sizeof(msg_header));
    tmp.data = (dat_unlock_request*) malloc(sizeof(dat_unlock_request));

	// Fill header
    tmp.header->priority = prio;
    tmp.header->version = PROTOCOL_VERSION;
    tmp.header->mode = MODE_CLIENT;
    tmp.header->func = FNC_UNLOCK;
    tmp.header->type = MSG_REQUEST;
    tmp.header->length = sizeof(dat_unlock_request);
    tmp.header->reserved = VALUE_RESERVED;

	// Fill datafield
    ((dat_unlock_request*)tmp.data)->clientID = (int16_t)clientID;

	// Check message
    retVal = check_packet(&tmp);
	if(retVal != NO_ERROR) return retVal;

	// Send out message
    retVal = send_msg(&tmp,target_server_ip);

	// Free memory
    free(tmp.header);
    free(tmp.data);

	return retVal;
}

uint8_t send_brdcst_req()
{
	msg tmp;
	uint8_t retVal;

	if(!initialized) return ERR_NO_INIT;

	// Allocate memory
    tmp.header = (msg_header*) malloc(sizeof(msg_header));

	// Fill header
    tmp.header->priority = prio;
    tmp.header->version = PROTOCOL_VERSION;
    tmp.header->mode = MODE_CLIENT;
    tmp.header->func = FNC_BROADCAST;
    tmp.header->type = MSG_REQUEST;
    tmp.header->length = 0;

	// Check message
    retVal = check_packet(&tmp);
	if(retVal != NO_ERROR) return retVal;

	// Send out message
    retVal = send_msg(&tmp,broadcastAddress);

	// Free memory
    free(tmp.header);

	return retVal;
}

uint8_t extract_gp_rsp(msg* packet)
{
	if(!initialized) return ERR_NO_INIT;

	return NO_ERROR;	/* Nothing to extract here */
}

uint8_t extract_dec_rsp(msg* packet, uint16_t* BID, uint8_t** data, uint32_t* data_len)
{
	if(!initialized) return ERR_NO_INIT;

    if(((dat_decrypt_response*)packet->data)->clientID != clientID) return ERR_NOTFORME;

    *BID = ((dat_decrypt_response*)packet->data)->blockID;
    *data_len = packet->header->length - sizeof(dat_decrypt_response) + 1; // uint8_t firstElement

	// Copy ASCII-Chars
    *data = (uint8_t*)malloc(sizeof(uint8_t) * (*data_len));
    memcpy(*data,&((dat_decrypt_response*)packet->data)->firstElement,*data_len);

	return NO_ERROR;
}

uint8_t extract_unlock_rsp(msg* packet)
{
	if(!initialized) return ERR_NO_INIT;

	return NO_ERROR;	/* Nothing to extract here */
}

uint8_t extract_brdcst_rsp(msg* packet)
{
	if(!initialized) return ERR_NO_INIT;

	return NO_ERROR;	/* Server IP is extracted by recv_msg() */
}

uint8_t extract_error_rsp(msg* packet, uint8_t* error_code, uint16_t* BID)
{
	if(!initialized) return ERR_NO_INIT;

    *error_code = ((error*)packet->data)->errCode;
    *BID = ((error*)packet->data)->blockID;

	return NO_ERROR;
}
