/**************************************************************
**  File        : PacketLib.c                                **
**  Version     : 2.4                                        **
**  Created     : 21.04.2016                                 **
**  Last change : 25.04.2016                                 **
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

/*
int server_broadcast_response(int socketdesc, struct sockaddr_in *serverSocket, struct sockaddr_in *clientAddress){
	msg_header *msgHeader = malloc(sizeof(msg_header)+sizeof(dat_broadcast_response));		// storage for the complete response
	dat_broadcast_response *msgData = (dat_broadcast_response*)(msgHeader+sizeof(msg_header));	// get the pointer to the data fields

	// fill the header
	msgHeader->func = FNC_BROADCAST;
	msgHeader->length = sizeof(msg_header)+sizeof(dat_broadcast_response);
	msgHeader->mode = MODE_SERVER;
	msgHeader->priority = 0;																// highest priority
	msgHeader->type = MSG_RESPONSE;
	msgHeader->version = PROTOCOL_VERSION;

	// fill the data field
	msgData->serverIP = ntohs(clientAddress->sin_addr.s_addr);

	// send the packet - example http://beej.us/guide/bgnet/output/html/multipage/sendman.html
	if(-1 == sendto(socketdesc,(void*)msgHeader,(sizeof(msg_header)+sizeof(dat_broadcast_response)),0, (struct sockaddr*)clientAddress, sizeof(struct sockaddr_in))){
		// error
		free((void*)msgHeader);
		return -1;
	}else{
		// no error
        free((void*)msgHeader);
		return 1;
	}
}


/// @brief get the priority from a client
/// @param[out] priority 0...255
/// @param[in] received data on socket
int get_priority(void *data){
	msg_header *msgHeader = (msg_header*)data;
	return msgHeader->priority;
}

/// @brief get the protocol version of a received packet
/// @param[out] protocol version
/// @param[in] received data on socket
int get_protocol_version(void *data){
	msg_header *msgHeader = (msg_header*)data;
	return msgHeader->version;
}

/// @brief get the mode of a received packet
/// @param[out] mode of received packet
/// @param[in] received data on socket
int get_mode(void *data){
	msg_header *msgHeader = (msg_header*)data;
	return msgHeader->mode;
}
*/


uint8_t check_packet(msg* packet)
{
    FID msg_type;
    // ---- check header fields on their own ----
    // check packet length
    if(packet->header->length > MAX_PACKET_LENGTH)
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
            if(packet->header->length != 4)
            {
                return ERR_PACKETLENGTH;
            }
            break;
        case DECRYPT_REQ:
            if((packet->header->length < 6) ||
               ((packet->header->length % 2) != 1))
            {
                return ERR_PACKETLENGTH;
            }
            break;
        case DECRYPT_RSP:
            if(packet->header->length < 5)
            {
                return ERR_PACKETLENGTH;
            }
            break;
        case UNLOCK_REQ:
            if(packet->header->length != 2)
            {
                return ERR_PACKETLENGTH;
            }
            break;
        case STATUS_RSP:
            if(packet->header->length != 8)
            {
                return ERR_PACKETLENGTH;
            }
            break;
        case ERROR_RSP:
            if(packet->header->length != 3)
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
        return ERR_INVALIDMODE;
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
            break;
    }

    return NO_ERROR;
}

uint8_t recv_msg(msg* packet, uint32_t* src_ip)
{
	/* socket-function recv(...);
	 * packet->header = malloc(...);
	 * packet->header->_____ = ______;
	 * packet->data = malloc("abh�ngig von packettype aus header")
	 * *src_ip = "aus socket-funktion"; */

	return check_packet(packet);
}

FID get_msg_type(msg* packet)
{
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

uint8_t send_msg(msg* packet, uint32_t target_ip)
{
	// check for valid pointer
	if(NULL == packet)
	{
		return -1;
	}

	/* setup network info */
	///	TODO: take the info from ONE CENTRAL library-socket to get the correct IP etc...
	size_t packet_length = sizeof(msg_header) + packet->header->length;
	struct sockaddr_in sa;
	inet_pton(AF_INET, "10.12.110.57", &(sa.sin_addr)); // IPv4
	int client_socket = 0;
	/* use socket-function sendto(...) */
	if(packet_length != sendto(client_socket, packet, packet_length, 0, sa)){
		return -1;
	}

	return SUCCESS;
}
