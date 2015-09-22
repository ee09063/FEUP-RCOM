/*#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>*/
#include <stdio.h>
#include <stdlib.h>

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
	SET = 0x07,
	DISC = 0x0b
} CMDType;

typedef enum {
	UA = 0x03,
	RR = 0x01,
	REJ = 0x05
} RESPType;

typedef enum {
	CONNECT,
	DISCONNECT
} State;

volatile int STOP = FALSE;

State globalState;
/*
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
*/
char* makeCMDFrame(CMDType cmd){
	char* buf = malloc(CMDLENGTH);

	buf[0] = FLAG;
	buf[1] = RTOT;
	buf[2] = cmd;
	buf[3] = cmd ^ RTOT;
	buf[4] = FLAG;

	return buf;
}

char* makeRESPFrame(RESPType resp){
	char* buf = malloc(CMDLENGTH);

	buf[0] = FLAG;
	buf[1] = TTOR;
	buf[2] = resp;
	buf[3] = resp ^ TTOR;
	buf[4] = FLAG;

	return buf;
}

int checkCMD(CMDType command, char* cmd){
	if(command == SET){
		if(cmd[0] != FLAG){
			printf("ERROR CHEKING POSITION 0 IN SET COMMAND\n");
			exit(ERROR);
		}
		if(cmd[1] != TTOR){
			printf("ERROR CHEKING POSITION 1 IN SET COMMAND\n");
			exit(ERROR);
		}
		if(cmd[2] != SET){
			printf("ERROR CHEKING POSITION 2 IN SET COMMAND\n");
			exit(ERROR);
		}
		if(cmd[3] != (SET ^ TTOR)){
			printf("ERROR CHEKING POSITION 3 IN SET COMMAND\n");
			exit(ERROR);
		}
		if(cmd[4] != FLAG){
			printf("ERROR CHEKING POSITION 4 IN SET COMMAND\n");
			exit(ERROR);
		}
		if(cmd[5] == 0x00){
			return OK;
		} else {
			printf("ERROR CHECKING LAST POSITION IN SET COMMAND\n");
			exit(ERROR);
		}
	} else if(command == DISC){
	    if(cmd[0] != FLAG){
			printf("ERROR CHEKING POSITION 0 IN DISC COMMAND\n");
			exit(ERROR);
		}
		if(cmd[1] != TTOR){
			printf("ERROR CHEKING POSITION 1 IN DISC COMMAND\n");
			exit(ERROR);
		}
		if(cmd[2] != DISC){
			printf("ERROR CHEKING POSITION 2 IN DISC COMMAND\n");
			exit(ERROR);
		}
		if(cmd[3] != (DISC ^ TTOR)){
			printf("ERROR CHEKING POSITION 3 IN DISC COMMAND\n");
			exit(ERROR);
		}
		if(cmd[4] != FLAG){
			printf("ERROR CHEKING POSITION 4 IN DISC COMMAND\n");
			exit(ERROR);
		}
		if(cmd[5] == 0x00){
			return OK;
		} else {
			printf("ERROR CHECKING LAST POSITION IN DISC COMMAND\n");
			exit(ERROR);
		}
	}
}

int checkRESP(RESPType cmd, char* resp){
	if(cmd == UA){
		if(resp[0] != FLAG){
			printf("ERROR CHEKING POSITION 0 IN UA RESPONSE\n");
			exit(ERROR);
		}
		if(resp[1] != RTOT){
			printf("ERROR CHEKING POSITION 1 IN UA RESPONSE\n");
			exit(ERROR);
		}
		if(resp[2] != UA){
			printf("ERROR CHEKING POSITION 2 IN UA RESPONSE\n");
			exit(ERROR);
		}
		if(resp[3] != (UA ^ RTOT)){
			printf("ERROR CHEKING POSITION 3 IN UA RESPONSE\n");
			exit(ERROR);
		}
		if(resp[4] != FLAG){
			printf("ERROR CHEKING POSITION 4 IN UA RESPONSE\n");
			exit(ERROR);
		}
		if(resp[5] == 0x00){
			return OK;
		} else {
			printf("ERROR CHECKING LAST POSITION IN UA RESPONSE\n");
			exit(ERROR);
		}
	}
}

/*
int readFrame(State state, int fd){
	char c;
	char buf[2];
	
	if(State == CONNECTION){ // WAITS FOR SET
		tcflush(fd, TCIFLUSH);
		int readState = 0;
		char cmd[CMDLENGTH];
		while(readState != 5){ // STATE MACHINE TO READ 5 FRAME INPUT
			if(read(fd, buf, 1)){
				c = buf[0];
				switch(readState){
					case 0:
						if(c == FLAG){
							cmd[0] = c;
							readState = 1;
						}
						break;
					case 1:
						if(c = TTOR){
							cmd[1] = c;
						}
						else if(c == FLAG){
							readState = 1;
						} else {
							readState = 0;
						}
						break;
					case 2:
						if(c == FLAG){
							readState = 1;
						} else {
							cmd[2] = c;
							readState = 3;
						}
						break;
					case 3:
						if(c == FLAG){
							readState = 1;
						} else {
							cmd[3] = c;
							readState = 4;
						}
						break;
					case 4:
						if(c == FLAG){
							cmd[4] == c;
							readState = 5; // FINAL STATE
						}
						break;
				}
			}
		}
		//READ 5 CHARACTERS, STARTING AND ENDEDING WITH FLAG, CHECK FOR ERRORS
		if(checkCMD(SET, cmd)){ // EXPECTING UA RESPONSE
			return OK;
		} else {
			printf("ERROR CHECKING SET COMMAND\n");
			exit(ERROR);
		}
	}
}

void llopen(char** argv, int argc){
	int fd = open_port(argv, argc);
	
	if(fd < 0){
		printf("ERROR OPENING PORT\n");
		exit(ERROR);
	}
	
	printf("PORT OPEN\n");

	globalState = CONNECTION;
	
	if(readFrame(SET)){
		char* buf = makeRESPFrame(UA); // MAKE A UA RESPONSE	
	
		printf("SENDING UA RESPONSE...\n");
		
		if(send(buf, CMDLENGTH, fd) != OK){
			printf("ERROR SENDING UA RESPONSE. WILL NOW EXIT\n");
			exit(ERROR);
		}
	}
}
*/
int main(int argc, char** argv)
{
	return OK;
}
