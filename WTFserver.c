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
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>


char *readToColon(int clientSocket){	//reads from socket until a colon is encountered

	char *readBuffer = NULL; 
	char *word = NULL;	//stores the word that will be returned
	int readval;

	while(1){
		
		readBuffer = (char *)malloc(sizeof(char));
		readval = read(clientSocket, readBuffer, 1);

		if (readval == 0){	//if readval == 0 then that means that there was nothing left to read... colon was not encountered
			printf("colon was not encountered at the end\n");
			break;
		}

		if (strcmp(readBuffer, ":") == 0){	//when a colon is encountered it is the end of the word
			break;
		}

		if (word == NULL){ //if its the first character read
			word = readBuffer;
		}
		else{	//add readbuffer onto word
			strcat(word, readBuffer);
		}
	}

	return word;
}



void *createThread(void *ptr_clientSocket){	//thread used to handle a create function call from client
	int clientSocket = *((int *)ptr_clientSocket);
	free(ptr_clientSocket);

	int projFound = 0;

	char *fileName = (char *)malloc(300 * sizeof(char));
	strcpy(fileName, readToColon(clientSocket));
	printf("fileName = %s\n", fileName);

	struct dirent *dirPtr;
	DIR *dir = opendir("./");
	if (dir == NULL){
		printf("Cannot open Current Working Directory\n");
		return NULL;
	}


	while ((dirPtr = readdir(dir)) != NULL){	// checks for the project in the current directory
		if(strcmp(dirPtr->d_name, fileName) == 0){
			projFound = 1;	// logs that the project is in the directory
		}
	}

	if (projFound == 1){	//if project is in the directory, then send an error since the project already exists
		printf("Project already exists\n");
	}
	else {	//if project is not found in the directory, then create the project and manifest file
		printf("Project not found\n");
		char projectPath[302] = "./";
		printf("hello\n");
		strcat(projectPath, fileName);
		printf("projectPath = %s\n", projectPath);

		mkdir(projectPath, 0777);	//make the project specified
		strcat(projectPath, "/.Manifest");
		int manifest = creat(projectPath, S_IRWXU);	//create the .Manifest file for the project
		if (manifest < 0){
			printf(".Manifest file could not be created\n");
		}

		manifest = open(projectPath, O_RDONLY);	//open the file to be read
		sendfile(manifest, clientSocket, 0, );


	}



	return NULL;
}


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
	int ClientSocketfd;
	char *commandResponse = "Processing Command...\n";
	char *incCommand = "Incorrect Command was received\n";

	
	////////////////////////////////////////////////////////////////////////////SERVER LOOP IS HERE 

	while (1){	//infinite loop for server to listen for connections and process them in threads
		
		//accept a client
		printf("Waiting for connections... \n");
		socketlen = sizeof(address);
		ClientSocketfd = accept(serverfd, (struct sockaddr *) &address, (socklen_t *) &socketlen);	//accept a server connection
		if (ClientSocketfd < 0){
			printf("Accept Failed\n");
			continue;
		}
		printf("Client Connected!\n");	//announce that a Client has been connected


		////////////////////////////////////////////////////////////////////HANDLE THE CONNECTION

		char *command = (char *)malloc(41 * sizeof(char));;
		strcpy(command, readToColon(ClientSocketfd));	//reads a command sent from client
		send(ClientSocketfd, commandResponse, strlen(commandResponse), 0);	//sends a validation response to client that command was received

		printf("commandBuffer = %s\n", command);	//print the command received

		if (strcmp(command, "checkout") == 0){

		}
		else if (strcmp(command, "update") == 0){

		}
		else if (strcmp(command, "upgrade") == 0){

		}
		else if (strcmp(command, "commit") == 0){

		}
		else if (strcmp(command, "push") == 0){

		}
		else if (strcmp(command, "create") == 0){	//calls the create function

			printf("create function called...\n");	//announce what function was called
			pthread_t createT;
			int *createClient = (int *)malloc(sizeof(int));
			*createClient = ClientSocketfd;
			pthread_create(&createT, NULL, createThread, createClient);

		}
		else if (strcmp(command, "destroy") == 0){

		}
		else if (strcmp(command, "add") == 0){

		}
		else if (strcmp(command, "remove") == 0){

		}
		else if (strcmp(command, "currentversion") == 0){

		}
		else if (strcmp(command, "history") == 0){

		}
		else if (strcmp(command, "rollback") == 0){

		}
		else {	//an incorrect command was sent, so print to server and send error to client... will most likely never happen
			printf("Incorrect command was received...\n");
			send(ClientSocketfd, incCommand, strlen(incCommand), 0);
		}

		printf("\n");
		free(command);
	}

	


	return 0; 
}
