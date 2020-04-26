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



void sendtoServer(int fd , char*to_Send){

		send(fd , to_Send , strlen(to_Send) , 0 );// the command name for the server to process and read
	

}





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

	if(fd==-1){
		return -1;
	}
	//open the file
	int ofd = open(".configure",O_WRONLY);

	if(ofd==-1){
		return -1;
	}

	//write to file 
	int sz;
	sz = write(ofd,IPHostname,strlen(IPHostname));
	if(sz == -1){
		return -1;
	}
	sz =write(ofd," ",strlen(" "));
	if(sz == -1){
		return -1;
	}
	sz = write(ofd,PORT,strlen(PORT));
	if(sz == -1){
		return -1;
	}


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
	printf("connecting\n");
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
         printf("there is an error to connect the socket to the server\n");
     }

 
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
		
	}
		

	}
	printf("PORT: %s\n", Port);
	 int success =tryConnect(IP,Port);
	return success;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////Create method////////////////////////////////////////////////////////////////////////////////////////





/* 
	this method is called and goes through the directory and tries to find
	.configure file, if it is not found then it returns a -1
	*/



int findConfig(){

	 struct dirent *de;  // Pointer for directory entry 
  
    // opendir() returns a pointer of DIR type.  
    DIR *dr = opendir("."); 
	 if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory\n" ); 
        return 0; 
    } 
	while ((de = readdir(dr)) != NULL){

		printf("name: %s\n\n\n",de->d_name);
		if(strcmp(de->d_name ,".configure")==0){
			return 1;

		}

	}
        


   return -1;
}


/*
//this method takes will take in the project name and make a local version of them project folder in its current directory and should place
the .Manifest the server sent in it.


*/

int CreateProjectfolder(char * Project_name){
	
char * some = (char*) malloc((strlen(Project_name)+2) * sizeof(char));
strcpy(some,"./");
strcat(some,Project_name);

	 int check = mkdir(some,S_IRWXU);

	if(!check){
		printf("directory created\n ");

	}
	else{

		printf("Unable to create directory\n");
		return -1;
	}
 




	return 1;
}




int recieveFilefromServer(int fd , char * foldername){

	
	char *Currentword= (char*) malloc(sizeof(char));
	char * file_content =NULL;
	char *filename=NULL;
	
	int Read =0;

	int numofFiles =0; //this will store the amount of files that are sent over

	int File_created =0;

	int Getfsize=0;

	int size=0;

	int getWsize= 0;//this gets the write size of the file


	while(1){
		char * Buffer = (char *) malloc(sizeof(char));
		Read = read(fd, Buffer, 1);
	

		if(Read==-1){

			printf("could not read from file!\n");
			return -1;
		}


		if(strcmp(Buffer,":")==0){
	
			//if we encounter sendfile then that means we s
			if(strcmp(Currentword,"sendfile")==0){

		

				if(foldername!=NULL){

					printf("%s\n\n\n",foldername);
					
					CreateProjectfolder(foldername);
			

				}
				

				Currentword=NULL;
				Currentword =(char*) malloc(sizeof(char));

				

				

			

				continue;
			}
			else if(numofFiles ==0){
			

				numofFiles= atoi(Currentword);
			
			Currentword=NULL;
				Currentword =(char*) malloc(sizeof(char));

				break;
			}
			

		}
		else{
			

			strcat(Currentword,Buffer);
			
			
			//this means that the function has failed and the file was not sent back
			if(strcmp(Currentword,"Failed")==0){
				return -1;

			}

		}



	}







printf("before while looop\n\n\n\n");




	int i=0;
	printf("numoffile %d:\n",numofFiles);

	while(i<numofFiles+1){
		char * Buffer = (char *) malloc(sizeof(char));

		// if(i==(numofFiles-1)){
		// 			break;// we break because we have written to the last file

		// 		}
		if(Buffer=="\0"){
			break;
		}

		printf("before read %s\n\n\n", Buffer);
		Read = read(fd, Buffer, 1);
		printf("after read\n\n\n");

		

		if(Read==-1){

			printf("could not read from file!\n");
			return -1;
		}

			printf("Buffer: %s\n\n\n",Buffer);
		//this will be either a file name or number of bytes
		if(strcmp(Buffer,":") == 0){
			
		

			//if numofFiles is greater than 0 then that means we have read up until the amount of files and the next number
			 //will be the number of bytes for file name
			 if(numofFiles>0 && Getfsize==0){
				 
			
				Getfsize=1;
				//convert size of file into integer
				size = atoi(Currentword);
				
				Currentword=NULL;
				Currentword =(char*) malloc(sizeof(char));
				continue;
			}//create the file since we have file size
			else if(Getfsize>0 && File_created==0){
				printf("here creating file\n\n\n");
			
				File_created=1;
				//numofFiles++;
				//malloc file name
				filename =(char *) malloc(sizeof(char)* size);
				strcpy(filename,Currentword);
				
				int cre = creat(filename,S_IRWXU);

				size=0;
				printf("here after file create\n\n\n");
				Currentword=NULL;
				Currentword =(char*) malloc(sizeof(char));
				printf("here after file create 2\n\n\n");
				printf("filecreated : %d\n\n\n\n\n\n\n", File_created);
				continue;

			}//this means that the file size and file have been created so now  get size of contents in file
			else if(size==0 && File_created==1){
				printf("here1\n\n\n");

				size= atoi(Currentword);

				getWsize=size;
				//if number of files is reached this means that it is writing to last file
				//however if the size of this file is zero then we write nothing so we break out 
				//of it
				if( getWsize ==0){
					i++;

					if(i==numofFiles){
						break;
					}
				}
				else if( getWsize==0){
					i++;
					if(i!=numofFiles){
						Getfsize==0;
						File_created==0;
						size==0;
						Currentword=NULL;
						Currentword =(char*) malloc(sizeof(char));
						continue;

					}
				}
				
				
				printf("here1.5\n\n\n");
				Currentword=NULL;
				Currentword =(char*) malloc(sizeof(char));

				
				continue;


			}
			else if( getWsize >0 && File_created==1 ){
				printf("here2\n\n\n");
				i++;
				
				int o = open(filename, O_RDWR);
				file_content= (char*) malloc(sizeof(char)*getWsize);
				strcpy(file_content, Currentword);
				int sz=write(o,file_content,strlen(file_content));
			
			Currentword=NULL;
				Currentword =(char*) malloc(sizeof(char));
				Getfsize==0;
				File_created==0;
				size==0;
				

				
				continue;




			}

		}
		else {
			
			strcat(Currentword,Buffer);
			printf("After Buffer: %s\n\n\n\n\n\n", Currentword);
			
			//this means that the function has failed and the file was not sent back
			if(strcmp(Currentword,"Failed")==0){
				return -1;

			}
			
		}




	}




	return 1;
}




































/////////////////////////////////////////////////////Destroy method/////////////////////////////////////////////////////////////////////////








int main(int argc, char * argv[]){



int Configcall =0;

int Create =0;

int Destroy_call=0;

for(int i=0; i<argc;i++){
	if(strcmp(argv[i],"configure")==0){
	
		Configcall=1;
		continue;
	}
	//Configure is called as a command so we have call configure method to write 

	//ip number and port number to the file
	else if(Configcall==1){
		
		int success = 0;
		//write the configure file
		success =configure(argv[i],argv[i+1]);

		if(success ==1){
			printf("configure command was completed successfully\n");
			return 0;
		}
		else if(success ==-1){
			printf("configure command was failed!\n");
			return -1;
			
		}
		
		
			break;
	}
	else if(strcmp(argv[i],"create")==0){
		printf("herereere\n\n\n\n\n");
		Create =1;
		continue;
	}//this means that create was called
	else if(Create ==1 ){
		
printf("herereere\n\n\n\n\n");
		//this will call the findconfig method which returns and int of 1 if found or  -1 if not found
		//checks the directory to see if the config file was found
		int success =findConfig();
		printf("herereere: %d\n\n\n\n\n", success);
		/*
			if find config returns a 1 then we will do the GetIandP method which
			reads from the .config file and finds the IP and Port and sends it to the 
			tryConnect() method to try and connect to the server every 3 seconds until success
		*/
		if(success ==1){
			printf("herereerein\n\n\n\n\n");
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
		char * command =(char*) malloc(sizeof(argv[i-1])+1);
		command ="create:";
		char * project_name =(char*) malloc(sizeof(argv[i])+1);
		strcpy(project_name,argv[i]);
		strcat(project_name,":");
		//sends command to server
		sendtoServer(pass, command);
		sendtoServer(pass,project_name);


		
		int valread=0; // return the bytes read after read is called
		printf("HERE 1 %s:\n",argv[i]);

		recieveFilefromServer(pass , argv[i]);
		printf("HERE 2:\n");

		
			


			break;

		}
		else if(success == -1){
			printf("ERROR! Configure command was not called before this command!\n");
			return -1;
		}
	
	}
	else if(strcmp(argv[i],"destroy")==0){
		Destroy_call =1;
	}
	else if(Destroy_call==1){

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

		char * command =(char*) malloc(sizeof(argv[i+1])+3);
		command ="destroy:";

		char * project_name =(char*) malloc(sizeof(argv[i+1])+3);
		project_name = argv[i+1];
		strcat(project_name,":");

		sendtoServer(pass,command);
		sendtoServer(pass, project_name);

		





















	
		}
		else if(success == 0){
			printf("ERROR! Configure command was not called before this command!\n");
			return -1;
		}
		


	}
	

}

 

	return 0;
}
