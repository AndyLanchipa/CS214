#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 
#include <fcntl.h>


///////////////////////////////client connect and Configure////////////////////////////////////////////////////////////////////////////////

/*
./WTF configure <IP/hostname> <port>

-save ip address or host name and port for use by later commands
-saves ip and portnumber so that they are not needed by parameters for  all other commands
	-written to a ./.configure file


*/
int configure(char * IPHostname, char * PORT){
	//create the file
	int  fd = creat(".configure" , S_IRWXU );
	//open the file
	int ofd = open(".configure",O_WRONLY);

	//write to file 
	write(ofd,IPHostname,strlen(IPHostname));
	write(ofd," ",strlen(" "));
	write(ofd,PORT,strlen(PORT));


		return 1;

}

/////////connect client to server/////////////


int tryConnect(char * IP, char * PORT){


 char *address;
      address = IP;

     //creating socket
     int net_socket =0;
     net_socket =socket(AF_INET,SOCK_STREAM,0);// we put 0 in third arguement because we are using the default protocal TCP
	

	//this will check if the socket was created
	 if(net_socket<0){
		 printf("\n socket Creation error");
		 return -1;
	 }

     //now we need to connect to server


	 int Porttwo = atoi(PORT);
    
	 // Connect to the remote server
	struct sockaddr_in remoteaddr;

	remoteaddr.sin_family = AF_INET;
	
	remoteaddr.sin_port = htons(Porttwo);
//convert ip to binary from text
	if(inet_pton(AF_INET, IP, &remoteaddr.sin_addr)<=0){
		printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
	}

	
	int connection_stat; //this will see if the connection is successful

	while(1){	
	printf("connecting");
	connection_stat =connect(net_socket, (struct sockaddr *)&remoteaddr, sizeof(remoteaddr));	
	//the connection went through and we will stop trying to constantly connect
	if(connection_stat == 0){
		break;
	}
	sleep(3);
	}
		

	//check if connection was successful

	//need to perform while loop so it attempts to connect 3 times
     if(connection_stat <0){
         printf("there is an error to connect the socket to the server");
     }




// 	 char *hello = "Hello from client"; 
// printf("hello\n");
// 	 send(net_socket , hello , strlen(hello) , 0 ); 
// 	 printf("hello\n");
//     printf("Hello message sent\n"); 
// 	char buffer[1024] = {0};
// 	int valread=0;
//     valread = read( net_socket, buffer, 1024); 
//     printf("%s\n",buffer ); 
 
	return net_socket;
}



/* 
this method takes in nothing but will get the port number and ipaddress from 
the configure file and pass it into the tryconnect funtion


*/
int GetIandP(){
	
	//open file
	int fd =open(".configure",O_RDONLY);
	
	int ipGot =0; //this will tell if the ip is gotten

	char  *Port =(char *)malloc(sizeof(char));
	char * IP =  (char *)malloc(sizeof(char));

	int eof =10;
	while(eof!=0){
		
		
		char * readBuffer= (char *)malloc(sizeof(char));
		
		eof = read(fd, readBuffer, 1);	//read from file
		

	//check for space and when it is encountered then input into socket and reset the IP char*
	if(strcmp(readBuffer," ")>0  && ipGot!=1){

		strcat(IP,readBuffer);
		
	

	}//if space is encountered then make ipgot =1 so that we can start constructing the port number
	else if(strcmp(readBuffer," ")==0){
		printf("here at this if condition\n\n\n");
		ipGot =1;

	}
	else if(strcmp(readBuffer," ")> 0 && ipGot==1){
		strcat(Port,readBuffer);
		;
	}
		

	}
	
	 int success =tryConnect(IP,Port);
	return success;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////Create method////////////////////////////////////////////////////////////////////////////////////////










/*
//this method takes will take in the project name and make a local version of them project folder in its current directory and should place
the .Manifest the server sent in it.


*/



int findConfig(){

	 struct dirent *de;  // Pointer for directory entry 
  
    // opendir() returns a pointer of DIR type.  
    DIR *dr = opendir("."); 
	 if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory" ); 
        return 0; 
    } 
	while ((de = readdir(dr)) != NULL){
		if(strcmp(de->d_name ,".configure")){
			return 1;

		}

	}
        


   return 0;
}































int CreateProjectfolder(int fd, char * Project_name){
	int valread=1;
	int firstloop =0;
	char * name_of_Project= (char*) malloc(sizeof(char));
	char * buffer = (char*) malloc(sizeof(char));
	
	// when 0 is reached then that is the end of the file
	while(valread!=0){
		char * buffer = (char*) malloc(sizeof(char));
		valread = read(fd, buffer, 1);

		if(strcmp(buffer, ":")==0){
			break;

		}

		if(valread ==-1 ){
			printf("could not project name!");
			return -1;

		}

		strcat(name_of_Project,buffer);
	}




	 int check = mkdir(name_of_Project,S_IRWXU);

	if(!check){
		printf("directory created\n ");

	}
	else{

		printf("Unable to create directory\n");
		return -1;
	}
 




	return 1;
}





























int main(int argc, char * argv[]){



int Configcall =0;
int Create =0;
for(int i=0; i<argc-1;i++){
	if(strcmp(argv[i],"configure")==0){
	
		Configcall=1;
		continue;
	}
	//Configure is called as a command so we have call configure method to write 

	//ip number and port number to the file
	else if(Configcall==1){
		
		//write the configure file
		configure(argv[i],argv[i+1]);
		
		
			break;
	}
	else if(strcmp(argv[i],"create")==0){
		Create =1;
		continue;
	}//this means that create was called
	else if(Create ==1 ){

		//this will call the findconfig method which returns and int of 1 if found or  -1 if not found
		//checks the directory to see if the config file was found
		int success =findConfig();
		
		/*
			if find config returns a 1 then we will do the GetIandP method which
			reads from the .config file and finds the IP and Port and sends it to the 
			tryConnect() method to try and connect to the server every 3 seconds until success
		*/
		if(success ==1){
		int pass =GetIandP();
		
		/*
			this means that the tryConnect() method has failed to connect to the server so we 
			will return a -1 and end the program
		*/
		if(pass == -1){
			return -1;

		}

		/*
			send the project name over to the server for it to check if it exists already
			pass is the file descriptor that got passed through from the tryConnect() method

		*/
		send(pass , argv[i+1] , strlen(argv[i+1]) , 0 );
		char * buffer = (char*) malloc(sizeof("folder not found") +1); 	//malloc size for the buffer to be read from server
		int valread=0; // return the bytes read after read is called
		valread = read( pass, buffer, sizeof(buffer));  //reads from the server and stores it into the buffer char *

		/*
			these if statements will strcmp the buffer and the phrase "folder not found"
			if they are equal then we create the project folder into the current directory
			
			if buffer and the phrase "folder is found" are equal then we print out the "the project
			already exists" and return -1 to end the program and close the socket
		*/

		int folder_Created=0;

		 if(strcmp(buffer,"folder not found")==0){
			 	folder_Created = CreateProjectfolder(pass,argv[i+1]);
				
				if(folder_Created==1){
					 printf("folder was created!");
				 //close the socket after the command
				 close(pass);

				}
				else if(folder_Created == -1){
					printf("folder could not be created");
					// close the socket after command
					close(pass);
				}
			
				
		 }
		 else if(strcmp(buffer, "folder is found")==0){
			 printf("the project folder already exists!");
			 close(pass);
			 return 0;
		 }

			


			break;

		}
		else if(success == 0){
			printf("ERROR! Configure command was not called before this command!");
			return -1;
		}
	
	}
	

}

 

	return 0;
}
