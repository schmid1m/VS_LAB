#include "myqtsocket.h"

uint8_t buffer[MAX_PACKET_LENGTH];

QUdpSocket MyQtSocket::my_sock;

MyQtSocket::MyQtSocket(QObject *parent) : QObject(parent)
{
    this->my_sock.bind();
}

uint8_t MyQtSocket::Read(msg *packet, uint32_t* addr, uint16_t* port)
{
    int result;
    msg_header* recvMsg;

    // check for valid pointer
    if(NULL == packet)
    {
        DEBUG_PRINTF("API ERROR: The given packet pointer is NULL\n")
        return ERR_INVALID_PTR;
    }
    if(NULL == addr)
    {
        DEBUG_PRINTF("API ERROR: The given IP is NULL\n")
        return ERR_INVALID_PTR;
    }

    packet->header = NULL;
    packet->data = NULL;

    // receive packet
    //result = recvfrom(socketDscp, buffer, MAX_PACKET_LENGTH, 0, (struct sockaddr*)&target_addr, &addr_length);
    QHostAddress host_addr;
    result = my_sock.readDatagram((char*)buffer, MAX_PACKET_LENGTH, &host_addr, port);
    if((result < 0) || (result == 0))
    {
        // cleanup
        DEBUG_PRINTF("API ERROR: Failed to receive a Packet\n")
        return ERR_NO_PACKET;
    }

    *addr = host_addr.toIPv4Address();
//    *src_port = htons(target_addr.sin_port);

    // cast packet to take a look into the header
    recvMsg = (msg_header*)buffer;

    recvMsg->length = htons(recvMsg->length);

    // check for valid length field
    if((unsigned int)result != (sizeof(msg_header) + recvMsg->length))
    {
        DEBUG_PRINTF ("API ERROR: The length of the Packet and the length within the packet dont match.\n")
        DEBUG_PRINTF2("           Length from Socket: %d; Packet length: %lu\n", result, sizeof(msg_header) + recvMsg->length)
        return ERR_PACKETLENGTH;
    }else
    {
        // allocate header memory is the same for every packet type
        packet->header = new msg_header;
        if(packet->header == NULL)
        {
            DEBUG_PRINTF("API ERROR: Could not allocate memory for the msg header\n")
            return ERR_ALLOC;
        }

        // allocate msg data memory
        packet->data = malloc((result - sizeof(msg_header)));
        if(packet->data == NULL)
        {
            DEBUG_PRINTF("API ERROR: Could not allocate memory for the msg data field\n")
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

uint32_t MyQtSocket::Send(uint8_t* data, uint32_t data_len, uint32_t addr, uint16_t port)
{
    QHostAddress rec_addr;
    rec_addr.setAddress(addr);
    return my_sock.writeDatagram((const char*)data, data_len, rec_addr,port);
}
