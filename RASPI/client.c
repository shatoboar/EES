#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

void printBits(uint8_t num);

int main(int argc, char **argv)
{

    printf("Starting the program...\n");
    fflush(stdout);
    struct sockaddr_rc addr = { 0 };
    int s, status;
    char dest[18] = "00:16:53:0F:77:72";

    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba( dest, &addr.rc_bdaddr );

    // connect to server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    // send a message
    if( status == 0 ) {
        printf("Connection successful\n");
        fflush(stdout);
    }

    //read data from the client
    uint8_t buf[4];
    int bytes_read;
    uint8_t send_buf[10];
    send_buf[0]=37;
    send_buf[1]=49;
    send_buf[2]=51;
    send_buf[3]=99;

    bytes_read = read(s, buf, sizeof(buf));
    if( bytes_read > 0 ) {
	for (int i = 0; i < bytes_read; i++) {
		printf("[");
		printf("%d", buf[i]);
		printf("]\n");
	}
	printf("%d\n", bytes_read);    
    }
    if(status==0){
    	status = write(s,send_buf, 10);
	printf("sent!!!");
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
