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

    // check for valid pointers
    if((NULL == packet) || (NULL == packet->header))
    {
        return ERR_INVALID_PTR;
    }
    if((NULL == packet->data) && (packet->header->length != 0))
    {
        return ERR_INVALID_PTR;
    }

    // ---- check header fields on their own ----
    // check packet length
    if(packet->header->length > sizeof(msg_header))
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
               ((packet->header->length % 2) != 1))
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

FID get_msg_type(msg* packet)
{
    // check for valid pointers
    if((NULL == packet) || (NULL == packet->header))
    {
        return UNKNOWN;
    }
    if((NULL == packet->data) && (packet->header->length != 0))
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
	msg* recvmsg;
	// allocate header memory is the same for every packet type
	packet->header = calloc(1, sizeof(msg_header));

	// struct to get the source ip
	struct sockaddr *src_addr = malloc(sizeof(struct sockaddr));
	// we deal only with ipv4 but safety first ;-) --> think check is not neccesary
	socklen_t addr_length = sizeof(struct sockaddr);

	// check for valid pointer
	if((NULL == packet) || (NULL == src_ip))
	{
		return ERROR;
	}

	// allocate enough packet buffer
	uint8_t *buffer = malloc(MAX_PACKET_LENGTH);
	if(NULL == buffer)
	{
		return ERR_ALLOC;
	}

	// receive packet
	result = recvfrom(socketDscp, buffer, MAX_PACKET_LENGTH, 0, src_addr, &addr_length);
	if((result == -1) || (result == 0))
	{
		return ERROR;
	}

	// cast packet to take a look into the header
	recvmsg = (msg*)buffer;
	// decide how many memory needs to be allocated
	switch(get_msg_type(recvmsg))
	{
		case GP_REQ:
				packet->data = calloc(1, sizeof(dat_gp_request));
				break;
		case GP_RSP:
				// no response?
				break;
		case DECRYPT_REQ:
				packet->data = calloc(1, sizeof(dat_decrypt_request));
				break;
		case DECRYPT_RSP:
				packet->data = calloc(1, sizeof(dat_decrypt_response));
				break;
		case UNLOCK_REQ:
				packet->data = calloc(1, sizeof(dat_unlock_request));
				break;
		case UNLOCK_RSP:
				// no response?
				break;
		case BROADCAST_REQ:
				// no request data?
				break;
		case BROADCAST_RSP:
				break;
		case STATUS_REQ:
				// no data?
				break;
		case STATUS_RSP:
				packet->data = calloc(1, sizeof(dat_status_response));
				break;
		case UNKNOWN:
				// what to do?
				return ERR_NO_PACKET;
				break;
		case ERROR_RSP:
				break;
		default:
				return ERR_NO_PACKET;
	}


	/* socket-function recv(...);
	 * packet->header = malloc(...);
	 * packet->header->_____ = ______;
	 * packet->data = malloc("abh�ngig von packettype aus header")
	 * *src_ip = "aus socket-funktion"; */

	// was bedeutet die Länge im Header? = HEADER + DATEN? oder versenden wir immer nur 2 Byte decrypted?

	// cleanup
	free(src_addr);
	free(buffer);

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

    target_addr.sin_addr.s_addr = inet_addr(target_ip);
	size_t packet_length = sizeof(msg_header) + packet->header->length;

    uint8_t* bitstream = malloc(packet_length);
    memcpy((void*)bitstream, (void*)packet->header, sizeof(msg_header));
    memcpy((void*)bitstream, (void*)packet->data, packet->header->length);

	/* use socket-function sendto(...) */
    //        sendto(int_fd,buf,size,flags,addr,addr_len)
    if(packet_length != sendto(socketDscp, bitstream, packet_length, 0, target_addr, INET_ADDRSTRLEN)){
        free(bitstream);
        return ERR_SEND_ERROR;
    }

    free(bitstream);
    return NO_ERROR;
}

uint8_t free_msg(msg* packet)
{
    // is packet pointer valid?
    if(NULL == packet)
    {
        return ERROR;
    }
    // is header pointer valid?
    if(NULL == packet->header)
    {
        return ERROR;
    }
    else
    {
        //delete valid header pointer
        free(packet->header);
        packet->header = NULL;
    }
    if(packet->data != NULL)
    {
        free(packet->data);
        packet->data = NULL;
    }
    free(packet);
    packet = NULL;
}
