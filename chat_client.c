#include <stdio.h>
#include <unistd.h>	//read, write, close
#include <stdlib.h> //strtol
#include <errno.h>
#include <string.h> //memset
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //inet_pton
#include <signal.h>
#include "chat_client.h"

int main(int argc, char** argv)	{
		
		int server_port, local_sd, comm_sd;//, comm_port to comm_port mallon den xriazete
		//pid_t childpid;
		struct sockaddr_in serv_addr, local_addr, remote_addr, address; //
		char str[128];//erase later
		/*
		To ektelesimo kalite stin morfi chat_server -p <server_port>. Elegxoume ton arithmo ton orismaton kai 
		ta orismata pou perniountai pos ine egkira
		*/
		if ( argc!=4 || strcmp( *(argv+1),"-s" )!=0 )	{
			printf("\nUsage: %s -s <server_IP> <server_port>\n",*(argv+0) );
			return 1;
		}
		/*
		To tritoo orisma ine to IP tou server me ton opoio epikoinoni o client. To orisma tha ine se "anagnosimi"
		morfi. Xrisimopoioume tin sinartisi inet_pton() i opoia metatrepi tin "anagnosimi" morfi kai apothikevi 
		to apotelesma sto trito orisma tis sinartisis, edo sto pedio sin.addr tis struct sockaddr_in pou 
		xrisimopoioume gia tin apothikefsi ton stoixoion tou server (IP,port). 
		*/
		memset( &serv_addr,0,sizeof(serv_addr) ); 
		switch( inet_pton( AF_INET,*(argv+2),&(serv_addr.sin_addr) ) )	{
			case -1:	//ipirkse lathos stin ektelesi tis sinartisis
				printf("\ninet_pton error %d\n",errno);
				perror("Error: ");
				return 1;			
				break;
			case 0:		//to orisma den itan egkiri IP diefthinsi
				printf("\n%s: Not valid IP address\n",*(argv+2) );
				return 1;
				break;
			case 1:		//i IP metatrapike epitixos
				break;
			default:	//an kataferi na pai edo tote kati adianoito sinevike kata tin ektelesi tis sinartisis
				printf("\nSomething went terribly wrong :-/\n" );
				return -1;
				break;			
		}
		/*
		To tetarto orisma ine o arithmos tou port opou tha akouei o server kai o client prospathi na enothi
		- sinepos ine akeraios. Metatrepoume to string se arithmo me tin strtol() i opoia epistrefi ena dixti 
		ston proto xaraktira pou den ine aritmos. Sinepos sosti isagogi ine mono otan aftow o xaraktiras ine 
		'\n' i xaraktira telous '\0'
		*/		
		char *p;
		server_port = strtol( *(argv+3),&p,10 );
		if ( *p != '\n' && *p != '\0' )	{
			printf("\nUsage: %s -s <server_IP> <server_port>; <server_port> is integer\n",*(argv+0) );
			return 1;
		}
		serv_addr.sin_port = htons(server_port); //prokathorismeno port pou akoui o server apo grammi entolon	
		serv_addr.sin_family = AF_INET;
		
		if ( (local_sd = socket(AF_INET, SOCK_STREAM, 0))==-1 ) {//socket client gia epikoinonia me server
			printf("\nSocket creation failure %d\n",errno);	
			perror("Socket");
			return 1;
		}
				
		if ( connect(local_sd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1 )	{
			printf("\nClient connect failure %d\n",errno);
			perror("Client:");
			return 1;
		}
		/*
		Afou epitefxthi epikoinonia me ton server o client perimeni ena minima apo ton server to opoio tha ton
		enimeroni me tis plirofories tou defterou client pou enothike me ton server kai tha ksekinisi epiloinonia
		mazi tou. To minima tou server ine ena struct sockaddr_in me tis plirofories tou alloy client pou ine
		enomenos kai o client to apothikevi se mia topiki metavliti tipou struct sockaddr_in
		*/
/*1*/		read( local_sd,&remote_addr,sizeof(remote_addr) );
		/*
		printing address for something I want to check
		memset(str,0,STR_MAXSIZE);
		inet_ntop(AF_INET,&remote_addr,str,64);
		printf("\nSocket: %d\tRemote : %s\tPort:%d",local_sd,str,remote_addr.sin_port);
		close(local_sd);		
		*/
		signal(SIGCHLD,SIG_IGN);
		
		setUpUDPServer(&comm_sd,&local_addr,server_port);
		/*
		printing address for something I want to check
		memset(str,0,STR_MAXSIZE);
		inet_ntop(AF_INET,&local_addr,str,64);
		printf("\nSocket: %d\tLocal : %s\tPort:%d",comm_sd,str,local_addr.sin_port);
		*/
		unsigned int len = sizeof(struct sockaddr);
		if ( fork() == 0 )	{											
			memset(str,0,STR_MAXSIZE);
			strcpy(str,"You are connected to another client. Let the CHAT begin!!!");
			sendto(comm_sd,str,STR_MAXSIZE,0,(struct sockaddr*)&remote_addr,len);
			do	{
				memset(str,0,STR_MAXSIZE);
				printf("\n>>");
				fgets(str,STR_MAXSIZE,stdin);
				sendto(comm_sd,str,STR_MAXSIZE,0,(struct sockaddr*)&remote_addr,len);
			}while( strcmp(str,"QUIT")!=0 );
			close(comm_sd);
			exit(0);			
		}																
		memset(str,0,STR_MAXSIZE);
		recvfrom(comm_sd,str,STR_MAXSIZE,0,(struct sockaddr*)&address,&len);
		printf("\n%s",str);											
		printf("\n");
		do	{
			memset(str,0,STR_MAXSIZE);
			recvfrom(comm_sd,str,STR_MAXSIZE,0,(struct sockaddr*)&address,&len);
			printf("\n%s",str);
		}while( strcmp(str,"QUIT")!=0 );
		
		close(comm_sd);
		return 0;			
}
