#include "linkLayer.h"

char* makeCMDFrame(CMDType cmd){
	char* buf = (char*)malloc(CMDLENGTH * sizeof(char));

	char addr;
	if(ll.type == TRANSMITTER){
		addr = TTOR;
	} else if(ll.type == RECEIVER) {
		addr = RTOT;
	} else {
		printf("ERROR VERIFYING TYPE IN COMMAND BUILDING\n");
		exit(ERROR);
	}
	
	buf[0] = FLAG;
	buf[1] = addr;
	buf[2] = cmd;
	buf[3] = cmd ^ addr;
	buf[4] = FLAG;
	
	return buf;
}

char* makeRESPFrame(RESPType resp){
	char* buf = malloc(CMDLENGTH);

	char addr;
	if(ll.type == TRANSMITTER){
		addr = RTOT;
	} else if(ll.type == RECEIVER){
		addr = TTOR;
	} else {
		printf("ERROR VERIFYING TYPE IN RESPONSE BUILDING\n");
		exit(ERROR);
	}
	
	buf[0] = FLAG;
	buf[1] = addr;
	buf[2] = resp;
	buf[3] = resp ^ addr;
	buf[4] = FLAG;

	return buf;
}

int checkRESP(RESPType cmd, char* resp){
	if(cmd == UA){
		if(resp[0] != FLAG){
			printf("ERROR CHEKING POSITION 0 IN UA RESPONSE\n");
			exit(ERROR);
		}
		if(resp[1] != TTOR){
			printf("ERROR CHEKING POSITION 1 IN UA RESPONSE\n");
			exit(ERROR);
		}
		if(resp[2] != UA){
			printf("ERROR CHEKING POSITION 2 IN UA RESPONSE\n");
			exit(ERROR);
		}
		if(resp[3] != (UA ^ TTOR)){
			printf("ERROR CHEKING POSITION 3 IN UA RESPONSE\n");
			exit(ERROR);
		}
		if(resp[4] != FLAG){
			printf("ERROR CHEKING POSITION 4 IN UA RESPONSE\n");
			exit(ERROR);
		}
		return OK;
	}
}

int checkCMD(CMDType command, char* cmd){
	if(command == DISC){
		if(cmd[0] != FLAG){
			printf("ERROR CHEKING POSITION 0 IN DISC COMMAND\n");
			exit(ERROR);
		}
		if(cmd[1] != RTOT){
			printf("ERROR CHEKING POSITION 1 IN DISC COMMAND\n");
			exit(ERROR);
		}
		if(cmd[2] != DISC){
			printf("ERROR CHEKING POSITION 2 IN DISC COMMAND\n");
			exit(ERROR);
		}
		if(cmd[3] != (DISC ^ RTOT)){
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

int readFrame(){
	char buf[2];
	char c;
	int mstate = 0; // MACHINE STATE
	int index = 0;
	if(ll.type == TRANSMITTER){
		tcflush(ll.fd, TCIFLUSH);
		ll.packet.length = 0;
		while(!getAlarmFlag()){
			if(read(ll.fd, buf, 1)){
				c = buf[0];
				switch(mstate){
					case 0:
						if(c == FLAG){
							index = 0;
							ll.packet.buf[index] = c;
							ll.frame.length = 1;
							index = 1;
							mstate = 1;
						}
						break;
					case 1:
						if(c == TTOR || c == RTOT){
							index = 1;
							ll.packet.buf[index] = c;
							ll.packet.length = 2;
							index = 2;
							mstate = 2;
						} else if(c == FLAG){
							mstate = 1;
						} else {
							mstate = 0;
						}
						break;
					case 2:
						if(c == FLAG){
							mstate = 1;
						} else {
							ll.packet.buf[index] = c;
							ll.frame.length = 3;
							index = 3;
							mstate = 3;
						}
						break;
					case 3:
						if(c == FLAG){
							mstate = 1;
						} else {
							ll.packet.buf[index] = c;
							ll.packet.length = 4;
							index = 4;
							mstate = 4;
						}
						break;
					case 4{
						if(c == FLAG){
							ll.packet.buf[index] = c;
							ll.packet.length = 5;
							return OK;
						} else {
							mstate = 0;
						}
						break;
					}
				}
			}
		}
	} else if(ll.type == RECEIVER){
		tcflush(ll.fd, TCIFLUSH);
		ll.packet.length = 0;
		while(!getAlarmFlag()){
			if(read(ll.fd, buf, 1)){
				c = buf[0];
				switch(mstate){
					case 0:
						if(c == FLAG){
							index = 0;
							ll.packet.buf[index] = c;
							ll.packet.length = 1;
							mstate = 1;
						}
						break;
					case 1:
						if(c == TTOR || c == RTOT){
							index = 1;
							ll.packet.buf[index] = c;
							ll.packet.length = 2;
							mstate = 2;
						} else if(c == FLAG){
							mstate = 1;
						} else {
							mstate = 0;
						}
						break;
					case 2:
						if(c == FLAG){
							mstate = 1;
						} else {
							index = 2;
							ll.packet.buf[index] = c;
							ll.packet.length = 3;
							mstate = 3;
						}
						break;
					case 3:
						if(c == FLAG){
							mstate = 1;
						} else {
							index = 3;
							ll.packet.buf[index] = c;
							ll.packet.length = 4;
							if(ll.state == CONNECTION){
								mstate = 5;
							} else if(ll.state == TRANSMISSION){
								mstate = 4;
							}
						}
						break;
					case 4: // FOR DATA TRANSMISSION
						if(c == FLAG){ // END OF RECEPTION
							index++;
							ll.packet.buf[index] = c;
							ll.packet.length++;
							return OK;
						} else {
							index++;
							ll.packet.buf[index] = c;
							ll.packet.length++;
						}
						break;
					case 5:
						if(c == FLAG){
							index = 4;
							ll.packet.buf[index] = c;
							ll.packet.length = 5;
							return OK;
						}
						break;
				}//switch
			}//if
		}//while
	}//else
	return ERROR;
}

int send(char* buf, int lenght){
	tcflush(ll.fd, TCOFLUSH);

	int sent_size = write(ll.fd, buf, lenght);

	if (sent_size == lenght)
		return OK;
	else
		return ERROR;
}

void llopen(char** argv, int argc){
	alarmSetup();
	alarm(0);
	
	int fd = open_port(argv, argc);
	
	if(fd < 0){
		printf("ERROR OPENING PORT\n");
		exit(ERROR);
	}
	
	printf("PORT OPEN\n");
	
	ll.state = CONNECTION;

	if(ll.type == TRANSMITTER){
		char* setCMD = makeCMDFrame(SET); // MAKE A SET COMMAND	
		int tries = 0;
		while(tries < ATTEMPTS) && ll.state == CONNECTION){
			printf("SENDING SET COMMAND...\n");
			if(send(setCMD, CMDLENGTH) != OK){
				printf("ERROR SENDING SET COMMAND. WILL NOW EXIT\n");
				exit(ERROR);
			} else {
				setAlarmFlag(0);
				alarm(TIMEOUT);
				if(readFrame() == OK){ // SUCESSFULLY RECEIVED UA RESPONSE FROM RECEIVER
					if(checkRESP(UA, ll.packet.buf) == OK){
						printf("CONNECTION ESTABLISHED\n");
						ll.state = TRANSMISSION;
						break;
					}
				}
				printf("TIMEOUT ON GETTING UA RESPONSE...\n");
			}
			tries++;
		}
	} else if(ll.type == RECEIVER){
		printf("WAITING FOR SET COMMAND\n");
		setAlarmFlag(0);
		while(TRUE){
			if(readFrame() == OK){
				if(checkCMD(SET, ll.packet.buf) == OK){
					printf("RECEIVED SET COMMAND\n");
					char* resp = makeRESPFrame(UA);
					printf("SENDING UA RESPONSE...\n");
					if(send(resp, CMDLENGTH) != OK){
						printf("ERROR SENDING UA RESPONSE. WILL NOW EXIT\n");
						exit(ERROR);
					}
					ll.state = TRANSMISSION;
				}
			}
		}
	}
}

int main(int argc, char** argv)
{
	
}
















