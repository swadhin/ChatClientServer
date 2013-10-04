#include <stdio.h>
#include <unistd.h>	//read, write, close
#include <stdlib.h> //strtol
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //inet_pton

#define LISTEN_Q	5

int main(int argc, char** argv)	{
		
		int listen_port, listen_sd, client1_sd, client2_sd;
		unsigned int client_len;	
		//pid_t childpid;	//erase if not used
		struct sockaddr_in serv_addr, client1_addr, client2_addr;
		char str[64];//erase later
		/*
		To ektelesimo kalite stin morfi chat_server -p <server_port>. Elegxoume ton arithmo ton orismaton kai 
		ta orismata pou perniountai pos ine egkira
		*/
		if ( argc!=3 || strcmp( *(argv+1),"-p" )!=0 )	{
			printf("\nUsage: %s -p <server_port>\n",*(argv+0) );
			return 1;
		}
		/*
		To trito orisma ine o arithmos tou port opou tha akouei o server - sinepos ine akeraios. Metatrepoume 
		to string se arithmo me tin strtol() i opoia epistrefi ena dixti ston proto xaraktira pou den ine aritmos.
		Sinepos sosti isagogi ine mono otan aftow o xaraktiras ine '\n' i xaraktira telous '\0'
		*/
		char *p;
		listen_port = strtol( *(argv+2),&p,10 );
		if ( *p != '\n' && *p != '\0' )	{
			printf("\nUsage: %s -p <server_port> ; <server_port> is integer\n",*(argv+0) );
			return 1;
		}
		
		if ( (listen_sd = socket(AF_INET, SOCK_STREAM, 0))==-1 ) {//socket sto opoio o server akouei gia sindeseis
			printf("\nSocket creation failure %d\n",errno);	
			perror("Socket");
			return 1;
		}
		
		memset( &serv_addr,0,sizeof(serv_addr) ); 
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(listen_port);	//prokathorismeno port opou akoui o server apo grammi entolon
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//dexete sindesis se oles tis diepafes	
		//inet_ntop(AF_INET,&serv_addr,str,64);	 //erase them later
		//printf("\n%s\n",str);		
		if ( bind(listen_sd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1 )	{	
			printf("\nServer bind failure %d\n",errno);	
			perror("Server:");
			return 1;
		}
		
		if ( listen(listen_sd,LISTEN_Q) == -1 )	{					
			printf("\nServer listen failure %d\n",errno);	
			perror("Server:");							
			return 1;									
		}
		/*
		O server mpeni se ena atermono loop opou se kathe loop kani 2 accept apo clients kai stelni tis plirofories
		tou enos ston allo oste na epitefxthis tairiasma ana dio clients. Akolouthos klini ta sockets kai perimeni
		tous epomenous dio
		*/
		for (;;)	{
			client_len = sizeof(struct sockaddr_in);
			if ( (client1_sd = accept( listen_sd,(struct sockaddr*)&client1_addr,&client_len ))==-1 )	{
				printf("\nServer accept failure%d\n",errno);
				perror("Server: ");
			}
			if ( (client2_sd = accept( listen_sd,(struct sockaddr*)&client2_addr,&client_len ))==-1 )	{
				printf("Server accept failure%d\n",errno);
				perror("Server: ");
			}
			/*
			Afou exi kani ta 2 accept kai apothikefse tis plirofories ton clients sta structs client1_addr kai 
			client2_addr ta stelni stous clients (client1_addr -> client2_sd, client2_addr -> client1_sd)
			*/
/*1*/			write( client1_sd,&client2_addr,sizeof(client2_addr) );
			close(client1_sd);
/*1*/			write( client2_sd,&client1_addr,sizeof(client1_addr) );
			close(client2_sd);
			inet_ntop(AF_INET,&client1_addr,str,64);
			printf("\nClient1: %s\tPort: %d",str, client1_addr.sin_port);
			inet_ntop(AF_INET,&client2_addr,str,64);
			printf("\nClient2: %s\tPort: %d",str, client2_addr.sin_port);
			printf("\n==================\n");
		}
		close(listen_sd);
		return 0;
}
