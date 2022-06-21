#ifndef CLIENT_H
#define CLIENT_H

int unmarshal(uint8_t* recv_buf, int bytes_read);
void marshal(uint8_t* send_buf, uint8_t message_type, uint8_t payload);
int receive_msg(int socket, uint8_t* recv_buf, uint8_t* send_buf);
int send_msg(int socket, uint8_t* send_buf, uint8_t* recv_buf, uint8_t message_type, uint8_t payload);

int init_bluetooth();
#endif
