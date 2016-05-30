/**************************************************************
**  File        : serverAPI.c                                **
**  Version     : 2.7                                        **
**  Created     : 25.04.2016                                 **
**  Last change : 26.05.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#include "internalMacros.h"
#include "commonAPI.h"
#include "serverAPI.h"
#include <stdlib.h>
#include <string.h>
#include "PacketLib.h"

// server socket
static uint8_t initialized = 0;

int init_server()
{
    int enable = 1, broadcast = 1;
	if(initialized)
    {
        return SUCCESS;
    }

    // init socket //
    socketDscp=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (socketDscp < 0)
    {
        /// TODO: initialized = 0; --> sollte man vielleicht hier auch tun?
        return ERROR;
    }

    // initialize my own address structure
    my_addr.sin_family = AF_INET;                               // Ethernet
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);                // automatically insert own address
    my_addr.sin_port = htons(SERVER_PORT);                      // set vslab server port
    memset(&(my_addr.sin_zero), 0x00, 8);                       // set remaining bytes to 0x0

    // initialize target structure --> all information will be populated by recvform() calls
    target_addr.sin_family = AF_INET;           // Ethernet
    //    target_addr.sin_addr.s_addr = htonl(INADDR_ANY); // This is initialized on send
    //    target_addr.sin_port = htons(CLIENT_PORT);       // This is initialized on send
    memset(&(target_addr.sin_zero), 0x00, 8);

    // bind socket
    if (bind(socketDscp, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) < 0)
    {
        shutdown(socketDscp, 2);
        return ERROR;
    }
    setsockopt(socketDscp, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    setsockopt(socketDscp, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));

    initialized = 1;

    return SUCCESS;
}

int deinit_server()
{
    // TODO: deinit_client schreiben und den Socket schließen ;-)
    return SUCCESS;
}

uint8_t send_gp_rsp(uint32_t target_client_ip, uint16_t target_client_port)
{
    msg temp_msg;
    uint8_t error_code;

    if(!initialized) return ERR_NO_INIT;

    temp_msg.header = malloc(sizeof(msg_header));
    if(temp_msg.header == NULL)
    {
        return ERR_ALLOC;
    }
    temp_msg.data = NULL;

    temp_msg.header->func = FNC_GP;
    temp_msg.header->length = 0;
    temp_msg.header->mode = MODE_SERVER;
    temp_msg.header->priority = SERVER_PRIO;
    temp_msg.header->reserved = VALUE_RESERVED;
    temp_msg.header->type = MSG_RESPONSE;
    temp_msg.header->version = PROTOCOL_VERSION;

    error_code = send_msg(&temp_msg,target_client_ip, target_client_port);
    free(temp_msg.header);
    return error_code;
}

uint8_t send_dec_rsp(uint16_t BID, int16_t clientID, uint8_t* data, uint32_t data_len, uint32_t target_client_ip, uint16_t target_client_port)
{
    msg tmp_msg;
    uint8_t error_code;
    uint32_t var;

    if(!initialized) return ERR_NO_INIT;

    tmp_msg.header = malloc(sizeof(msg_header));
    if(tmp_msg.header == NULL)
    {
        return ERR_ALLOC;
    }

    tmp_msg.header->func = FNC_DECRYPT;
    tmp_msg.header->length = sizeof(dat_decrypt_response)+((data_len-1)*sizeof(uint8_t));
    tmp_msg.header->mode = MODE_SERVER;
    tmp_msg.header->priority = SERVER_PRIO;
    tmp_msg.header->reserved = VALUE_RESERVED;
    tmp_msg.header->type = MSG_RESPONSE;
    tmp_msg.header->version = PROTOCOL_VERSION;

    tmp_msg.data = malloc(tmp_msg.header->length);
    if(tmp_msg.data == NULL)
    {
        free(tmp_msg.header);
        return ERR_ALLOC;
    }
    dat_decrypt_response* tmp_data = (dat_decrypt_response*) tmp_msg.data;
    tmp_data->blockID= htons(BID);
    tmp_data->clientID = htons(clientID);

    for (var = 0; var < data_len; var++) {
        (&(tmp_data->firstElement))[var]=data[var];
    }

    error_code = send_msg(&tmp_msg,target_client_ip, target_client_port);
    free(tmp_msg.header);
    free(tmp_msg.data);
    return error_code;
}

uint8_t send_unlock_rsp(uint32_t target_client_ip, uint16_t target_client_port)
{
    msg temp_msg;
    uint8_t error_code;

    if(!initialized) return ERR_NO_INIT;

    temp_msg.header = malloc(sizeof(msg_header));
    if(temp_msg.header == NULL)
    {
        return ERR_ALLOC;
    }
    temp_msg.data = NULL;

    temp_msg.header->func = FNC_UNLOCK;
    temp_msg.header->length = 0;
    temp_msg.header->mode = MODE_SERVER;
    temp_msg.header->priority = SERVER_PRIO;
    temp_msg.header->reserved = VALUE_RESERVED;
    temp_msg.header->type = MSG_RESPONSE;
    temp_msg.header->version = PROTOCOL_VERSION;

    error_code = send_msg(&temp_msg,target_client_ip, target_client_port);
    free(temp_msg.header);
    return error_code;
}

uint8_t send_brdcst_rsp(uint32_t target_client_ip, uint16_t target_client_port)
{
    msg temp_msg;
    uint8_t error_code;

    if(!initialized) return ERR_NO_INIT;

    temp_msg.header = malloc(sizeof(msg_header));
    if(temp_msg.header == NULL)
    {
        return ERR_ALLOC;
    }
    temp_msg.data = NULL;

    temp_msg.header->func = FNC_BROADCAST;
    temp_msg.header->length = 0;
    temp_msg.header->mode = MODE_SERVER;
    temp_msg.header->priority = SERVER_PRIO;
    temp_msg.header->reserved = VALUE_RESERVED;
    temp_msg.header->type = MSG_RESPONSE;
    temp_msg.header->version = PROTOCOL_VERSION;

    error_code = send_msg(&temp_msg,target_client_ip, target_client_port);
    free(temp_msg.header);
    return error_code;
}

uint8_t send_status_rsp(int16_t CID, uint32_t sequence_number, uint32_t target_status_ip, uint16_t target_client_port)
{
    msg temp_msg;
    uint8_t error_code;
    dat_status_response* dat;
    uint32_t target_client_ip = target_status_ip;

    if(!initialized) return ERR_NO_INIT;

    temp_msg.header = malloc(sizeof(msg_header));
    if(temp_msg.header == NULL)
    {
        return ERR_ALLOC;
    }
    dat = malloc(sizeof(dat_status_response));
    if(dat == NULL)
    {
        free(temp_msg.header);
        return ERR_ALLOC;
    }

    temp_msg.header->func = FNC_STATUS;
    temp_msg.header->length = 0;
    temp_msg.header->mode = MODE_SERVER;
    temp_msg.header->priority = SERVER_PRIO;
    temp_msg.header->reserved = VALUE_RESERVED;
    temp_msg.header->type = MSG_RESPONSE;
    temp_msg.header->version = PROTOCOL_VERSION;

    dat->clientID = htons(CID);
    dat->wordCount = htons(sequence_number);
    dat->reserved = VALUE_RESERVED;

    temp_msg.data = (void*) dat;

    error_code = send_msg(&temp_msg,target_client_ip, target_client_port);
    free(temp_msg.header);
    free(temp_msg.data);
    return error_code;
}

uint8_t send_error_rsp(uint8_t err_code, uint32_t blk_ID, FID fid, uint32_t target_client_ip, uint16_t target_client_port)
{
    msg temp_msg;
    uint8_t rsp_error_code;
    error* dat;

    if(!initialized) return ERR_NO_INIT;

    // Only pass relevant error codes to the client
    if((err_code == ERR_INVALIDVERSION) ||
       (err_code == ERR_DECRYPT) ||
       (err_code == ERR_SERVERINUSE) ||
       (err_code == ERR_FUNCTIONEXEC) ||
       (err_code == ERR_FUNCTIONTIMEOUT) ||
       (err_code == ERR_NO_GP) ||
       (err_code == ERR_PACKETLENGTH) ||
       (err_code == ERR_LOCK_TIMEOUT) ||
       (err_code == ERR_NOSUCHFUNCTION) ||
       (err_code == ERR_UNKNOWN))
    {

    }
    else
    {
        return(NO_ERROR);
    }

    temp_msg.header = malloc(sizeof(msg_header));
    if(temp_msg.header == NULL)
    {
        return ERR_ALLOC;
    }
    dat = malloc(sizeof(error));
    if(dat == NULL)
    {
        free(temp_msg.header);
        return ERR_ALLOC;
    }

    switch(fid)
    {
        case GP_REQ:
        case GP_RSP:
            temp_msg.header->func = FNC_GP;
            break;
        case DECRYPT_REQ:
        case DECRYPT_RSP:
            temp_msg.header->func = FNC_DECRYPT;
            break;
        case UNLOCK_REQ:
        case UNLOCK_RSP:
            temp_msg.header->func = FNC_UNLOCK;
            break;
        case BROADCAST_REQ:
        case BROADCAST_RSP:
            temp_msg.header->func = FNC_BROADCAST;
            break;
        default:
            free(temp_msg.header);
            return ERR_NOSUCHFUNCTION;
    }

    temp_msg.header->length = 3;
    temp_msg.header->mode = MODE_SERVER;
    temp_msg.header->priority = SERVER_PRIO;
    temp_msg.header->reserved = VALUE_RESERVED;
    temp_msg.header->type = MSG_ERROR;
    temp_msg.header->version = PROTOCOL_VERSION;

    dat->errCode = err_code;
    if((err_code == ERR_SERVERINUSE) ||(err_code == ERR_DECRYPT))
    {
        dat->blockID = htons(blk_ID);
    }
    else
    {
        dat->blockID = 0;
    }

    temp_msg.data = (void*) dat;

    rsp_error_code = send_msg(&temp_msg,target_client_ip, target_client_port);
    free(temp_msg.header);
    free(temp_msg.data);
    return rsp_error_code;
}

uint8_t extract_gp_req(msg* packet, uint16_t* gp, int16_t* CID, uint8_t* prio)
{
    if(!initialized) return ERR_NO_INIT;

    uint8_t retVal;

    retVal = check_pointers(packet);
    if(retVal != NO_ERROR)
    {
        return retVal;
    }

    *prio = packet->header->priority;
    *gp = htons(((dat_gp_request*)(packet->data))->generator);
    *CID = htons(((dat_gp_request*)(packet->data))->clientID);

    return NO_ERROR;    /* Server IP is extracted by recv_msg() */
}

uint8_t extract_dec_req(msg* packet, int16_t* CID, uint16_t* BID, uint16_t** data, uint32_t* data_len)
{
    if(!initialized) return ERR_NO_INIT;

    uint8_t retVal;
    uint32_t index;

    retVal = check_pointers(packet);
    if(retVal != NO_ERROR)
    {
        return retVal;
    }

    *data = malloc(packet->header->length - (2 * sizeof(uint16_t)));
    if(*data == NULL)
    {
        return ERR_ALLOC;
    }

    *data_len = (packet->header->length - (2 * sizeof(uint16_t))) / sizeof(uint16_t);
    *CID = htons(((dat_decrypt_request*)(packet->data))->clientID);
    *BID = htons(((dat_decrypt_request*)(packet->data))->blockID);

    for(index = 0; index < *data_len; index++)
    {
        (*data)[index] = htons(((uint16_t*)&(((dat_decrypt_request*)(packet->data))->firstElement))[index]);
    }

    return NO_ERROR;    /* Server IP is extracted by recv_msg() */
}

uint8_t extract_unlock_req(msg* packet, int16_t* CID)
{
    if(!initialized) return ERR_NO_INIT;

    uint8_t retVal;

    retVal = check_pointers(packet);
    if(retVal != NO_ERROR)
    {
        return retVal;
    }

    *CID = htons(((dat_unlock_request*)(packet->data))->clientID);

    return NO_ERROR;    /* Server IP is extracted by recv_msg() */
}

uint8_t extract_brdcst_req(msg* packet)
{
    if(!initialized) return ERR_NO_INIT;

    uint8_t retVal;

    retVal = check_pointers(packet);
    if(retVal != NO_ERROR)
    {
        return retVal;
    }

    return NO_ERROR;
}

uint8_t extract_status_req(msg* packet)
{
    if(!initialized) return ERR_NO_INIT;

    uint8_t retVal;

    retVal = check_pointers(packet);
    if(retVal != NO_ERROR)
    {
        return retVal;
    }

    return NO_ERROR;
}

