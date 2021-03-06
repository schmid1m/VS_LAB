/**************************************************************
**  File        : clientAPI.c                                **
**  Version     : 2.7                                        **
**  Created     : 25.04.2016                                 **
**  Last change : 26.05.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#include "internalMacros.h"
#include "commonAPI.h"
#include "clientAPI.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int16_t clientID          = -1;
static uint8_t prio              = 255;
static uint32_t broadcastAddress = 0;
static uint8_t initialized       = 0;

int init_client(int16_t p_cID, uint8_t p_prio, uint32_t p_bca)
{
	int enable = 1, broadcast = 1;
    if(initialized)
    {
        return SUCCESS;
    }

    clientID = p_cID;
    prio = p_prio;
    broadcastAddress = p_bca;

    // init socket //
    socketDscp=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (socketDscp < 0)
    {
    	printf("Client: can not get socket\n");
        return ERROR;
    }

    // initialize my socket
    my_addr.sin_family = AF_INET;                       // Ethernet
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);        // automatically insert own address
    my_addr.sin_port = htons(0);                        // set vslab server port
    memset(&(my_addr.sin_zero), 0x00, 8);               // set remaining bytes to 0x0
    // initialize target structure
    target_addr.sin_family = AF_INET;                   // Ethernet
//    target_addr.sin_addr.s_addr = htonl(INADDR_ANY); // This is initialized on send
//    target_addr.sin_port = htons(SERVER_PORT);       // This is initialized on send
    memset(&(target_addr.sin_zero), 0x00, 8);
    // bind socket
    if (bind(socketDscp, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) < 0)
    {
    	printf("Client: can not bind socket\n");
        shutdown(socketDscp, 2);
        return ERROR;
    }
    setsockopt(socketDscp, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    setsockopt(socketDscp, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));

    initialized = 1; // true
    return SUCCESS;
}

int deinit_client()
{
    // TODO: deinit_client schreiben und den Socket schließen ;-)
    return SUCCESS;
}

uint8_t send_gp_req(uint16_t gp, uint32_t target_server_ip, uint16_t target_server_port)
{
    msg temp_msg;
    uint8_t error_code;

    if(!initialized) return ERR_NO_INIT;

    temp_msg.header = malloc(sizeof(msg_header));
    if(temp_msg.header == NULL)
    {
        return ERR_ALLOC;
    }

    temp_msg.data = malloc(sizeof(dat_gp_request));
    if(temp_msg.data == NULL)
    {
        free(temp_msg.header);
        return ERR_ALLOC;
    }

    temp_msg.header->func = FNC_GP;
    temp_msg.header->length = sizeof(dat_gp_request);
    temp_msg.header->mode = MODE_CLIENT;
    temp_msg.header->priority = prio;
    temp_msg.header->reserved = VALUE_RESERVED;
    temp_msg.header->type = MSG_REQUEST;
    temp_msg.header->version = PROTOCOL_VERSION;

    ((dat_gp_request*)temp_msg.data)->clientID = htons(clientID);
    ((dat_gp_request*)temp_msg.data)->generator = htons(gp);

    error_code = send_msg(&temp_msg,target_server_ip, target_server_port);
    free(temp_msg.header);
    free(temp_msg.data);
    return error_code;
}

uint8_t send_dec_req(uint16_t BID, uint16_t *data, uint32_t data_len, uint32_t target_server_ip, uint16_t target_server_port)
{
    msg tmp_msg;
    uint8_t error_code = 0;
    uint32_t var = 0;

    if(!initialized) return ERR_NO_INIT;

    tmp_msg.header = malloc(sizeof(msg_header));
    if(tmp_msg.header == NULL)
    {
        return ERR_ALLOC;
    }

    tmp_msg.header->func = FNC_DECRYPT;
    tmp_msg.header->length = sizeof(dat_decrypt_request)+((data_len-1)*sizeof(uint16_t));
    tmp_msg.header->mode = MODE_CLIENT;
    tmp_msg.header->priority = prio;
    tmp_msg.header->reserved = VALUE_RESERVED;
    tmp_msg.header->type = MSG_REQUEST;
    tmp_msg.header->version = PROTOCOL_VERSION;

    tmp_msg.data = malloc(tmp_msg.header->length);
    if(tmp_msg.data == NULL)
    {
        free(tmp_msg.header);
        return ERR_ALLOC;
    }

    dat_decrypt_request* tmp_data = (dat_decrypt_request*) tmp_msg.data;
    tmp_data->blockID= htons(BID);
    tmp_data->clientID = htons(clientID);

    for (var = 0; var < data_len; var++) {
        (&(tmp_data->firstElement))[var]=htons(data[var]);
    }
    error_code = send_msg(&tmp_msg,target_server_ip, target_server_port);
    free(tmp_msg.header);
    free(tmp_msg.data);
    return error_code;
}

uint8_t send_unlock_req(uint32_t target_server_ip, uint16_t target_server_port)
{
    msg tmp;
    uint8_t retVal;

    if(!initialized) return ERR_NO_INIT;

    // Allocate memory
    tmp.header = (msg_header*) malloc(sizeof(msg_header));
    if(tmp.header == NULL)
    {
        return ERR_ALLOC;
    }
    tmp.data = (dat_unlock_request*) malloc(sizeof(dat_unlock_request));
    if(tmp.data == NULL)
    {
        free(tmp.header);
        return ERR_ALLOC;
    }

    // Fill header
    tmp.header->priority = prio;
    tmp.header->version = PROTOCOL_VERSION;
    tmp.header->mode = MODE_CLIENT;
    tmp.header->func = FNC_UNLOCK;
    tmp.header->type = MSG_REQUEST;
    tmp.header->length = sizeof(dat_unlock_request);
    tmp.header->reserved = VALUE_RESERVED;

    // Fill datafield
    ((dat_unlock_request*)tmp.data)->clientID = htons((int16_t)clientID);

    // Send out message
    retVal = send_msg(&tmp,target_server_ip, target_server_port);

    // Free memory
    free(tmp.header);
    free(tmp.data);

    return retVal;
}

uint8_t send_brdcst_req(uint16_t target_server_port)
{
    msg tmp;
    uint8_t retVal;

    if(!initialized) return ERR_NO_INIT;

    // Allocate memory
    tmp.header = (msg_header*) malloc(sizeof(msg_header));
    if(tmp.header == NULL)
    {
        free(tmp.header);
        return ERR_ALLOC;
    }
    tmp.data = NULL;

    // Fill header
    tmp.header->priority = prio;
    tmp.header->version = PROTOCOL_VERSION;
    tmp.header->mode = MODE_CLIENT;
    tmp.header->func = FNC_BROADCAST;
    tmp.header->type = MSG_REQUEST;
    tmp.header->length = 0;

    // Send out message
    retVal = send_msg(&tmp,broadcastAddress, target_server_port);

    // Free memory
    free(tmp.header);

    return retVal;
}

uint8_t extract_gp_rsp(msg* packet)
{
    if(!initialized) return ERR_NO_INIT;

    uint8_t retVal;

    retVal = check_pointers(packet);
    if(retVal != NO_ERROR)
    {
        return retVal;
    }

    return NO_ERROR;    /* Nothing to extract here */
}

uint8_t extract_dec_rsp(msg* packet, uint16_t* BID, uint8_t** data, uint32_t* data_len)
{
    if(!initialized) return ERR_NO_INIT;

    uint8_t retVal;
    int16_t cid;

    retVal = check_pointers(packet);
    if(retVal != NO_ERROR)
    {
        return retVal;
    }

    cid = htons(((dat_decrypt_response*)packet->data)->clientID);
    if(cid != clientID) return ERR_NOTFORME;

    *BID = htons(((dat_decrypt_response*)packet->data)->blockID);
    *data_len = packet->header->length - sizeof(dat_decrypt_response) + 1; // uint8_t firstElement

    // Copy ASCII-Chars
    *data = (uint8_t*)malloc(sizeof(uint8_t) * (*data_len + 1)); // '\0' string printable
    if(*data == NULL)
    {
        return ERR_ALLOC;
    }
    memcpy(*data,&((dat_decrypt_response*)packet->data)->firstElement,*data_len);

    return NO_ERROR;
}

uint8_t extract_unlock_rsp(msg* packet)
{
    if(!initialized) return ERR_NO_INIT;

    uint8_t retVal;

    retVal = check_pointers(packet);
    if(retVal != NO_ERROR)
    {
        return retVal;
    }

    return NO_ERROR;    /* Nothing to extract here */
}

uint8_t extract_brdcst_rsp(msg* packet)
{
    if(!initialized) return ERR_NO_INIT;

    uint8_t retVal;

    retVal = check_pointers(packet);
    if(retVal != NO_ERROR)
    {
        return retVal;
    }

    return NO_ERROR;    /* Server IP is extracted by recv_msg() */
}

uint8_t extract_error_rsp(msg* packet, uint8_t* error_code, uint16_t* BID)
{
    if(!initialized) return ERR_NO_INIT;

    uint8_t retVal;

    retVal = check_pointers(packet);
    if(retVal != NO_ERROR)
    {
        return retVal;
    }

    *error_code = ((error*)packet->data)->errCode;
    *BID = htons(((error*)packet->data)->blockID);

    return NO_ERROR;
}
