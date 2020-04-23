#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


//////client connect and Configure/////////

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

/////////oonnect client to server/////////////


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

	

	int connection_stat =connect(net_socket, (struct sockaddr *)&remoteaddr, sizeof(remoteaddr));	
	
	//check if connection was successful
     if(connection_stat ==-1){
         printf("there is an error to connect the socket to the server");
     }
 
	return 1;
}



/* 
this method takes in nothing but will get the port number and ipaddress from 
the configure file and pass it into the tryconnect funtion


*/
void GetIandP(){

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
	return;
}






int main(int argc, char * argv[]){
int Configcall =0;
for(int i=0; i<argc-1;i++){
	if(strcmp(argv[i],"configure")==0){
	
		Configcall=1;
		continue;
	}
	//Configure is called as a command so we have call configure method to write 
	//ip number and port number to the file
	if(Configcall==1){
		

		configure(argv[i],argv[i+1]);
		
			break;
	}

}

 GetIandP();




    

	return 0;
}
