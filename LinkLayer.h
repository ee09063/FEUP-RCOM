#ifndef _SERIAL_PORT_DRIVER_H_
#define _SERIAL_PORT_DRIVER_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>

#include "alarm.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1
#define FALSE 0
#define TRUE 1

#define ERROR -1
#define OK 0

#define CMDLENGTH 5
#define ATTEMPTS 3
#define TIMEOUT 3
#define MAX_FRAME_SIZE	256

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

#define TRANSMITTER 0
#define RECEIVER 1

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
	CONNECTION,
	TERMINATION,
	TRANSMISSION
} State;

volatile int STOP = FALSE;

typedef struct{
	int fd;
	int type; //TRANSMITTER OR RECEIVER
	
	State state;
	Packet packet;
} linkLayer;

typedef struct {
	char buf[MAX_FRAME_SIZE];
	int length;
} Packet;

struct termios oldtio;
static linkLayer ll;

int openPort(char** argv, int argc);
int closePort();

int send(char* buf, int lenght);

char* makeCMDFrame(CMDType cmd);
char* makeRESPFrame(RESPType resp);

int checkRESP(RESPType cmd, char* resp);
int checkCMD(CMDType command, char* cmd);

int readFrame();














#endif