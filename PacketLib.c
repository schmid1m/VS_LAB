#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "PacketLib.h"
#include "Macros.h"
#include "stdlib.h"

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
<<<<<<< HEAD
	// take a look on how you get ip to ip field
	//memcpy((void*)msgData->serverIP, (const void*)serverSocket->sin_addr, 4);				// copy the 4 Bytes INET address into the data fields, maybe we need htonl() but i don't think so
=======
    memcpy((void*)msgData->serverIP, &(serverSocket->sin_addr), 4);							// copy the 4 Bytes INET address into the data fields, maybe we need htonl() but i don't think so
>>>>>>> devel

	// send the packet - example http://beej.us/guide/bgnet/output/html/multipage/sendman.html
	if(-1 == sendto(socketdesc,(void*)msgHeader,(sizeof(msg_header)+sizeof(dat_broadcast_response)),0, (struct sockaddr*)clientAddress, sizeof(struct sockaddr_in))){
		// error
<<<<<<< HEAD
		free((void*)msgHeader);
		return ERROR;
	}else{
		// no error
		free((void*)msgHeader);
=======
        free((void*)msgHeader);
		return ERROR;
	}else{
		// no error
        free((void*)msgHeader);
>>>>>>> devel
		return SUCCESS;
	}
}

<<<<<<< HEAD
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

=======
int check_packet(msg packet)
{
    // check packet length
    if(packet.header->length > MAX_PACKET_LENGTH)
    {
        return ERR_PACKETLENGTH;
    }
    // check protocol version
    if(packet.header->version != PROTOCOL_VERSION)
    {
        return ERR_INVALIDVERSION;
    }
    // check if mode is valid
    if((packet.header->mode != MODE_STATUS) &&
       (packet.header->mode != MODE_SERVER) &&
       (packet.header->mode != MODE_CLIENT))
    {
        return ERR_INVALIDMODE;
    }
    // check if function is known
    if((packet.header->func != FNC_POLYNOME)  &&
       (packet.header->func != FNC_DECRYPT)   &&
       (packet.header->func != FNC_UNLOCK)    &&
       (packet.header->func != FNC_BROADCAST) &&
       (packet.header->func != FNC_STATUS))
    {
        return ERR_NOSUCHFUNCTION;
    }
    // check if type is valid
    if((packet.header->type != MSG_REQUEST)  &&
       (packet.header->type != MSG_RESPONSE) &&
       (packet.header->type != MSG_ERROR))
    {
        return ERR_INVALIDTYPE;
    }
    // TODO check if Header is consistent
    // TODO check data field
    return NO_ERROR;
}
>>>>>>> devel
