
#include <stdio.h> 
#include <stdlib.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 





//////client /////////




int main(int argc, char * argv[]){















//configure: get the Port number and IP address and connect the client to the Server ./WTF 
	 char *address;
	 address = argv[1];

	//creating socket
	int net_socket =0;
	net_socket =socket(AF_INET,SOCK_STREAM,0);// we put 0 in third arguement because we are using the default protocal TCP

	//now we need to connect to server


	//specify an adress  for the socket
	struct sockaddr_in Server_addr;
	
	Server_addr.sin_family = AF_INET;
	Server_addr.sin_port = htons(PORT);
	//this converts the string address from command line and converts into a format for our address structure so we connect properly
	inet_aton(address, &Server_addr.sin_addr.s_addr)

	int connection_stat =connect(net_socket,(struct sockaddr*) &Server_addr,sizeof(Server_addr));

	if(connection_stat ==-1){
		printf("there is an error to connect the socket to the server");
	}




	return 0;
}