#ifndef CLIENT_H
#define CLIENT_H

void printBits(uint8_t num);
int unmarshal(uint8_t* recv_buf, int bytes_read);
void marshal(uint8_t* send_buf, uint8_t message_type, uint8_t payload);
void receive_msg(int socket, uint8_t* recv_buf);
void send_msg(int socket, uint8_t* send_buf, uint8_t message_type, uint8_t payload);


#endif
