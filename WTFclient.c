#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 
#include <fcntl.h>
#include <openssl/md5.h>
#include <sys/mman.h>



void sendtoServer(int fd , char*to_Send){

		send(fd , to_Send , strlen(to_Send) , 0 );// the command name for the server to process and read
	

}

int FindPath_remove(int fd , char * path){



//this will be to find how many bytes needed to go back to go back to the letter
int bytes_to_off = 0;

int at_path = 0;

int eof = 10; // this will be to detect the end of the file

char * Current_word = (char *) malloc(sizeof(char));

while(eof!=0){

char * Buffer = (char*) malloc(sizeof(char));

bytes_to_off++;

eof = read(fd , Buffer , 1);
if (eof == 0){
			break;
		}






//if the current Buffer is space then that means we have found the end of the word
		//or if Buffer is the new line meaning we are looking at a new line which is a new file path
		if(strcmp(Buffer," ")==0 || strcmp(Buffer,"\n")==0){
			printf("CurrentWord=:%s \n",Current_word);

			at_path++;//tells me where current path is
			if(strcmp(Buffer,"\n")==0){


				Current_word = NULL;

				Current_word = (char*) malloc(sizeof(char));
				bytes_to_off=0;
				at_path=0;
				continue;
				//continue

			}

			if(at_path == 3 && strcmp(Current_word,path)==0){
				printf("here at path\n\n\n\n\n\n");
				lseek(fd,(-1*bytes_to_off),SEEK_CUR);

				int bytes =write(fd,"R", strlen("R"));

				if(bytes == -1){
					printf("failed to remove entry from .Manifest\n");
					return -1;
				}

				

				return 1;




			}//this means that we are at the file path and the paths dont equal so reset current word
			else if(at_path == 3 && strcmp(Current_word,path)!=0){

				Current_word =NULL;
				Current_word = (char*) malloc(sizeof(char));
				continue;

			}
			Current_word =NULL;
				Current_word = (char*) malloc(sizeof(char));


		}
		else if(strcmp(Buffer, " ")!=0){

			strcat(Current_word,Buffer);

		}

}



printf("The file does not exist in the .Manfist file\n");


return -1;


}


int Remove_Manifest(char *Project_name, char * File_path){

	DIR * dir = opendir(Project_name);


	if(dir){
		
		closedir(dir);
	

	}
	else if( ENOENT == errno){
		printf("Directory does not exist!\n");
		return -1;

	}
	else if(dir == NULL){
		printf("Directory could not be opened!\n");
		return -1;
	}

	//building the file path for the file to be attempted to open
	char * Project_Path = (char* ) malloc (sizeof(Project_name)+ sizeof(".Mainfest")+3);
	
	char *Full_path = (char *) malloc( sizeof(Project_name)+sizeof(File_path)+3);//
	strcat(Full_path,"./");
	strcat(Full_path,Project_name);

	strcpy(Project_Path,Full_path);

	strcat(Project_Path,"/.Manifest");
	strcat(Full_path , "/");
	strcat(Full_path, File_path);

	
	int pfd =0;
	pfd= open(Project_Path,O_RDWR);




	if(pfd==-1){
	
	printf("The .Manifest file could not be opened or found!\n");
	return -1;

	
	}


	int success =FindPath_remove(pfd , Full_path);






	return success;
}




//////////////////////////////////////////////////////////////////add method///////////////////////////////////////////////////////////////////////////////////////////////////////
const char * MD5transform(int fd , int size ){
	unsigned char digest[16];

	char * md5 = (char*) malloc(sizeof(char)*33);
	//runescap
	char* Buffer;

	 Buffer = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
	 printf("hereree\n\n\n\n");
	 MD5((unsigned char*) Buffer, size,digest );
	  munmap(Buffer, size); 

	  for(int i=0; i<16 ;i++){
		
		printf("%02x\n",digest[i]);

		sprintf(&md5[i*2], "%02x", (unsigned int)digest[i]);

		


	  }

	  printf("md5 is : %s\n" , md5);

	  return md5;


}

/*
	this will write to the manifest file when add is called

*/
int WritetoManifest(int fd , char * File_Path , const char * MD5){

	int bytes =0;

//write to the file 
bytes = write(fd, "A", strlen("A"));
	if(bytes==- 1){
		printf("Failed to write to .Manifest file!\n");
		return -1;
	}
	bytes = write(fd ," ", strlen(" "));
	if(bytes==- 1){
		printf("Failed to write to .Manifest file!\n");
		return -1;
	}

	bytes = write(fd, "0", strlen("0"));
	if(bytes==- 1){
		printf("Failed to write to .Manifest file!\n");
		return -1;
	}

	bytes = write(fd," ",strlen(" "));
	if(bytes==- 1){
		printf("Failed to write to .Manifest file!\n");
		return -1;
	}
	bytes = write(fd, File_Path,strlen(File_Path));
	if(bytes==- 1){
		printf("Failed to write to .Manifest file!\n");
		return -1;
	}
	bytes = write(fd ," ", strlen(" "));
	if(bytes==- 1){
		return -1;
	}
	bytes = write(fd,MD5, strlen(MD5));
	if(bytes==- 1){
		printf("Failed to write to .Manifest file!\n");
		return -1;
	}
	
	bytes = write(fd, "\n",strlen("\n"));
	if(bytes==- 1){
		printf("Failed to write to .Manifest file!\n");
		return -1;
	}
	




	return 1;
}



int Manifest_containspath(char* FilePath , int fd , const char * MD5){
	
	char * Current_word =(char* ) malloc(sizeof(char));

	

	int eof = 10;

	while(eof !=0 ){


		char* Buffer =(char*) malloc(sizeof(char));

		eof = read(fd,Buffer, 1);
		if (eof == 0){
			break;
		}





		//if the current Buffer is space then that means we have found the end of the word
		//or if Buffer is the new line meaning we are looking at a new line which is a new file path
		if(strcmp(Buffer," ")==0 || strcmp(Buffer,"\n")==0){
			printf("CurrentWord=:%s \n",Current_word);


			if(strcmp(Buffer,"\n")==0){


				Current_word = NULL;

				Current_word = (char*) malloc(sizeof(char));
				continue;
				//continue


			}
			//if the currentword is equal to file path then we break out of loop because 
			//we already have written this file path with everything to the file already
			if(strcmp(FilePath,Current_word)==0){

				printf("here  where matches in file\n\n\n");

				return -1;

			}//if it does not equal the file path then proceed to the end of the file 
			//if the file is not found by the end of this loop the we seek to the end of the file 
			// and then write
			else if(strcmp(FilePath,Current_word)!=0){

				Current_word = NULL;

				Current_word = (char*) malloc(sizeof(char));
				//continue
				continue;

			}
		}
		else if(strcmp(Buffer," ")!= 0){

			strcat(Current_word,Buffer);


		}

	}

	//if we make it here that means that the path is not in the file so we write the new path

	int success = WritetoManifest(fd ,FilePath,MD5);

	if(success == -1){

		printf("Failed to write to File!");
		return -1;
	}




	return 1;


}




int updateManifest(int fd,  const char *md5 ,char * FilePath){

	struct stat st;

stat(FilePath,&st);

int size =st.st_size;



if(size == 2){
	/*
		this means that the mainfest file only has the original zero 
		in the file and we do not need to check if the file path is already in the file
	*/


	int bytes = 0; //this will hold the read bytes for the reads done in the file 
	

	/*
		this writes to the mainfest file in the format
		Version FilePath hash A , the A is the fact that the file has not been modified
	*/
	lseek (fd ,2 ,SEEK_CUR);
	bytes = write(fd, "A", strlen("A"));
	if(bytes==- 1){
		printf("Failed to write to .Manifest file!\n");
		return -1;
	}
	bytes = write(fd," ",strlen(" "));
	if(bytes==- 1){
		printf("Failed to write to .Manifest file!\n");
		return -1;
	}

	bytes = write(fd, "0", strlen("0"));
	if(bytes==- 1){
		printf("Failed to write to .Manifest file!\n");
		return -1;
	}
	bytes = write(fd," ",strlen(" "));
	if(bytes==- 1){
		printf("Failed to write to .Manifest file!\n");
		return -1;
	}
	bytes = write(fd, FilePath,strlen(FilePath));
	if(bytes==- 1){
		printf("Failed to write to .Manifest file!\n");
		return -1;
	}
	bytes = write(fd ," ", strlen(" "));
	if(bytes==- 1){
		printf("Failed to write to .Manifest file!\n");
		return -1;
	}
	bytes = write(fd,md5, strlen(md5));
	if(bytes==- 1){
		printf("Failed to write to .Manifest file!\n");
		return -1;
	}

	
	bytes = write(fd, "\n",strlen("\n"));
	if(bytes==- 1){
		printf("Failed to write to .Manifest file!\n");
		return -1;
	}

	return 1;



}/*
	this if statement is if there is a file in the manifest file already 
	if the file is encountered and has the same file path then we get an error 
	saying that the file has been written to the file 
	however if not then we add to the .manifest file

*/
else if(size>2){

	/*
		method to traverse through file and find if the file path has is 
		found inside the file it will return and int
		-1 if not found and 1 if found
	*/

	
	int  success = Manifest_containspath(FilePath ,  fd , md5);

	if(success == -1){
		//this means that the path was ffound in the file already so 
		printf("Path already exists in .Manifest File!\n");
		return -1;
	}

}



return 1;


}





int add(char * Project_name , char * File_name ){


	/*
		check if the file that we are gonna add to the manifest file exists
		if not then we get an error and do not add it to the manifest file
	*/
/*
		checking to see if the folder exits and if it does then it just closes the
		directory and skips rest of if statements
	*/
	DIR * dir = opendir(Project_name);


	if(dir){
		
		closedir(dir);
	

	}
	else if( ENOENT == errno){
		printf("Directory does not exist!\n");
		return -1;

	}
	else if(dir == NULL){
		printf("Directory could not be opened!\n");
		return -1;
	}





	//building the file path for the file to be attempted to open
	char * Project_Path = (char* ) malloc (sizeof(Project_name)+ sizeof(".Mainfest")+3);
	
	char *Full_path = (char *) malloc( sizeof(Project_name)+sizeof(File_name)+3);//
	strcat(Full_path,"./");
	strcat(Full_path,Project_name);

	strcpy(Project_Path,Full_path);

	strcat(Project_Path,"/.Manifest");
	strcat(Full_path , "/");
	strcat(Full_path, File_name);

	int fd=0;
	fd = open(Full_path,O_RDONLY);



	//this means that the file we are looking for does not exist so 
	//the method fails
	if(fd == -1){
		printf("The File that was inputted does not exist!\n");

		return -1;
	}


struct stat st;



stat(Full_path,&st);

 int size;
 
  if (stat(Full_path, &st) == 0) {

        size = (int)st.st_size;
	
	}


//buffer to store our hashed file
 const char * md5 = (char*) malloc(sizeof(char)*33);
 

//function to store the md5 has string
md5= MD5transform(fd,size);
close(fd);//closes the file given in arguemnet because it has been hashed

//write to manifest file
int pfd =0;
pfd= open(Project_Path,O_RDWR);




if(pfd==-1){
	
	printf("The .Manifest file could not be opened or found!\n");
	return -1;
}


	
	//already opened manifest file now will update it if Full_Path is not found
//FullPath = ./project3/there1.txt 
int update = updateManifest(pfd,md5,Full_path);






	






	return update;
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
					
					int Create_Folder =CreateProjectfolder(foldername);

					if(Create_Folder==-1){
						return -1;

					}
			

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

				
				//if number of files is reached this means that it is writing to last file
				//however if the size of this file is zero then we write nothing so we break out 
				//of it
				if( size ==0){
					i++;

					if(i==numofFiles){
						break;
					}
				}
				else if(size==0){
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
			else if( size >0 && File_created==1 ){
				printf("here2\n\n\n");
				i++;
				
				int o = open(filename, O_RDWR);
				file_content= (char*) malloc(sizeof(char)*size);
				strcpy(file_content, Currentword);
				int sz=write(o,file_content,strlen(file_content));
				printf("size:%d \n", size);
			
			Currentword=NULL;
				Currentword =(char*) malloc(sizeof(char));
				Getfsize==0;
				File_created==0;
				size==0;
				if(i==numofFiles){
						break;
					}
				

				
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
printf("argc: %d\n", argc);
for(int i=0; i<argc;i++){


/////////////////////////////////configure////////////////////////////////////////////////////////////////////////////////////////////////

	if(strcmp(argv[i],"configure")==0){
		printf("argcasdasdsadasd: %d\n", i);
		if(argc!=4){
			printf("too little arguemnts in configure command!\n");
			return -1;

		}

		int success = 0;
		//write the configure file
		success =configure(argv[i+1],argv[i+2]);

		if(success ==1){
			printf("configure command was completed successfully\n");
			return 0;
		}
		else if(success ==-1){
			printf("configure command was failed!\n");
			return -1;
			
		}
		
		
			break;
	/////////////////////////////////////////create///////////////////////////////////////////////////////////////////////////////////
	}
	else if(strcmp(argv[i],"create")==0){

		if(argc<3){
			printf("Too few arguements create command failed\n");
			return -1;
		}
		else if(argc>3){
			printf("Too mant arguements create command failed\n");
			return -1;
		}

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
		char * command =(char*) malloc(sizeof(argv[i])+1);
		command ="create:";
		char * project_name =(char*) malloc(sizeof(argv[i+1])+1);
		strcpy(project_name,argv[i+1]);
		strcat(project_name,":");
		//sends command to server
		sendtoServer(pass, command);
		sendtoServer(pass,project_name);


		
		int valread=0; // return the bytes read after read is called


		int File_Success =recieveFilefromServer(pass , argv[i+1]);

		/*
			if the recieveFilefromServver returns a -1 this means that the project folder is already
			found in the server
		*/

		if(File_Success==-1){
			printf("Create command has failed, Project folder already exists or failed to Create folder!\n");
		}
		else if(File_Success ==1){
			printf("Create command was successfully executed!\n");
		}
	

		
			


			break;

		}
		else if(success == -1){
			printf("ERROR! Configure command was not called before this command!\n");
			return -1;
		}
		




















/////////////////////////////////////////////destroy///////////////////////////////////////////////////////////////////

	}//this means that create was called
	else if(strcmp(argv[i],"destroy")==0){

		if(argc<3){
			printf("Too few arguements create command failed\n");
			return -1;
		}
		else if(argc>3){
			printf("Too many arguements create command failed\n");
			return -1;
		}
		
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

		char * command =(char*) malloc(sizeof(argv[i])+1);
		command ="destroy:";

		char * project_name =(char*) malloc(sizeof(argv[i+1])+1);
		
		strcpy(project_name,argv[i+1]);
		strcat(project_name,":");

		sendtoServer(pass,command);
		sendtoServer(pass, project_name);


		//still need to read from file to make sure project was destroyed\
		//sending me "Deleted"
		//sending "failed" when project name doesnt exists


	
		}
		else if(success == 0){
			printf("ERROR! Configure command was not called before this command!\n");
			return -1;
		}
		


///////////////////////////////////////add///////////////////////////////////////////////////////////////////////////////////////////////

	}
	else if(strcmp(argv[i],"add")==0){
		/*
		If the file doesn’t exist in the manifest, add the new entry to the manifest.
		If it already does, output a warning saying that the file already exists. 
		You can tag the file entry in the manifest with “M” letting you know later that it was 
		modfied. But you dont change the hash or anything.
		
		*/
		if(argc>4){
			printf("Too many arguements, add command failed!\n");
			return -1;
		}
		else if(argc<4){
			printf("Too few arguements, add command failed!\n");
			return -1;
		}
		int success =0;
		success =add(argv[i+1],argv[i+2]);
		if(success== -1){
			printf("Add command has failed!\n");
			return -1;
		}
		else if( success == 1){
			printf("Add command has been completed!\n");
			return 1;
		}
		


	}////////////////////////////////////////////////////////////////remove////////////////////////////////////////////////////////////////////////////////
	else if(strcmp(argv[i],"remove")==0){

		if(argc>4){
			printf("Too many arguements, add command failed!\n");
			return -1;
		}
		else if(argc<4){
			printf("Too few arguements, add command failed!\n");
			return -1;
		}

		int success =0;
		success = Remove_Manifest(argv[i+1],argv[i+2]);


		if(success == 1){
			printf("The remove command has been completed\n");
			return -1;

		}
		else if(success == -1){
			printf("The remove command has failed!\n");
			return -1;
		}
















	}
	

}

 

	return 0;
}
