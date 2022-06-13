#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#define MSG_RESEND 255;
#define ACK 1;
#define INIT 2;
#define DEPLOY_SIGNAL 3;
#define PREDICTED_BUCKET 4;

void printBits(uint8_t num);
int unmarshal(uint8_t* recv_msg, int bytes_read);
void marshal(uint8_t* send_buf, uint8_t message_type, uint8_t payload);

int main(int argc, char **argv)
{

	printf("Starting the program...\n");
	fflush(stdout);
	struct sockaddr_rc addr = { 0 };
	int s, status;
	char dest[18] = "00:16:53:0F:77:72";
	uint8_t recv_buf[4];
	uint8_t send_buf[4];

	// allocate a socket
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	// set the connection parameters (who to connect to)
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t) 1;
	str2ba( dest, &addr.rc_bdaddr );

	// connect to server
	status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
	// send a message
	if(status == 0) {
		printf("Connection successful\n");
		fflush(stdout);
	}

	//read data from the client
	int bytes_read;
	send_buf[0]= 37;
	send_buf[1]= 49;
	send_buf[2]= 51;
	send_buf[3]= 99;

	bytes_read = read(s, recv_buf, sizeof(recv_buf));
	unmarshal(recv_buf, bytes_read);

	if(status==0){
		status = write(s,send_buf, 4);
		printf("Sent!\n");
	}


	if (status < 0) perror("uh oh");

	close(s);
	return 0;
}

void printBits(uint8_t num)
{
	for(int bit=0;bit<(sizeof(uint8_t) * 8); bit++)
	{
		printf("%i ", num & 0x01);
		num = num >> 1;
	}

}

int unmarshal(uint8_t* recv_buf, int bytes_read){
	if (bytes_read != 4){
		printf("Wrong amount of bytes received!\n");
		return -1;	
	}
       	if (recv_buf[0] + recv_buf[2] != 0 || recv_buf[1] + recv_buf[3] != 0){
	       printf("Checksum Error! \n");
	       return -1;
       	}
	if (bytes_read > 0) {
		for (int i = 0; i < bytes_read; i++) {
			printf("[");
			printf("%d", recv_buf[i]);
			printf("]\n");
		}
		printf("%d\n", bytes_read);    
	}
	return recv_buf[0];

}

void marshal(uint8_t* send_buf, uint8_t message_type, uint8_t payload){
	send_buf[0] = message_type;
	send_buf[1] = payload;
	send_buf[2] = ~send_buf[0];
	send_buf[3] = ~send_buf[1];

}
	

