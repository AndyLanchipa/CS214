#include <stdio.h> 
#include <stdlib.h>
#include <sys/socket.h> 
#include <unistd.h> 
#include <string.h> 
#include <signal.h>
#include <netinet/in.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>



/*  
void *




*/


void sighandler(int val){
	printf("Signal caught: %d, Server turning off.\n", val);
	
	//perform all atexit functions
	//add them as needed
}



///////////////SERVER/////////////////////////


int main(int argc, char *argv[]){
	
	//signal(SIGINT, sighandler);	//catches ctrl+c
	
	if (argc != 2){
		printf("Must only include one command line argument: Port Number\n");
		return 1;
	}
	
	for (int i = 0; i < strlen(argv[1]); i ++){
		if (isdigit(argv[1][i]) == 0){	//if port number is not a digit
			printf("Port Number must be a number\n");
			return 1;
		}
	}
	int PortNum = atoi(argv[1]);	//holds the port number

	int serverfd = socket(AF_INET, SOCK_STREAM, 0);	//create socket... serverfd is the socket file descriptor
	struct sockaddr_in address;
	
	if (serverfd == 0){	
		printf("Socket Failed\n");
		return 1;
	}

	//possible use of setsockopt to set the socket options 
	

	//attach socket to the port number
	address.sin_family = AF_INET; 
    address.sin_addr.s_addr = htonl(INADDR_ANY); 
    address.sin_port = htons(PortNum); 

	
	if (bind(serverfd, (struct sockaddr *) &address, sizeof(address)) < 0){
		printf("Bind Error\n");
		return 1; 
	}
	if (listen(serverfd, 128) < 0){	// listen for connections to server
		printf("Listen Error\n");
		return 1;
	}

	int socketlen;
	int NewSocketfd;
	int CommandRead;
	char *commandResponse = "Processing Command...\n";
	char *incCommand = "Incorrect Command was received\n";

	while (1){	//infinite loop
		printf("Waiting for connections... \n");

		socketlen = sizeof(address);
		NewSocketfd = accept(serverfd, (struct sockaddr *) &address, (socklen_t *) &socketlen);	//accept a server connection
		if (NewSocketfd < 0){
			printf("Accept Failed\n");
			return 1;
		}
		printf("Client Connected!\n");

		//handle the connection 
		char *commandBuffer = (char *)malloc(41 * sizeof(char));
		CommandRead = read(NewSocketfd, commandBuffer, 40);	//reads a command sent from client
		send(NewSocketfd, commandResponse, strlen(commandResponse), 0);	//sends a validation response to client that command was received


		printf("commandBuffer = %s\n", commandBuffer);	//print the command received

		if (strcmp(commandBuffer, "checkout") == 0){

		}
		else if (strcmp(commandBuffer, "update") == 0){

		}
		else if (strcmp(commandBuffer, "upgrade") == 0){

		}
		else if (strcmp(commandBuffer, "commit") == 0){

		}
		else if (strcmp(commandBuffer, "push") == 0){

		}
		else if (strcmp(commandBuffer, "create") == 0){

		}
		else if (strcmp(commandBuffer, "destroy") == 0){

		}
		else if (strcmp(commandBuffer, "add") == 0){

		}
		else if (strcmp(commandBuffer, "remove") == 0){

		}
		else if (strcmp(commandBuffer, "currentversion") == 0){

		}
		else if (strcmp(commandBuffer, "history") == 0){

		}
		else if (strcmp(commandBuffer, "rollback") == 0){

		}
		else {	//an incorrect command was sent, so print to server and send error to client
			printf("Incorrect command was received...\n");
			send(NewSocketfd, incCommand, strlen(incCommand));
		}

		free(commandBuffer);
	}

	


	return 0; 
}
