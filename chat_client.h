#ifndef CHAT_CLIENT
#define CHAT_CLIENT

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define STR_MAXSIZE	128

	int setUpUDPServer(int*,struct sockaddr_in*,int);
	

#endif
