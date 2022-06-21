#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include "client.h"

#define MSG_RESEND 255
#define ACK 1
#define INIT 2
#define DEPLOY_SIGNAL 3
#define PREDICTED_BUCKET 4

int main(int argc, char **argv)
{
	uint8_t recv_buf[4];
	uint8_t recv_buf_2[4];
	uint8_t send_buf[4];
	int s = init_bluetooth();
	int status;
	int msg_type;
	if (s != -1)
	{
		uint8_t message_type = INIT;
		uint8_t payload = 0;
		status = send_msg(s, send_buf, message_type, payload);
		printf("Sent!\n");
		msg_type = receive_msg(s, recv_buf);
		if (msg_type != ACK)
		{
			printf("Expected ACK but got something else back\n");
		}
		msg_type = receive_msg(s, recv_buf_2);
		if (msg_type != INIT)
		{
			printf("Protocol Breach, expected INIT command\n");
		}
		else
		{
			printf("We got %d buckets \n", recv_buf_2[1]);
			message_type = ACK;
			payload = 0;
			status = send_msg(s, send_buf, message_type, payload);
		}
	}

	// read data from the client

	if (status < 0)
		perror("uh oh");

	close(s);

	return 0;
}

int unmarshal(uint8_t *recv_buf, int bytes_read)
{
		if (bytes_read != 4)
	{
		printf("Wrong amount of bytes received! %d Bytes read! \n", bytes_read);
		return -1;
	}
	if (bytes_read > 0)
	{
		for (int i = 0; i < bytes_read; i++)
		{
			printf("[");
			printf("%d", recv_buf[i]);
			printf("]\n");
		}
	}


	if ((uint8_t)recv_buf[0] != (uint8_t)~recv_buf[2] || (uint8_t)recv_buf[1] != (uint8_t)~recv_buf[3])
	{
		printf("%d \n", ~recv_buf[0]);
		printf("%d \n", ~recv_buf[2]);
		printf("Checksum Error! \n");
		return -1;
	}
	int msg_type = recv_buf[0];
	switch (msg_type)
	{
	case ACK:
		printf("ACK received! \n");
		break;
	case INIT:
		printf("Init Signal received! \n");
		break;
	case DEPLOY_SIGNAL:
		printf("Deploy Signal received! \n");
		break;
	case PREDICTED_BUCKET:
		printf("Predicted Bucket received! \n");
		break;
	case MSG_RESEND:
		printf("MSG resend received! \n");
		break;
	default:
		printf("Error while receiving message \n");
		msg_type = -1;
		break;
	}
	return msg_type;
}

void marshal(uint8_t *send_buf, uint8_t message_type, uint8_t payload)
{
	send_buf[0] = message_type;
	send_buf[1] = payload;
	send_buf[2] = ~send_buf[0];
	send_buf[3] = ~send_buf[1];
}

int receive_msg(int socket, uint8_t *recv_buf, uint8_t *send_buf)
{
	int bytes_read = 0;
	int status =0;
	while (bytes_read == 0)
	{
		bytes_read = read(socket, recv_buf, sizeof(recv_buf));
		sleep(1);
	}
	int msg_type = unmarshal(recv_buf, bytes_read);
	if(msg_type == -1){
		marshal(send_buf, MSG_RESEND, 0);
	}
	else{
		marshal(send_buf, ACK, 0);
	}
	
	status = write(socket, send_buf, 4);
	
}
int send_msg(int socket, uint8_t *send_buf, uint8_t *recv_buf, uint8_t message_type, uint8_t payload)
{
	marshal(send_buf, message_type, payload);
	int status = write(socket, send_buf, 4);
	int bytes_read = 0;
	while (bytes_read == 0)
	{
		bytes_read = read(socket, recv_buf, sizeof(recv_buf));
		sleep(1);
	}
	int msg_type = unmarshal(recv_buf, bytes_read);
	if(msg_type==ACK){
		return status;		
	}
	else if(msg_type==MSG_RESEND){
		send_msg(socket, send_buf, recv_buf, message_type, payload);
		return status;
	}
	else{
		printf("Unexpected Message type received, expected ack.\n");
		return -1;
	}


}

int init_bluetooth()
{
	printf("Connecting to NXT...\n");
	struct sockaddr_rc addr = {0};
	int s, status;
	// char dest[18] = "00:16:53:0F:77:72";
	char dest[18] = "00:16:53:0F:82:9F";

	// allocate a socket
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	// set the connection parameters (who to connect to)
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t)1;
	str2ba(dest, &addr.rc_bdaddr);

	// connect to server

	status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
	if (status == 0)
	{
		printf("Connection successful\n");
		return s;
	}
	else
	{
		printf("Connection Error\n");
		return -1;
	}
}
