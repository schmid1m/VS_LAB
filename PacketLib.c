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

//#include <arpa/inet.h>
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
    if((packet->header->func != FNC_POLYNOME)  &&
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
    // Check length
        switch(msg_type)
    // ---- Check data field ----
    // TODO check data field
    return NO_ERROR;
}

uint8_t recv_msg(msg* packet)
{

}

FID get_msg_type(msg* packet)
{
    if (packet->header->type == MSG_ERROR)
    {
        return ERROR_RSP;
    }
    else if(packet->header->func == FNC_POLYNOME)
    {
        if(packet->header->type == MSG_REQUEST)
        {
            return POLYNOME_REQ;
        }
        else
        {
            return POLYNOME_RSP;
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

uint8_t send_msg(msg* packet)
{

}
