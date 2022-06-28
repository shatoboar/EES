#ifndef BLUETOOTHSERVICE_H
#define BLUETOOTHSERVICE_H

#include "stdint.h"

class BluetoothService
{
private:
    uint8_t recv_buf[4];
	uint8_t send_buf[4];
    int s;
    int unmarshal(uint8_t* recv_buf, int bytes_read);
    void marshal(uint8_t* send_buf, uint8_t message_type, uint8_t payload);
    int receive_msg(int socket, uint8_t* recv_buf, uint8_t* send_buf);
    int send_msg(int socket, uint8_t* send_buf, uint8_t* recv_buf, uint8_t message_type, uint8_t payload);
public:
    BluetoothService();
    void CloseConnection();
    int InitRoutine();
    void DeployRoutine();
    void SendClassificationRoutine(int classification);
};


#endif
