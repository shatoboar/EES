#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include "BluetoothService.h"

#define MSG_RESEND 255
#define ACK 1
#define INIT 2
#define DEPLOY_SIGNAL 3
#define PREDICTED_BUCKET 4



	

int BluetoothService::unmarshal(uint8_t *recv_buf, int bytes_read)
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
	return recv_buf[0];
}

void BluetoothService::marshal(uint8_t *send_buf, uint8_t message_type, uint8_t payload)
{
	send_buf[0] = message_type;
	send_buf[1] = payload;
	send_buf[2] = ~send_buf[0];
	send_buf[3] = ~send_buf[1];
}

int BluetoothService::receive_msg(int socket, uint8_t *recv_buf)
{
	int bytes_read = 0;
	while (bytes_read == 0)
	{
		bytes_read = read(socket, recv_buf, sizeof(recv_buf));
		sleep(1);
	}
	int msg_type = unmarshal(recv_buf, bytes_read);
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
int BluetoothService::send_msg(int socket, uint8_t *send_buf, uint8_t message_type, uint8_t payload)
{
	marshal(send_buf, message_type, payload);
	int status = write(socket, send_buf, 4);
	return status;
}

BluetoothService::BluetoothService()
{
	printf("Connecting to NXT...\n");
	struct sockaddr_rc addr = {0};
	int s, status;
	char dest[18] = "00:16:53:0F:77:72";
	//char dest[18] = "00:16:53:0F:82:9F";

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
	}
	else
	{
		perror("Oh no;");

	}
}

void BluetoothService::CloseConnection(){
	close(s);
	printf("Connection Closed\n");
}

int main(int argc, char **argv)
{

	BluetoothService bt;
	/* int status;
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
	*/

	bt.CloseConnection();

	return 0;
}

