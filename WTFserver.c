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
#include <sys/mman.h>

typedef struct filesInManifest {	//used to make a linked list of files in the .Manifest file
	char *filePath;
	char *version;
	struct filesInManifest *next;
} FIM;

FIM *FIMconstructor(char *newPath){	//creates a node for FIM LL
	FIM *temp = (FIM *)malloc(sizeof(FIM));
	temp->filePath = (char *)malloc(strlen(newPath) * sizeof(char));
	temp->next = NULL;
	strcpy(temp->filePath, newPath);
	return temp;
}

void printLL(FIM *head){ //prints linked list
	FIM *ptr = head; 
	while (ptr != NULL){
		printf("%s\n", ptr->version);
		ptr = ptr->next;
	}
}

int FIMlen(FIM *head){	//gets the length of a linked List
	FIM *ptr = head;
	int count = 0;

	while (ptr != NULL){
		count++;
		ptr = ptr->next;
	}
	return count;
}

FIM *manifestPathsLL(int fd){
	int eof = -1; 
	int spaceCount = 0;
	char *currentWord = NULL;
	

	FIM *head = NULL;
	FIM *ptr = head; 


	while (eof != 0){
		char *readBuffer = (char *)malloc(sizeof(char));
		eof = read(fd, readBuffer, 1);

		if (eof == 0){
			break;
		}


		if (spaceCount == 1){
			if (currentWord == NULL){
				currentWord = readBuffer;
			}
			else {
				strcat(currentWord, readBuffer);
			}
		}

		if (strcmp(readBuffer, " ") == 0){
			spaceCount++;
			if (spaceCount == 2){
				if (head == NULL){
					head = FIMconstructor(currentWord);
					ptr = head;
					currentWord = NULL;
				}
				else {
					ptr->next = FIMconstructor(currentWord);
					ptr = ptr->next;
					currentWord = NULL;
				}
			}
		}

		if (strcmp(readBuffer, "\n") == 0){	//when a new line is encountered 
			spaceCount = 0;
		}
	}
	
	return head; 
}

int getProjectVersion(int fd){	//get the version of the project from the .Manifest file
	int exit = 0; 
	char *versionHolder = NULL;
	int eof = -1;

	while(exit == 0){
		char *readBuffer = (char *)malloc(sizeof(char));
		eof = read(fd, readBuffer, 1);

		if (strcmp(readBuffer, "\n") == 0){
			break;
		}
		else {
			if (versionHolder == NULL){
				versionHolder = readBuffer; 
			}
			else {
				strcat(versionHolder, readBuffer);
			}
		}
	}
	int version = atoi(versionHolder);
	
	return version;
}

FIM *manifestVandPLL(int fd){	//get the versions and the filepaths into a linked list
	int eof = -1; 
	int spaceCount = 0;
	char *currentWord = NULL;
	char *currentvWord = NULL;

	FIM *head = NULL;
	FIM *ptr = head; 


	while (eof != 0){
		char *readBuffer = (char *)malloc(sizeof(char));
		eof = read(fd, readBuffer, 1);

		if (eof == 0){
			break;
		}


		if (spaceCount == 1){
			if (currentWord == NULL){
				currentWord = readBuffer;
			}
			else {
				strcat(currentWord, readBuffer);
			}
		}
		else if (spaceCount == 0) {
			if (currentvWord == NULL){
				currentvWord = readBuffer;
			}
			else {
				strcat(currentvWord, readBuffer);
			}
		}

		if (strcmp(readBuffer, " ") == 0){
			spaceCount++;
			if (spaceCount == 2){
				if (head == NULL){
					head = FIMconstructor(currentWord);
					currentvWord[strlen(currentvWord)-1] = '\0';
					printf("currentvWord = %sd\n", currentvWord);
					head->version = (char *)malloc(strlen(currentvWord) * sizeof(char));
					strcpy(head->version, currentvWord);
					printf("ptr->version == %s\n", head->version);
					ptr = head;
					currentWord = NULL;
					currentvWord = NULL;
				}
				else {
					ptr->next = FIMconstructor(currentWord);
					currentvWord[strlen(currentvWord)-1] = '\0';
					printf("currentvWord = %sd\n", currentvWord);
					ptr->next->version = (char *)malloc(strlen(currentvWord) * sizeof(char));
					strcpy(ptr->next->version, currentvWord);
					printf("ptr->version == %s\n", ptr->version);
					ptr = ptr->next;
					currentWord = NULL;
					currentvWord = NULL;
				}
			}
		}

		if (strcmp(readBuffer, "\n") == 0){	//when a new line is encountered 
			spaceCount = 0;
			currentvWord = NULL;
		}
	}
	
	return head; 
}

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

char *getFileChars(char *fileName, int size){
	int fd = open(fileName, O_RDONLY);

	char * readBuffer = (char *)malloc(size * sizeof(char));
	int readerr = read(fd, readBuffer, size);


	return readBuffer;
}

int sendTheFiles(int clientFD, char *fileName){	//used to send one file into the stream...
	//format: (# of chars in name):(filename):(# of chars in file):(chars in file):

	struct stat forSize;  
	int filesize;
	printf("fileName = %s\n", fileName);
    if (stat(fileName, &forSize) == 0) {
        filesize = (int)forSize.st_size;
	}
    else {
		printf("Size could not be found\n");
        return -1;
	}



	char *fileNamec = (char *)malloc((1 + strlen(fileName)) * sizeof(char));
	strcpy(fileNamec, fileName);
	strcat(fileNamec, ":");

	char *sendColon = ":";	//send buffer for :

	char *sendFilePathLen = (char *)malloc((1 + strlen(fileName)) * sizeof(char));
	sprintf(sendFilePathLen, "%d", (int)strlen(fileName)); //send buffer for the file path 
	strcat(sendFilePathLen, sendColon);

	char *charsInFile = (char *)malloc((1 + forSize.st_size) * sizeof(char));
	strcpy(charsInFile, getFileChars(fileName, forSize.st_size));	//stores send buffer to send the bytes in the file
	strcat(charsInFile, sendColon);

	char *sendFilecontentsLen = (char *)malloc((1 + strlen(charsInFile)) * sizeof(char));	
	sprintf(sendFilecontentsLen, "%d", filesize);	//send buffer for # of chars in file
	strcat(sendFilecontentsLen, sendColon);
	

	int senderr = send(clientFD, sendFilePathLen, strlen(sendFilePathLen), 0);	/////send the number of chars in the file path
	if (senderr < 0){
		return -1;
	}

	senderr = send(clientFD, fileNamec, strlen(fileNamec), 0);	/////////////////send the file path
	if (senderr < 0){
		return -1;
	}

	senderr = send(clientFD, sendFilecontentsLen, strlen(sendFilecontentsLen), 0); /////send the # of chars in the file)
	if (senderr < 0){
		return -1;
	}

	senderr = send(clientFD, charsInFile, filesize, 0);	//send the string of all characters in the file
	if (senderr < 0){
		return -1;
	}

	senderr = send(clientFD, sendColon, strlen(sendColon), 0); // send a colon
	if (senderr < 0){
		return -1;
	}

	return 1;

}

void *createThread(void *ptr_clientSocket){	//thread used to handle a create function call from client
	int clientSocket = *((int *)ptr_clientSocket);
	free(ptr_clientSocket);

	int projFound = 0;

	char *fileName = (char *)malloc(300 * sizeof(char));
	strcpy(fileName, readToColon(clientSocket));

	printf("creating project: ./%s\n", fileName);

	struct dirent *dirPtr;
	DIR *dir = opendir("./");
	if (dir == NULL){
		printf("Cannot open Current Working Directory\n");
		return NULL;
	}


	while ((dirPtr = readdir(dir)) != NULL){	// checks for the project in the current directory
		if(strcmp(dirPtr->d_name, fileName) == 0){
			projFound = 1;	// logs that the project is in the directory
			break;
		}
	}


	closedir(dir);
	char *sendFail = "Failed:";
	int senderr;

	if (projFound == 1){	//if project is in the directory, then send an error since the project already exists
		printf("%s already exists\n", fileName);
		senderr = send(clientSocket, sendFail, strlen(sendFail), 0);
	}
	else {	//if project is not found in the directory, then create the project and manifest file
		char projectPath[302] = "./";
		strcat(projectPath, fileName);

		if (mkdir(projectPath, 0777) < 0){	//make the project specified
			printf("%s directory could not be created\n", projectPath);
			senderr = send(clientSocket, sendFail, strlen(sendFail), 0);
		}

		strcat(projectPath, "/.Manifest");
		int manifest = creat(projectPath, S_IRWXU);	//create the .Manifest file for the project
		if (manifest < 0){
			printf(".Manifest file could not be created\n");
			senderr = send(clientSocket, sendFail, strlen(sendFail), 0);
		}
		char *versionNo = "0\n";
		senderr = write(manifest, versionNo, strlen(versionNo));

		////send file in  format: sendFile:(number of Files):(length of filename):(filename):
		char *sendSF = "sendfile:1:";
		senderr = send(clientSocket, sendSF, strlen(sendSF), 0);
		printf("projectPath = %s\n\n", projectPath);
		senderr = sendTheFiles(clientSocket, projectPath);
		
	}

	return NULL;
}

void destroyDirectory(char *projName){

	DIR *directory = opendir(projName);
	struct dirent *fileptr;
	int fd = 0;

	if (!directory){
		return;
	}



	while ((fileptr = readdir(directory)) != NULL){
		if (strcmp(fileptr->d_name, ".") != 0 && strcmp(fileptr->d_name, "..") != 0){
			if (fileptr->d_type == DT_DIR){
				//recursively into directory
				//then delete directory after it returns
				char *newFilePath = (char *)malloc((strlen(projName) + strlen(fileptr->d_name) + 2) * sizeof(char));

				strcpy(newFilePath, projName);
				strcat(newFilePath, "/");
				strcat(newFilePath, fileptr->d_name);

				destroyDirectory(newFilePath);
				
				rmdir(newFilePath);

				free(newFilePath);
			}
			else {	//if its a file
				char *newFilePath = (char *)malloc((strlen(projName) + strlen(fileptr->d_name) + 2) * sizeof(char));

				strcpy(newFilePath, projName);
				strcat(newFilePath, "/");
				strcat(newFilePath, fileptr->d_name);


				unlink(newFilePath);


				free(newFilePath);
			}
		}
	}
	closedir(directory);


}



void *destroyThread(void *ptr_clientSocket){	//thread used to handle the destroy function call from client
	int clientSocket = *((int *)ptr_clientSocket);
	free(ptr_clientSocket);



	int projFound = 0;

	char *fileName = (char *)malloc(300 * sizeof(char));
	strcpy(fileName, readToColon(clientSocket));	//gives the project name

	printf("destroying project: ./%s\n", fileName);

	struct dirent *dirPtr;
	DIR *dir = opendir("./");
	if (dir == NULL){
		printf("Cannot open Current Working Directory\n");
		return NULL;
	}


	while ((dirPtr = readdir(dir)) != NULL){	// checks for the project in the current directory
		if(strcmp(dirPtr->d_name, fileName) == 0){
			projFound = 1;	// logs that the project is in the directory
			break;
		}
	}

	closedir(dir);

	char *sendFail = "Failed:";
	char *sendSucc = "Deleted:";

	if (projFound == 0) {
		printf("file does not exist and thus cannot be deleted\n");
		send(clientSocket, sendFail, strlen(sendFail), 0);
	}
	else{	//project was found and must be DESTROYED

		char *projName = (char *)malloc((2 + strlen(fileName)) * sizeof(char));	//stores the project path
		strcpy(projName, "./");
		strcat(projName, fileName);

		destroyDirectory(projName);
		rmdir(projName);
		send(clientSocket, sendSucc, strlen(sendSucc), 0);
	}

	return NULL;
}


void *checkoutThread(void *ptr_clientSocket){	//thread used to handle the checkout function call from client
	int clientSocket = *((int *)ptr_clientSocket);
	free(ptr_clientSocket);

	int projFound = 0;

	char *ProjName = (char *)malloc(300 * sizeof(char));
	strcpy(ProjName, readToColon(clientSocket));

	printf("Checking out: ./%s\n", ProjName);

	struct dirent *dirPtr;
	DIR *dir = opendir("./");
	if (dir == NULL){
		printf("Cannot open Current Working Directory\n");
		return NULL;
	}


	while ((dirPtr = readdir(dir)) != NULL){	// checks for the project in the current directory
		if(strcmp(dirPtr->d_name, ProjName) == 0){
			projFound = 1;	// logs that the project is in the directory
			break;
		}
	}


	closedir(dir);
	char *sendFail = "Failed:";
	int senderr;

	if (projFound == 0){	//if project is in the directory, then send an error since the project already exists
		printf("%s does not exist\n", ProjName);
		senderr = send(clientSocket, sendFail, strlen(sendFail), 0);
	}
	else {
		printf("project exists\n");

		char *cwd = "./";
		char *mantemp = "/.Manifest";
		char *manifestName = (char *)malloc((strlen(cwd) + strlen(mantemp) + strlen(ProjName)) * sizeof(char));
		strcpy(manifestName, cwd);
		strcat(manifestName, ProjName);
		strcat(manifestName, mantemp);

		printf("manifest path = %s\n", manifestName);
		int manifestFD = open(manifestName, O_RDONLY);
		//check if manifest file can be opened... if not then something went wrong because there is no manifest file
		printf("manifest path = %s\n", manifestName);

		FIM *head = NULL;
		head = manifestPathsLL(manifestFD);
		
		int fiml = FIMlen(head);
		fiml++;

		
		char *nOfFiles = (char *)malloc((1 + strlen(manifestName)) * sizeof(char));
		sprintf(nOfFiles, "%d", fiml);
		strcat(nOfFiles, ":");

		char *sendsucc = (char *)malloc((strlen(nOfFiles) + strlen("sendfile:")) * sizeof(char));
		strcpy(sendsucc, "sendfile:");
		strcat(sendsucc, nOfFiles);
		senderr = send(clientSocket, sendsucc, strlen(sendsucc), 0);

		senderr = sendTheFiles(clientSocket, manifestName);	//send the manifest file
		printf("manifestName = %s\n", manifestName);

		FIM *ptr = head;
		
		while (ptr != NULL){	// send the files 
			printf("ptr->filePath = %slol\n", ptr->filePath);
			ptr->filePath[strlen(ptr->filePath)-1] = '\0';
			senderr = sendTheFiles(clientSocket, ptr->filePath);
			ptr = ptr->next;
		}

		ptr = head;
		while (ptr != NULL){	//free all the nodes in the LL 
			ptr = ptr->next;
			free(head->filePath);
			free(head);
			head = ptr;
		}
		

		close(manifestFD);
	}

	return NULL;
}

void *updateThread(void *ptr_clientSocket){
	int clientSocket = *((int *)ptr_clientSocket);
	free(ptr_clientSocket);

	int projFound = 0;

	char *ProjName = (char *)malloc(300 * sizeof(char));
	strcpy(ProjName, readToColon(clientSocket));

	printf("Update Call: ./%s\n", ProjName);

	struct dirent *dirPtr;
	DIR *dir = opendir("./");
	if (dir == NULL){
		printf("Cannot open Current Working Directory\n");
		return NULL;
	}


	while ((dirPtr = readdir(dir)) != NULL){	// checks for the project in the current directory
		if(strcmp(dirPtr->d_name, ProjName) == 0){
			projFound = 1;	// logs that the project is in the directory
			break;
		}
	}


	closedir(dir);
	char *sendFail = "Failed:";
	int senderr;

	if (projFound == 0){	//if project is in the directory, then send an error since the project already exists
		printf("%s does not exist\n", ProjName);
		senderr = send(clientSocket, sendFail, strlen(sendFail), 0);
	}
	else {
		char *cwd = "./";
		char *mantemp = "/.Manifest";
		char *manifestName = (char *)malloc((strlen(cwd) + strlen(mantemp) + strlen(ProjName)) * sizeof(char));
		strcpy(manifestName, cwd);
		strcat(manifestName, ProjName);
		strcat(manifestName, mantemp);


		char *sendsucc = (char *)malloc((strlen("sendfile:1:")) * sizeof(char));
		strcpy(sendsucc, "sendfile:1:");
		
		senderr = send(clientSocket, sendsucc, strlen(sendsucc), 0);
		senderr = sendTheFiles(clientSocket, manifestName);
	}
	return NULL;
}

void *currentversionThread(void * ptr_clientSocket){
	int clientSocket = *((int *)ptr_clientSocket);
	free(ptr_clientSocket);

	int projFound = 0;

	char *ProjName = (char *)malloc(300 * sizeof(char));
	strcpy(ProjName, readToColon(clientSocket));

	printf("Getting current of: ./%s\n", ProjName);

	struct dirent *dirPtr;
	DIR *dir = opendir("./");
	if (dir == NULL){
		printf("Cannot open Current Working Directory\n");
		return NULL;
	}


	while ((dirPtr = readdir(dir)) != NULL){	// checks for the project in the current directory
		if(strcmp(dirPtr->d_name, ProjName) == 0){
			projFound = 1;	// logs that the project is in the directory
			break;
		}
	}


	closedir(dir);
	char *sendFail = "Failed:";
	int senderr;

	if (projFound == 0){	//if project is in the directory, then send an error since the project already exists
		printf("%s does not exist\n", ProjName);
		senderr = send(clientSocket, sendFail, strlen(sendFail), 0);
	}
	else {
		char *cwd = "./";
		char *mantemp = "/.Manifest";
		char *manifestName = (char *)malloc((strlen(cwd) + strlen(mantemp) + strlen(ProjName)) * sizeof(char));
		strcpy(manifestName, cwd);
		strcat(manifestName, ProjName);
		strcat(manifestName, mantemp);

		FIM *head = NULL;


		int manifestFD = open(manifestName, O_RDONLY);
		int version = getProjectVersion(manifestFD);	//get the version # of the project
		printf("version == %d\n", version);
		close(manifestFD);

		manifestFD = open(manifestName, O_RDONLY);	//get linked list of paths and version numbers
		head = manifestVandPLL(manifestFD);
		close(manifestFD);

		int fiml = FIMlen(head);
		char *noOfFiles = (char *)malloc(40 * sizeof(char *));	//send number of files
		sprintf(noOfFiles, "%d:", fiml);
		

		char *ProjVersion = (char *)malloc(40 * sizeof(char *));	//send version for the whole project 
		sprintf(ProjVersion, "%d:", version);

		char *sendsucc = (char *)malloc((strlen("versions:") + strlen(noOfFiles) + strlen(ProjVersion)) * sizeof(char));	//sends -- versions:(# of files):
		strcpy(sendsucc, "versions:");
		strcat(sendsucc, noOfFiles);
		strcat(sendsucc, ProjVersion);
		senderr = send(clientSocket, sendsucc, strlen(sendsucc), 0);


		FIM *ptr = head;
		while (ptr != NULL){	//sends -- (length of filepath):(version):(filepath):
			printf("ptr->filePath = %slol\n", ptr->filePath);
			ptr->filePath[strlen(ptr->filePath)-1] = '\0';
			printf("ptr->filePath = %slol\n", ptr->filePath);

			char *fileNameSize = (char *)malloc((1 + strlen(ptr->filePath)) * sizeof(char));//send the file name size
			sprintf(fileNameSize, "%d:", (int)strlen(ptr->filePath));
			senderr = send(clientSocket, fileNameSize, strlen(fileNameSize), 0);

			senderr = send(clientSocket, ptr->version, strlen(ptr->version), 0);	//send the version 
			senderr = send(clientSocket, ":", strlen(":"), 0);	//send colon
			senderr = send(clientSocket, ptr->filePath, strlen(ptr->filePath), 0); //send filepath
			senderr = send(clientSocket, ":", strlen(":"), 0);

			free(fileNameSize);
			ptr = ptr->next;
		}

		ptr = head;
		while (ptr != NULL){	//free all the nodes in the LL 
			ptr = ptr->next;
			free(head->filePath);
			free(head->version);
			free(head);
			head = ptr;
		}



		printLL(head);

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
	char *commandResponse = "Processing Command...\n:";
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

		char *command = (char *)malloc(41 * sizeof(char));
		strcpy(command, readToColon(ClientSocketfd));	//reads a command sent from client

		printf("commandBuffer = %s\n", command);	//print the command received

		if (strcmp(command, "checkout") == 0){

			pthread_t checkoutT;
			int *checkoutClient = (int *)malloc(sizeof(int));
			*checkoutClient = ClientSocketfd;
			pthread_create(&checkoutT, NULL, checkoutThread, checkoutClient);

		}
		else if (strcmp(command, "update") == 0){

			pthread_t updateT;
			int *updateClient = (int *)malloc(sizeof(int));
			*updateClient = ClientSocketfd;
			pthread_create(&updateT, NULL, updateThread, updateClient);

		}
		else if (strcmp(command, "upgrade") == 0){

		}
		else if (strcmp(command, "commit") == 0){

		}
		else if (strcmp(command, "push") == 0){

		}
		else if (strcmp(command, "create") == 0){	//calls the create function

			pthread_t createT;
			int *createClient = (int *)malloc(sizeof(int));
			*createClient = ClientSocketfd;
			pthread_create(&createT, NULL, createThread, createClient);
			
		}
		else if (strcmp(command, "destroy") == 0){
			pthread_t destroyT;
			int *destroyClient = (int *)malloc(sizeof(int));
			*destroyClient = ClientSocketfd;
			pthread_create(&destroyT, NULL, destroyThread, destroyClient);
		}
		else if (strcmp(command, "currentversion") == 0){

			pthread_t currentversionT;
			int *currentversionClient = (int *)malloc(sizeof(int));
			*currentversionClient = ClientSocketfd;
			pthread_create(&currentversionT, NULL, currentversionThread, currentversionClient);

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
