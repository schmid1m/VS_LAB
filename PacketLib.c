/**************************************************************
**  File        : PacketLib.c                                **
**  Version     : 2.6                                        **
**  Created     : 21.04.2016                                 **
**  Last change : 10.05.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "PacketLib.h"
#include "Macros.h"
#include "internalMacros.h"
#include "commonAPI.h"

int socketDscp = 0;                       // socket descriptor
struct sockaddr_in  my_addr;        // my own address information
struct sockaddr_in  target_addr;    // target address information
uint8_t buffer[MAX_PACKET_LENGTH];

uint8_t check_pointers(msg* packet)
{
    // check for valid pointers
    if((NULL == packet) || (NULL == packet->header))
    {
        return ERR_INVALID_PTR;
    }
    if((NULL == packet->data) && (packet->header->length != 0))
    {
        return ERR_INVALID_PTR;
    }
    return NO_ERROR;
}

uint8_t check_packet(msg* packet)
{
    FID msg_type;
    uint8_t retVal;

    retVal = check_pointers(packet);
    if(retVal != NO_ERROR)
    {
        return retVal;
    }

    // ---- check header fields on their own ----
    // check packet length
    if(packet->header->length > (MAX_PACKET_LENGTH - sizeof(msg_header)))
    {
        return ERR_PACKETLENGTH;
    }
    // check protocol version
    if(packet->header->version != PROTOCOL_VERSION)
    {
        return ERR_INVALIDVERSION;
    }
    // check if mode is valid
    if((packet->header->mode != MODE_STATUS) &&
       (packet->header->mode != MODE_SERVER) &&
       (packet->header->mode != MODE_CLIENT))
    {
        return ERR_INVALIDMODE;
    }
    // check if function is known
    if((packet->header->func != FNC_GP)  &&
       (packet->header->func != FNC_DECRYPT)   &&
       (packet->header->func != FNC_UNLOCK)    &&
       (packet->header->func != FNC_BROADCAST) &&
       (packet->header->func != FNC_STATUS))
    {
        return ERR_NOSUCHFUNCTION;
    }
    // check if type is valid
    if((packet->header->type != MSG_REQUEST)  &&
       (packet->header->type != MSG_RESPONSE) &&
       (packet->header->type != MSG_ERROR))
    {
        return ERR_INVALIDTYPE;
    }
    // ---- Header is consistent ----
    msg_type = get_msg_type(packet);
    // validate func ID
    if(msg_type == UNKNOWN)
    {
        return ERR_HEADER_DATA;
    }
    // Check length
    switch(msg_type)
    {
        case GP_RSP:
        case UNLOCK_RSP:
        case BROADCAST_REQ:
        case BROADCAST_RSP:
        case STATUS_REQ:
            if(packet->header->length != 0)
            {
                return ERR_PACKETLENGTH;
            }
            break;
        case GP_REQ:
            if(packet->header->length != sizeof(dat_gp_request))
            {
                return ERR_PACKETLENGTH;
            }
            break;
        case DECRYPT_REQ:
            if((packet->header->length < sizeof(dat_decrypt_request)) ||
               ((packet->header->length % 2) != 0))
            {
                return ERR_PACKETLENGTH;
            }
            break;
        case DECRYPT_RSP:
            if(packet->header->length < sizeof(dat_decrypt_response))
            {
                return ERR_PACKETLENGTH;
            }
            break;
        case UNLOCK_REQ:
            if(packet->header->length != sizeof(dat_unlock_request))
            {
                return ERR_PACKETLENGTH;
            }
            break;
        case STATUS_RSP:
            if(packet->header->length != sizeof(dat_status_response))
            {
                return ERR_PACKETLENGTH;
            }
            break;
        case ERROR_RSP:
            if(packet->header->length != sizeof(error))
            {
                return ERR_PACKETLENGTH;
            }
            break;
        default:
            return ERR_NOSUCHFUNCTION;
    }
    // check if mode and type info match
    if((packet->header->mode == MODE_STATUS) &&
       (packet->header->type != MSG_REQUEST))
    {
        return ERR_HEADER_DATA;
    }
    else if((packet->header->mode == MODE_CLIENT) &&
            (packet->header->type != MSG_REQUEST))
    {
        return ERR_HEADER_DATA;
    }
    else if((packet->header->mode == MODE_SERVER) &&
            (packet->header->type != MSG_RESPONSE) &&
            (packet->header->type != MSG_ERROR))
    {
        return ERR_HEADER_DATA;
    }
    else
    {
        // Header valid
    }

    // ---- Check data field ----
    switch(msg_type)
    {
        case DECRYPT_REQ:
            if(((dat_decrypt_request*)(packet->data))->clientID < 0)
            {
                return ERR_DATA;
            }
            break;
        case DECRYPT_RSP:
            if(((dat_decrypt_response*)(packet->data))->clientID < 0)
            {
                return ERR_DATA;
            }
            break;
        case UNLOCK_REQ:
            if(((dat_unlock_request*)(packet->data))->clientID < 0)
            {
                return ERR_DATA;
            }
            break;
        case STATUS_RSP:
            if(((dat_status_response*)(packet->data))->clientID < -1)
            {
                return ERR_DATA;
            }
            break;
        default:
            // Cannot check data field (all data are valid)
            break;
    }

    return NO_ERROR;
}

FID get_msg_type(msg* packet)
{
    uint8_t retVal;

    retVal = check_pointers(packet);
    if(retVal != NO_ERROR)
    {
        return UNKNOWN;
    }

    if (packet->header->type == MSG_ERROR)
    {
        return ERROR_RSP;
    }
    else if(packet->header->func == FNC_GP)
    {
        if(packet->header->type == MSG_REQUEST)
        {
            return GP_REQ;
        }
        else
        {
            return GP_RSP;
        }
    }
    else if(packet->header->func == FNC_DECRYPT)
    {
        if(packet->header->type == MSG_REQUEST)
        {
            return DECRYPT_REQ;
        }
        else
        {
            return DECRYPT_RSP;
        }
    }
    else if(packet->header->func == FNC_UNLOCK)
    {
        if(packet->header->type == MSG_REQUEST)
        {
            return UNLOCK_REQ;
        }
        else
        {
            return UNLOCK_RSP;
        }
    }
    else if(packet->header->func == FNC_BROADCAST)
    {
        if(packet->header->type == MSG_REQUEST)
        {
            return BROADCAST_REQ;
        }
        else
        {
            return BROADCAST_RSP;
        }
    }
    else if(packet->header->func == FNC_STATUS)
    {
        if(packet->header->type == MSG_REQUEST)
        {
            return STATUS_REQ;
        }
        else
        {
            return STATUS_RSP;
        }
    }
    return UNKNOWN;
}

uint8_t recv_msg(msg* packet, uint32_t* src_ip)
{
    int result;
    msg_header* recvMsg;

    // check for valid pointer
    if((NULL == packet) || (NULL == src_ip))
    {
        return ERR_INVALID_PTR;
    }

    packet->header = NULL;
    packet->data = NULL;

    // struct to get the source ip
    struct sockaddr_in *src_addr = malloc(sizeof(struct sockaddr_in));
    if(src_addr == NULL)
    {
        return ERR_ALLOC;
    }
    // we deal only with ipv4 but safety first ;-) --> think check is not necessary
    socklen_t addr_length = sizeof(struct sockaddr);

    // receive packet
    result = recvfrom(socketDscp, buffer, MAX_PACKET_LENGTH, 0, (struct sockaddr*)src_addr, &addr_length);
    if((result < 0) || (result == 0))
    {
        // cleanup
        free(src_addr);
        return ERR_NO_PACKET;
    }

    *src_ip = ntohl(src_addr->sin_addr.s_addr);
    free(src_addr);

    // cast packet to take a look into the header
    recvMsg = (msg_header*)buffer;

    recvMsg->length = htons(recvMsg->length);

    // check for valid length field
    if((unsigned int)result != (sizeof(msg_header) + recvMsg->length))
    {
        return ERR_PACKETLENGTH;
    }else
    {
        // allocate header memory is the same for every packet type
        packet->header = calloc(1, sizeof(msg_header));
        if(packet->header == NULL)
        {
            return ERR_ALLOC;
        }

        // allocate msg data memory
        packet->data = malloc((result - sizeof(msg_header)));
        if(packet->data == NULL)
        {
            free(packet->header);
            packet->header = NULL;
            return ERR_ALLOC;
        }
        // copy data
        memcpy(packet->header, (msg_header*)buffer, sizeof(msg_header));
        memcpy(packet->data, &(buffer[sizeof(msg_header)]), recvMsg->length);
    }

    // final packet check
    return check_packet(packet);
}

uint8_t send_msg(msg* packet, uint32_t target_ip)
{
    // check for invalid pointers also done here
    uint8_t ret_val = check_packet(packet);
    if(ret_val != NO_ERROR)
    {
        return ret_val;
    }

    target_addr.sin_addr.s_addr = htonl(target_ip);
    ssize_t packet_length = sizeof(msg_header) + packet->header->length;

    uint8_t* bitstream = malloc(packet_length);
    if(bitstream == NULL)
    {
        return ERR_ALLOC;
    }
    /* copy message to the bitstream */
    memcpy((void*)bitstream, (void*)packet->header, sizeof(msg_header));
    memcpy((void*)&(bitstream[sizeof(msg_header)]), (void*)packet->data, packet->header->length);
    ((msg_header*)bitstream)->length = htons(((msg_header*)bitstream)->length);

    /* use socket-function sendto(...) */
    if(packet_length != sendto(socketDscp, bitstream, packet_length, 0, (struct sockaddr*)&target_addr, sizeof(struct sockaddr))){
        free(bitstream);
        return ERR_SEND_ERROR;
    }

    free(bitstream);
    return NO_ERROR;
}

uint8_t free_msg(msg* packet)
{
    uint8_t retVal;

    retVal = check_pointers(packet);
    if(retVal != NO_ERROR)
    {
        return retVal;
    }

    // delete header pointer
    // valid pointer check is done in check_pointers(packet)
    free(packet->header);
    packet->header = NULL;
    if(packet->data != NULL)
    {
        free(packet->data);
        packet->data = NULL;
    }
    return NO_ERROR;
}
