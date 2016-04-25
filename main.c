/**************************************************************
**  File        : main.c                                     **
**  Version     : 2.4                                        **
**  Created     : 19.04.2016                                 **
**  Last change : 25.04.2016                                 **
**  Project     : Verteilte Systeme Labor                    **
**************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Macros.h"
#include "PacketLib.h"
#include "internalMacros.h"

int main(){
    // Arbitrary file Input (Data and length)
    uint16_t input[] = { 0xFD32, 0x8CAC, 0x6905, 0x4329 };
    uint32_t length = 8;
    
    // Extract generator polynome
    uint16_t gp = input[0];
    length -= 2;
    
    // Set up a message structure to "send" the generator polynome
    msg my_msg;
    // Alloc Header
    my_msg.header = malloc(sizeof(msg_header));
    // Alloc Data
    my_msg.data = (void*)malloc(sizeof(dat_polynom_request));
    // Convert my_msg.data to its real datatype
    dat_polynom_request* data = (dat_polynom_request*)my_msg.data;
    
    // Fill header with data
    my_msg.header->priority= 0x61;
    my_msg.header->version = 14;
    my_msg.header->mode = MODE_CLIENT;
    my_msg.header->func = FNC_POLYNOME;
    my_msg.header->type = MSG_REQUEST;
    my_msg.header->length = 4;
    my_msg.header->reserved = VALUE_RESERVED;
    
    // Fill data field with ID and generator polynome
    data->clientID = 90;
    data->generator = gp;

	// Create a bitstream to send
    uint32_t *msg_send = malloc(HEADER_LENGTH + my_msg.header->length);
    // Copy the header
    memcpy(msg_send, my_msg.header, HEADER_LENGTH);
    // Copy the data
    memcpy((void*)msg_send + HEADER_LENGTH, data, my_msg.header->length);

	// Free allocated memory
    free(my_msg.header);
    free(my_msg.data);
    
    // "send" data

	// Print a copy of the sent data
    printf("%x\n", *(uint32_t*)((void*)msg_send));
    printf("%x\n", *(uint32_t*)((void*)msg_send + 4));
    printf("%x\n\n\n\a", *(uint32_t*)((void*)msg_send + HEADER_LENGTH));
    
    // "read" the data
    
    // Allocate header memory
    my_msg.header = malloc(sizeof(msg_header));
    // Copy header data
    memcpy(my_msg.header, msg_send, HEADER_LENGTH);

    // Allocate data memory (length taken from data)
    my_msg.data = (void*)malloc(sizeof(my_msg.header->length));
    // Copy data
    memcpy(my_msg.data, ((const void*)msg_send) + HEADER_LENGTH, my_msg.header->length);

    // Print received header data
    printf("header: %x\n",   my_msg.header->priority);
    printf("header: %d\n",   my_msg.header->version);
    printf("header: %x\n",   my_msg.header->mode);
    printf("header: %x\n",   my_msg.header->func);
    printf("header: %x\n",   my_msg.header->type);
    printf("header: %x\n\n", my_msg.header->length);
    
    // Print received Data
    printf("data: %d\n", ((dat_polynom_request*)my_msg.data)->clientID);
    printf("data: %x\n\n", ((dat_polynom_request*)my_msg.data)->generator);
    
    // End Program
    return 0;
}
