#include "chat_client.h"

int setUpUDPServer(int *socketd,struct sockaddr_in *address_info, int port)	{
		
	if ( (*socketd = socket(AF_INET, SOCK_DGRAM, 0))==-1 ) {//socket client gia epikoinonia me ton allo client
			printf("\nSocket creation failure %d\n",errno);	//
			perror("Socket");
			return 1;
	}
	/*
	Apofasisame oti to port gia epikoinonia metaksi 2 clients that ine to epomeno aftou pou xrisimopoiithike
	gia epikoinonia me ton TCP server
	*/
	memset( address_info,0,sizeof(*address_info) ); 
	address_info->sin_family = AF_INET;
	address_info->sin_addr.s_addr = htonl(INADDR_ANY);
	
	INC_PORT:
	++port;	
	
	address_info->sin_port = htons(port);
	
	if ( bind(*socketd,(struct sockaddr*)address_info,sizeof(*address_info))==-1 )	{	
		if ( errno != EADDRINUSE )	{
			printf("\nServer bind failure %d\n",errno);	
			perror("Server:");
			return -1;
		}
		else	{
			printf("\nServer bind failure %d\n",errno);	
			perror("Server:");
			printf("Increasing Port Number");
			goto INC_PORT;
		}
	}
		
	return 0;
}
