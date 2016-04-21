#include "PacketLib.h"
#include "Macros.h"
#include "stdlib.h"

/// @brief Send a response to a broadcast from a client
/// @param[out] ERROR if sending message isn't successful otherwise you'll get SUCCESS
/// @param[in] socketdesc socket descriptor of the socket that should be used
/// @param[in] serverSocket pointer to the server socket struct
/// @param[in] clientAddress pointer to the client socket struct
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
	// take a look on how you get ip to ip field
	//memcpy((void*)msgData->serverIP, (const void*)serverSocket->sin_addr, 4);				// copy the 4 Bytes INET address into the data fields, maybe we need htonl() but i don't think so

	// send the packet - example http://beej.us/guide/bgnet/output/html/multipage/sendman.html
	if(-1 == sendto(socketdesc,(void*)msgHeader,(sizeof(msg_header)+sizeof(dat_broadcast_response)),0, (struct sockaddr*)clientAddress, sizeof(struct sockaddr_in))){
		// error
		free((void*)msgHeader);
		return ERROR;
	}else{
		// no error
		free((void*)msgHeader);
		return SUCCESS;
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

