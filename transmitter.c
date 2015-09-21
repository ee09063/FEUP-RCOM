#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1
#define FALSE 0
#define TRUE 1

#define ERROR -1
#define OK 0

#define CMDLENGTH 5

#define	FLAG 0x7e

typedef enum {
	TTOR = 0x03, // CMD -> T TO R | RESP -> R TO T
	RTOT = 0x01 // CMD -> R TO T | RESP -> T TO R
} AddrField;

typedef enum {
	SET = 0x03,
	DISC = 0x0b
} CMDType;

typedef enum {
	UA = 0x07,
	RR = 0x05,
	REJ = 0x01
} RESPType

typedef enum {
	CONNECT,
	DISCONNECT
} State;

volatile int STOP = FALSE;

struct termios oldtio;

int open_port(char** argv, int argc){
	struct termios newtio;

	int fd;

	if ((argc < 2) ||
		((strcmp("/dev/ttyS0", argv[1]) != 0) &&
		(strcmp("/dev/ttyS1", argv[1]) != 0))) {
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR | O_NOCTTY);

	if (fd <0){
		perror(argv[1]);
		exit(ERROR);
	}

	if (tcgetattr(fd, &oldtio) == -1) {
		perror("tcgetattr");
		exit(-1);
	}

	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	newtio.c_lflag = 0;

	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 0;

	tcflush(fd, TCIOFLUSH);

	if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
		perror("tcsetattr");
		exit(ERROR);
	}

	printf("New termios structure set\n");

	return fd;
}

int close_port(int fd){
	if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
		perror("tcsetattr");
		exit(ERROR);
	}

	close(fd);
}

int send(char* buf, int lenght, int fd){
	tcflush(fd, TCOFLUSH);

	int sent_size = write(fd, buf, lenght);

	if (sent_size == lenght)
		return OK;
	else
		return ERROR;
}

char* makeCMDFrame(CMDType cmd){
	char* buf = malloc(CMDLENGTH);

	buf[0] = FLAG;
	buf[1] = TTOR;
	buf[2] = cmd;
	buf[3] = cmd ^ TTOR;
	buf[4] = FLAG;

	return buf;
}

int readFrame(State state){
	if(State == CONNECTION)
}

void llopen(char** argv, int argc){
	int fd = open_port(argv, argc);
	
	if(fd < 0){
		printf_s("ERROR OPENING PORT\n");
		exit(ERROR);
	}
	
	print_s("PORT OPEN\n");
	
	char* buf = makeCMDFrame(SET); // MAKE A SET COMMAND	
	
	printf_s("SENDING SET COMMAND...\n");
	
	if(send(buf, CMDLENGTH, fd) != OK){
		print_s("ERROR SENDING SET COMMAND. WILL NOW EXIT\n");
		exit(ERROR);
	} else {
		
	}
	
	
	
	
}

int main(int argc, char** argv)
{
	

	if (buf){
		for (int i = 0; i < CMDLENGTH; i++){
			printf_s("Position %d --> %x\n", i, buf[i]);
		}
	}
	else{
		printf_s("Error Creating Command");
		exit(ERROR);
	}

	return OK;
}
