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
#include <math.h>



void sendtoServer(int fd , char*to_Send){

		send(fd , to_Send , strlen(to_Send) , 0 );// the command name for the server to process and read
	

}

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
//////////////////////////////////////////////////////////////Commit method//////////////////////////////////////////////////////////////////////////////////////////









int TraverseClientM(int SFD ,int CFD, int Ssize, int Csize, int Bytesread, int Commit_fd, char* Project_File){
	/*
	Traverse through the entirty of the Servers manifest and compare it to the clients manifest 
	

	*/

	int bytes= 0;

	// Contents needed to hold server entry things
	char * SVersion =NULL;
	char * SCurrent_file_path = NULL;
	
	char * Sword =(char *) malloc(sizeof(char));
	char * SBuffer = (char * ) malloc(sizeof(char));

	//contents needed to hold client Entry Things
	char * CTag = NULL;
	char * CVersion =NULL;
	char * CCurrent_file_path = NULL;
	char * CLive_hash = NULL;
	char * Cword =(char *) malloc(sizeof(char));
	char * CBuffer = (char * ) malloc(sizeof(char));
	int Removefound =0;
	int max =0;

	if(Ssize>Csize){

		max = Ssize;
	}
	else if(Ssize<Csize){

		max =Csize;
	}
	else{
		max =Csize;
	}








	//this loop goes until the bytesread != size of manifest file bytes
	while(Bytesread != max){
		//only start reading from servers file when we have gotten the clients tag
		if(CTag != NULL && Bytesread < Ssize){
		bytes  = read(SFD , SBuffer, 1);

		if(bytes == -1){
			printf("could not read from file!\n");
			return -1;
		}
		}
		bytes = read(CFD, CBuffer , 1);

		if(bytes = 0){
			printf("Reached end of Client .manifest file!\n");
			break;
		}

		if(bytes == -1){
			printf("could not read from file!\n");
			return -1;
		}
			Bytesread++;

		//if both are pointing to the same things then we encounter spaces and new lines at the same time
		//since we are only reviewing for the size of the Server manifest file length first
		if(strcmp(CBuffer, " ") == 0 || strcmp(CBuffer,"\n") == 0){
			//meaning we have not filled in a tag of the clients manifest entry yet then we fill it in
			if(CTag == NULL){
				CTag = (char * ) malloc(strlen(Cword)+1);
				strcpy(CTag,Cword);
				if(strcmp(CTag, "R")== 0){//meaning that the entry does not exist anymore
				Removefound++;
				}
				free(Cword);
				Cword =NULL;
				Cword= (char*) malloc(sizeof(char));
				continue;
			}
			else if(CVersion == NULL){

				if(Bytesread<=Ssize){
				SVersion = (char *) malloc(strlen(Sword)+1);
				strcpy(SVersion,Sword);
				free(Sword);
				Sword =NULL;
				Sword= (char*) malloc(sizeof(char));
				}

				CVersion =(char *) malloc(strlen(Cword)+1);
				strcpy(CVersion,Cword);
				free(Cword);
			 	Cword =NULL;
				
				Cword= (char*) malloc(sizeof(char));
			}
			else if(CCurrent_file_path == NULL){
				if(Bytesread<=Ssize){
				SCurrent_file_path = (char *) malloc(strlen(Sword)+1);
				strcpy(SCurrent_file_path,Sword);
				free(Sword);
				Sword =NULL;
				Sword= (char*) malloc(sizeof(char));
				}


				CCurrent_file_path =(char*) malloc(strlen(Cword)+1);
				strcpy(CCurrent_file_path,Cword);
				
				free(Cword);
				
				Cword =NULL;
				
				Cword= (char*) malloc(sizeof(char));

				
			}
			else if(CLive_hash == NULL){

				if(Bytesread<=Ssize){

				if(strcmp(Cword,Sword) != 0){//if both hashes are different then we check to if the Server file Version is less than then Clients file version
				//Servers file version num cannot be lower than clients , if it is then ask user to synch with repository before committing changes 
				int ServerV =atoi(Sword);
				int ClientV = atoi(Cword);

				if(ServerV>ClientV){//if server file version is greater and hashes are different then delete commit file and ask to sych
					printf("the client must synch with the repository before committing changes!\n");

					int status = remove(Project_File);//Project file has path to file
					if(status == -1){
					printf("failed to delete .Commit from project folder!\n");
					return -1;
					}
					return -1;//since it cant do anything more until client is syched

				



				}





				}
				//if we get to the end of the entry and RemoveFound ==1 then we cant do anything with it and write it to the .commit
				//file so it can be deleted on the Server side
				if(Removefound == 1){

				bytes = write(Commit_fd, "D", strlen("D"));
				bytes = write(Commit_fd, " ", strlen(" "));
				bytes = write(Commit_fd, CVersion, strlen(CVersion));
				bytes = write(Commit_fd, " ", strlen(" "));
				bytes = write(Commit_fd, CCurrent_file_path, strlen(CCurrent_file_path));
				bytes = write(Commit_fd, " ", strlen(" "));
				bytes = write(Commit_fd, Cword, strlen(Cword));
				bytes = write(Commit_fd, "\n", strlen("\n"));

				//free memory and reset storage variables
				free(Cword);
				free(Sword);
				free(CTag);
				free(CCurrent_file_path);
				free(SCurrent_file_path);
				free(SVersion);
				free(CVersion);
				CTag = NULL;
				CCurrent_file_path =NULL;
				SCurrent_file_path = NULL;
				SVersion = NULL;
				CVersion = NULL;
				Cword =NULL;
				Sword =NULL;
				Cword= (char*) malloc(sizeof(char));
				Sword= (char*) malloc(sizeof(char));
				Removefound = 0;//reset removefound;
				

				}



				//if the both hashes from client and server are the same check the live hash of the client
				if(strcmp(Cword,Sword)==0 ){

				//now we havwe access to the Servers hash for the current file
				//open the current file in client side and get the hash and compare it to the server hash
				//if different hashes then add 'M' infront of the entry when adding to the .commit file since 
				//client file has been changed;

				int Filefd = open(CCurrent_file_path , O_RDONLY);//open file path

				if(Filefd == -1){

					printf("Failed to open a file in .Manifest to get hashcode!\n");
					return -1;
				}

				struct stat st;


			//getting the size of the client manifest file in bytes
			stat(CCurrent_file_path,&st);

 			int size;
 
  			if (stat(CCurrent_file_path, &st) == 0) {

        		size = (int)st.st_size;
	
			}

				

				const char* livehash = (char*) malloc(sizeof(char)*33);
				livehash = MD5transform(Filefd , size); //gets live hash
				close(Filefd);

				//check if clients live hash is the same as servers hash

				if(strcmp(livehash , Sword) == 0 ){
					//they are the same hash meaning we reset our storage variables for both server and client and continue;
				free(Cword);
				free(Sword);
				free(CTag);
				free(CCurrent_file_path);
				free(SCurrent_file_path);
				free(SVersion);
				free(CVersion);
				CTag = NULL;
				CCurrent_file_path =NULL;
				SCurrent_file_path = NULL;
				SVersion = NULL;
				CVersion = NULL;
				Cword =NULL;
				Sword =NULL;
				Cword= (char*) malloc(sizeof(char));
				Sword= (char*) malloc(sizeof(char));
				continue;
				}
				else if(strcmp(livehash,Sword)!=0){
					//they do no equal the same hash so we write to the commit file and reset storage variables and continue;

				//write to commit file

				bytes = write(Commit_fd, "M", strlen("M"));
				bytes = write(Commit_fd, " ", strlen(" "));

				//increment the Version by 1
				int Versionup = atoi(CVersion);
				Versionup++;
				free(CVersion);
				CVersion =NULL;
				int digits = floor(log10(abs(Versionup)))+1;
				CVersion = (char *) malloc(sizeof(char) *digits);
				sprintf(CVersion,"%d",Versionup);

				bytes = write(Commit_fd, CVersion, strlen(CVersion));
				bytes = write(Commit_fd, " ", strlen(" "));
				bytes = write(Commit_fd, CCurrent_file_path, strlen(CCurrent_file_path));
				bytes = write(Commit_fd, " ", strlen(" "));
				bytes = write(Commit_fd, livehash, strlen(livehash));
				bytes = write(Commit_fd, "\n", strlen("\n"));

				//free memory and reset storage variables
				free(Cword);
				free(Sword);
				free(CTag);
				free(CCurrent_file_path);
				free(SCurrent_file_path);
				free(SVersion);
				free(CVersion);
				CTag = NULL;
				CCurrent_file_path =NULL;
				SCurrent_file_path = NULL;
				SVersion = NULL;
				CVersion = NULL;
				Cword =NULL;
				Sword =NULL;
				Cword= (char*) malloc(sizeof(char));
				Sword= (char*) malloc(sizeof(char));

				}


			}
			// else if(strcmp(Cword , Sword)!=0){



			// }

			}
			else if(Bytesread>Ssize){

				



			//if we get to the end of the entry and RemoveFound ==1 then we cant do anything with it and write it to the .commit
				//file so it can be deleted on the Server side
				if(Removefound == 1){

				bytes = write(Commit_fd, "D", strlen("D"));
				bytes = write(Commit_fd, " ", strlen(" "));
				bytes = write(Commit_fd, CVersion, strlen(CVersion));
				bytes = write(Commit_fd, " ", strlen(" "));
				bytes = write(Commit_fd, CCurrent_file_path, strlen(CCurrent_file_path));
				bytes = write(Commit_fd, " ", strlen(" "));
				bytes = write(Commit_fd, Cword, strlen(Cword));
				bytes = write(Commit_fd, "\n", strlen("\n"));

				//free memory and reset storage variables
				free(Cword);
			
				free(CTag);
				free(CCurrent_file_path);
				free(CVersion);
				CTag = NULL;
				CCurrent_file_path =NULL;
				CVersion = NULL;
				Cword =NULL;
				Cword= (char*) malloc(sizeof(char));
				Removefound = 0;//reset removefound;
				

				}
			//we are writing to the .commit file here since these are new entries not found in the servers manifest file
				int Filefd = open(CCurrent_file_path , O_RDONLY);//open file path

				if(Filefd == -1){

					printf("Failed to open a file in .Manifest to get hashcode!\n");
					return -1;
				}

				struct stat st;


			//getting the size of the client manifest file in bytes
			stat(CCurrent_file_path,&st);

 			int size;
 
  			if (stat(CCurrent_file_path, &st) == 0) {

        		size = (int)st.st_size;
	
			}

				

				const char* livehash = (char*) malloc(sizeof(char)*33);
				livehash = MD5transform(Filefd , size); //gets live hash
				close(Filefd);


				//write to commit file and all the added entries 

				bytes = write(Commit_fd, "A", strlen("A"));
				bytes = write(Commit_fd, " ", strlen(" "));

				bytes = write(Commit_fd, CVersion, strlen(CVersion));
				bytes = write(Commit_fd, " ", strlen(" "));
				bytes = write(Commit_fd, CCurrent_file_path, strlen(CCurrent_file_path));
				bytes = write(Commit_fd, " ", strlen(" "));
				bytes = write(Commit_fd, livehash, strlen(livehash));
				bytes = write(Commit_fd, "\n", strlen("\n"));

				//free memory and reset storage variables
				free(Cword);
				free(CTag);
				free(CCurrent_file_path);
				free(CVersion);
				
				CTag = NULL;
				CCurrent_file_path =NULL;
				CVersion = NULL;
				Cword =NULL;
				Cword= (char*) malloc(sizeof(char));
		
			}
		}





		}
		else{

			strcat(Cword, CBuffer);
			if(CTag != NULL && Bytesread < Ssize){
				strcat(Sword,SBuffer);
			}


		}















		


	}










return 1;

}


/*
The commit command will fail if the project name doesn’t exist on the server, if the server can not be contacted, 
if the client can not fetch the server's .Manifest file for the project, 
if the client has a .Update file that isn't empty (no .Update is fine) or has a .Conflict file. 
After fetching the server's .Manifest, the client should should first check to make sure that the .Manifest versions match.
 If they do not match, the client can stop immediatley and ask the user to update its local project first. 
 If the versions match, the client should run through its own .Manifest and compute a live hash for each file listed in it.
  Every file whose live hash is different than the stord hash saved in the client's local .Manifest should have an entry 
  written out to a .Commit with its file version number incremented. 
  The commit should be successful if the only differences between the server's .Manifest and the client's are:

*/

int Commit_Method(int fd , char * Project_name){

	/*
	
		fetch servers .manifest for the project specified, then compare every entry in it to the clients manifest
		check for changes and check if there are any changes on the server for the client.
	
	*/

//checking if the project folder exists if it doesnt then the method cannot be done

char *Project_Folder = (char*) malloc(strlen(Project_name)+ strlen("./")+ strlen("/.Commit"));//allocate memory for folder path
Project_Folder = "./";
strcat(Project_Folder,Project_name);
DIR * dir = opendir(Project_Folder);


	if(dir){

		//directory exists
		
		closedir(dir);
	

	}
	else if( ENOENT == errno){
		printf("Project folder does not exist, update command failed!\n");
		return -1;

	}
	else if(dir == NULL){
		printf("Project directory could not be opened, update command failed!\n");
		return -1;
	}

//sendfile
//sendfile:1:./project/.Manifest:(# of chars):(chars in manifest):

/*
	read up to files being sent and then break out of while loop so
	now you have the while loop for the second comapre while loop
*/
int numofFiles =0;//number of files
char * Word = (char*) malloc(sizeof(char)); //malloc to hold the word we need will get strcat with other things
int bytes = 0; //bytes read.
int Commit_fd =0;
int charsinfile =0;// bytes in server manifest
char * Project_Path =NULL;
char * Buffer = (char*) malloc(sizeof(char));
char * Bufferto = (char*) malloc(sizeof(char));
char * VersionServer =NULL;
int bytesread =0;
char * VersionClient =NULL;
while(1){



bytes = read(fd , Buffer, 1); // this reads the string passed in by the server

if(bytes == -1){

	printf("could not read from file!\n");
	return -1;
	}

if(strcmp(Buffer ,":" )== 0 || strcmp(Buffer, "\n")==0){

		if(strcmp(Word, "Failed") ==0 ){
		printf("Project folder was not found in server!\n");
		return -1;
	}
	

	if(strcmp(Word, "sendfile") == 0 ){
		free(Word);


		Word = NULL;
		Word = (char*) malloc(sizeof(char));
		continue;
	}//meaning that we have no reached number of files we need to know the number of files so store it 
	else if(numofFiles == 0){

		numofFiles = atoi(Word);

		free(Word);
		Word =NULL;
		Word = (char*) malloc(sizeof(char));
		continue;
	}//this means that the project path we need to open has not been filled in yet
	else if(Project_Path == NULL){

		Project_Path = (char*) malloc(strlen(Word)+1);// malloc for size of path
		strcpy(Project_Path,Word);// copy path to project path
		free(Word);
		Word = NULL;
		Word = (char*) malloc(sizeof(char));
		



	}//num of chars in manifest that we dont need so when it gets here it resets word and breaks out of loop;
	else if( charsinfile == 0){
		charsinfile = atoi(Word);
		free(Word);
		Word = NULL;
		Word = (char*) malloc(sizeof(char));
	}
	else if(VersionServer == 0){
		VersionServer = (char *) malloc(strlen(Word)+1);

		bytesread =strlen(Word)+1;//gets version number length and +1 because we read a new line
		strcpy(VersionServer,Word);
		
		free(Word);
		Word = NULL;
		Word =(char * ) malloc(sizeof(char));
		break;



	}

}
else{

	strcat(Word, Buffer);


}
}
 free(Buffer);
 Buffer =NULL;
 Buffer = (char *) malloc(sizeof(char));
 char * Wordtwo = (char *) malloc(sizeof(char));

//need to open file thats in the client to be compared to the server ones manifest.
int cfd = open(Project_Path,O_RDONLY);//open file and only be able to read it from client manifest

//get version for client version of manifest

while(bytes != 0){

bytes = read(cfd, Buffer , 1);

if(bytes == 0){

	break;//end of file
}

if(strcmp(Buffer , "\n") == 0){

	//this means we are at the version number so just add it to the version number of client;
	VersionClient  = (char *) malloc(strlen(Word)+1);
	strcpy(VersionClient,Word);//

	free(Word);
	Word = NULL;
	Word = (char * ) malloc(sizeof(char));
	break;



}
else{

	strcat(Word, Buffer);//make the word

}


}

/*
	comparing version numbers to see if the they match and if they dont notify the user
*/

if(strcmp(VersionClient,VersionServer) != 0){

	//if version do not match then ask to update local project
	printf("Please update the local project first!\n");
	free(VersionServer);
	free(VersionClient);
	return -1;


}//if they match then make the commit file 
else if(strcmp(VersionClient,VersionServer) == 0){

	strcat(Project_Folder,"/.Commit");// Project folder holds ./Project/.Commit
	//create the .Commit file

	bytes = creat(Project_Folder, S_IRWXU); //this makes the file

	if(bytes == -1){

		printf("The .Commmit file could not be made!\n");
		return -1;
	}

	Commit_fd = open(Project_Folder ,O_RDWR);//opens the .commit file so we can write to it

	if(Commit_fd == -1){
		printf("Could not open the .Commit file!\n");
		return -1;


	}
	

}

//method to go through server manifest first and check for changes////////

struct stat st;


//getting the size of the client manifest file in bytes
stat(Project_Path,&st);

 int size;
 
  if (stat(Project_Path, &st) == 0) {

        size = (int)st.st_size;
	
	}


int call_traverse = TraverseClientM(fd, cfd,charsinfile,size,bytesread,Commit_fd,Project_Folder);
if(call_traverse == -1){

	return -1;
}



	return 1;
}























///////////////////////////////////////////////////////////////////////////////////////////////CURRENT VERISION//////////////////////////////////////////////////////////////////////////////////////////////////////


/*
	this method basically prints out the version for the whole project and the file names and version of those 
	projects that are contained in the project folder

*/


int Print_Versions(char * Project_Name ,int fd){


	char * Current_word = (char *) malloc(sizeof(char));
	int bytes = 0;
	int numofFiles =0;
	int Version_of_Project= 0;
	int size_file_name= 0;
	int Version_Num =0;

//versions:(number of files):(version number of whole project):(size of file name):(version number):(file name):

while(1){

char * Buffer = (char *) malloc(sizeof(char));
		bytes = read(fd, Buffer, 1);
	

		if(bytes == -1){

			printf("could not read from file!\n");
			return -1;
		}


		if(strcmp(Buffer,":")==0){
	
			//if we encounter sendfile then that means we s
			if(strcmp(Current_word,"Version")==0){

		

				
				

				Current_word=NULL;
				Current_word =(char*) malloc(sizeof(char));

				

				

			

				continue;
			}
			else if(numofFiles ==0){
			

				numofFiles = atoi(Current_word);
			
				Current_word=NULL;
				Current_word =(char*) malloc(sizeof(char));

				continue;
			}
			else if(Version_of_Project == 0){
				Version_of_Project = atoi(Current_word);

				Current_word=NULL;
				Current_word =(char*) malloc(sizeof(char));
				break;

			}
			

		}
		else{
			

			strcat(Current_word,Buffer);
			
			
			//this means that the function has failed and the file was not sent back
			if(strcmp(Current_word,"Failed")==0){
				return -1;

			}

		}




int i=0;
char * file_name=NULL;
printf("FileName \t Versions\n");
while(i<numofFiles){
	char* Buff = (char *) malloc(sizeof(char));

	bytes = read(fd ,Buff, 1);

	if(bytes ==-1 ){
		printf("Could not read String\n");
		return -1;
	}


	if(strcmp(Current_word ,":")== 0){

		//meaning we havent gotten file name length to read; we go here
		if(size_file_name == 0){

			size_file_name = atoi(Current_word);//convert the number string to int
			
			//reset the current word string
			Current_word =NULL;
			Current_word = (char* ) malloc(sizeof(char));
			continue;
			
		}//if we dont have the version for the current file then we go in here
		else if(Version_Num == 0){
			i++;
			Version_Num = atoi(Current_word);//convert the version number to int
			file_name = (char*) malloc(size_file_name+1);
			bytes = read(fd,file_name ,size_file_name);// we are gonna read the file for however many bytes the name is
			if(bytes ==-1){
				printf("Could not read from string\n");
				return -1;
			}
			char * Buff =(char *) malloc(sizeof(char));
			bytes = read(fd,Buff ,1);// we are gonna read the file for however many bytes the name is
			if(bytes ==-1){
				printf("Could not read from string\n");
				return -1;
			}
			printf("%s \t %d\n", file_name,Version_Num);// print out the versions with file names
			size_file_name = 0;
			Version_Num = 0;
			free(file_name);
			file_name = NULL;
			//reset the current word string
			Current_word =NULL;
			Current_word = (char* ) malloc(sizeof(char));

			if(strcmp(Buff , ":")==0){
				continue;
			}
			
			


		}


	}
	else{
		// cat current word
		strcat(Current_word,Buff);


	}


}


}


	
	
	
	return 1;
	
}





//////////////////////////////////////////////////////////////////////////////CHECKOUT///////////////////////////////////////////////////////////////////////////////////
int Make_Directories(char * File_Path){



DIR* dir = opendir(File_Path);
if (dir) {
    /* Directory exists. */
    closedir(dir);
	return 1;
} else if (ENOENT == errno) {
    /* Directory does not exist. */

	int check = mkdir(File_Path,S_IRWXU);
	if (!check) {
        printf("Directory created\n");
		return 1; 
	}
    else{ 
        printf("Unable to create directory\n"); 
        return -1;
    } 

}

	 return 1;
 }

 

 int Find_Slash(char * File_Path){


	 int counter = strlen(File_Path);
	 char * Current_word = (char*) malloc(sizeof(char));

	 for(int i = 0; i<counter;i++){
		 char *Buffer = (char*) malloc(sizeof(char));
		

		//if a slash is encountered then check if the path exits
		 if(strcmp(Buffer, "/")==0){


			 if(strlen(Current_word)==2){

				 //this means we encounter a ./ so this is not a valid path
				 strcat(Current_word,Buffer);
				 continue;

			 }
			/*
				if we get here then that means that path is valid and we have to
				check if the path exists, if it doesnt then we add the path to the directory
				when we get to the end of buffer then we finally make the file


				Method will take in current word and check if the directory exists 
				if not then we will make the directory 
			*/
			
			int check =Make_Directories(Current_word);
			if(check == 1){
				//meaning the directory was made or the directory already exists
				strcat(Current_word,Buffer);
				continue;

			}
			else if(check  == -1){

				printf("Failed to make directory!\n");
				return -1;
			}






		 }
		 else{

			 strcat(Current_word,Buffer);



		 }





	 }

	 return 1; //this means that all directories have been made

 }
 
 
 


//////////////////////////////////////////////////////////////////////////REMOVE//////////////////////////////////////////////////////////////////////////////////////////////


int FindPath_remove(int fd , char * path){



//this will be to find how many bytes needed to go back to go back to the letter
int bytes_to_off = 0;

int at_path = 0;

int removefound =0; //if this is equal to 1 then that means that we have encountered a R so that entry doesnt exist

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

			if(strcmp(Current_word,"R")==0){
				removefound++;

			}

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

				if(removefound ==1){
					printf("Entry not found in the .Manifest file!\n");
					return -1;
				}


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
				removefound =0;

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

	int remove_found =0;

	

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

			if(strcmp(Current_word, "R")==0){
				printf("here\n\n\n\n\n\n\n");
				remove_found++;
			}


			if(strcmp(Buffer,"\n")==0){ //this indicates that we are moving on to a different entry
			// so we reset remove found!


				Current_word = NULL;

				Current_word = (char*) malloc(sizeof(char));
				remove_found=0;
				continue;
				//continue


			}
			//if the currentword is equal to file path then we break out of loop because 
			//we already have written this file path with everything to the file already
			if(strcmp(FilePath,Current_word)==0 && remove_found ==0){

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

			}//if path is found and remove found =1 , this means that the directory was found but was removed so it can still
			//be added to the manifest list
			else if(strcmp(FilePath,Current_word)==0 && remove_found ==1){	
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
		printf("Warning file already exists in .Manifest File!\n");
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

		if(errno == EEXIST)
    {
        // Directory already exists, do something

		printf("Project folder Already exists!\n");
		return -1;
    }



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

				/*
					we need a way to make directories and subdirectories and check if they
					exist already or not given the filename string which contains the full path to the file 
				*/
			
			/*
				//this method finds the path before a slash and then makes the directory if it doesnt exist and returns 1 if it is made and if it already existed
				returns -1 if something fails to make directory
			*/

				int f =Find_Slash(filename);

				if(f == -1){

					printf("Failed to make directory!\n");

					return -1;
				}




				
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

		char * D= (char *) malloc(strlen("Deleted") +1);
		int bytes = read(pass,D, (strlen("Deleted")+1));

		if(strcmp(D, "Deleted")==0){
			printf("The Destroy Command has been successfully executed!\n");
			break;
		}
		else if(strcmp(D, "Deleted")==0){
			printf("The Destroy command has failed to execute because the project does not exist on the server!");
			return -1;
		}



	
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
			break;
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



	}/*
		this is the checkout method
		fails if project project name doesn't exist or if the client cannot communicate with the server
		or if the configure function was not called, if it does run then it will do the following things:
		it will request the entire project from the server, the server will send over the current version
		of the project .Manifest as well as all the files that are listed in it
		client will be responsible for recieving the project , creating subdirectories under the project and putting all files
		into place as well as saving the .Manifest
	*/
	else if(strcmp(argv[i],"checkout") == 0){
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

		//edit recieve files method to also create subfolders if necessary
		char * command =(char*) malloc(sizeof(argv[i])+1);
		command ="checkout:";

		char * project_name =(char*) malloc(sizeof(argv[i+1])+1);
		
		strcpy(project_name,argv[i+1]);
		strcat(project_name,":");

		sendtoServer(pass,command);
		sendtoServer(pass, project_name);





		int  recieved =recieveFilefromServer(pass,argv[i+1]);


		if(recieved==-1){
			printf("CHECKOUT command has failed, Project folder already exists or failed to Create folder!\n");
			break;
		}
		else if(recieved ==1){
			printf("checkout command was successfully executed!\n");
			break;
		}







	}
	else if(success == 0){
		printf("ERROR! Configure command was not called before this command!\n");
			return -1;

	}
	

}///////////////////////////////////////////////////current version////////////////////////////////////////////////////////////////////////////////////////////////////
else if(strcmp(argv[i],"currentversion")==0){





if(argc<3){
			printf("Too few arguements currentversion command failed\n");
			return -1;
		}
		else if(argc>3){
			printf("Too many arguements currentversion command failed\n");
			return -1;
		}

		//this will call the findconfig method which returns and int of 1 if found or  -1 if not found
		//checks the directory to see if the config file was found
		int success =findConfig();
		printf("herereere: %d\n\n\n\n\n", success);

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

		//edit recieve files method to also create subfolders if necessary
		char * command =(char*) malloc(sizeof(argv[i])+1);
		command ="currentversion:";

		char * project_name =(char*) malloc(sizeof(argv[i+1])+1);
		
		strcpy(project_name,argv[i+1]);
		strcat(project_name,":");

		sendtoServer(pass,command);
		sendtoServer(pass, project_name);
		//being read like versions:(number of files):(version number of whole project):(size of file name):(version number):(file name):


		int versions = Print_Versions(argv[i+1],pass);

		if(versions == -1){
			printf("CurrentVersion command has failed\n");
			break;
		}
		else if(versions ==1){
			printf("CurrentVersion command was successfully executed!\n");
			break;
		}


	}
	else if(success == 0){
		printf("ERROR! Configure command was not called before this command!\n");
			return -1;

	}




}
else if(strcmp(argv[i],"commit")==0){


//need too
if(argc<3){
			printf("Too few arguements update command failed\n");
			return -1;
		}
		else if(argc>3){
			printf("Too many arguements update command failed\n");
			return -1;
		}

		//this will call the findconfig method which returns and int of 1 if found or  -1 if not found
		//checks the directory to see if the config file was found
		int success =findConfig();

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
			
			//edit recieve files method to also create subfolders if necessary
		char * command =(char*) malloc(sizeof(argv[i])+1);
		command ="commit:";

		char * project_name =(char*) malloc(sizeof(argv[i+1]));
		
		
		strcpy(project_name,argv[i+1]);
		strcat(project_name,":");

		sendtoServer(pass,command);
		sendtoServer(pass, project_name);

		//checking to see if .update files exist and are empty same with conflict files


		int success = Commit_Method( pass, project_name);




















		if(success == -1){
			printf("The commit command has failed!\n");
			return -1;
		}
		else if(success == 1){

			printf("The commit command was successful!\n");
			return 0;
		}











		}
		else if(success == 0){
		printf("ERROR! Configure command was not called before this command!\n");
			return -1;

	}



























	
}

 

	
}
	
return 0;
}
