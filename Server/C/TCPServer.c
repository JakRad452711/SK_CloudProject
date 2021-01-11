/* 
// this file was created specifically for "SK_CloudProject" project.
// the "SK_CloudProject" is created as a collage project for a subject "Sieci Komputerowe".
// this file contains source code responsible for server's TCP communication and logic.
*/

#include <arpa/inet.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "TCPServer.h"
#include "../../Shared/Actions.h"
#include "../../Shared/Errors.h"
#include "../../Shared/Paths.h"
#include "../../Shared/Values.h"

int main(int argc, char** argv) {
	int action;
	int connection;
	int portNumber;
	int sizeOfSockaddr_in;
	int socketFd;
	char buffer[BUFFER_SIZE];
	char* remainingCharacters;
	struct sockaddr_in endpoint;
	struct sockaddr_in thisMachine;

	if(argc < 2) {
		puts("(TCP server) too few arguments were entered");
		puts("(TCP server) please enter a port number as an argument");
		return ERROR_BAD_ARGUMENTS;
	}

	remainingCharacters = NULL;
	portNumber = (int) strtol(argv[1], &remainingCharacters, 10);
	sizeOfSockaddr_in = sizeof(struct sockaddr_in);

	if(argv[1] == remainingCharacters || portNumber < 0 || portNumber > 65535) {
		puts("(TCP server) wrong port number was entered");
		return ERROR_WRONG_INPUT;
	}

	printf("(TCP server) port number: %d\n", portNumber);

	thisMachine.sin_family = AF_INET;
	thisMachine.sin_port = htons(portNumber);
	thisMachine.sin_addr.s_addr = htonl(INADDR_ANY);
	
	puts("(TCP server) variables prepared");
	
	if((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		puts("(TCP server) a socket error occured");
		return ERROR_SOCKET;
	} puts("(TCP server) socket prepared");

	if(bind(socketFd, (struct sockaddr*) &thisMachine, sizeOfSockaddr_in) < 0) {
		puts("(TCP server) bind error occured");
		return ERROR_BIND;
	} puts("(TCP server) after bind");

	if(listen(socketFd, 100) < 0) {
		puts("(TCP server) listen error occured");
		return ERROR_LISTEN;
	} puts("(TCP server) after listen\n");

	while(1) {
		if((connection = accept(socketFd, (struct sockaddr*) &endpoint, &sizeOfSockaddr_in)) < 0) {
			puts("(TCP server) accept error occured");
			continue;
		}
		else {
			printf("(TCP server) connection from: %s:\n", inet_ntoa(thisMachine.sin_addr));
			
			if(fork() == 0) {
				puts("(TCP client) child process greets");
				
				while(1) {
					puts("(TCP server) on while start");
					
					// variables used to check if user can perform an action
					int numberOfRequestFormFields;
					char actionType[BUFFER_SIZE];
					char login[BUFFER_SIZE];
					char password[BUFFER_SIZE];
					char* requestFormStrtok;
					char* requestFormField;
					char* userFileLineField;
					char* userCredentialsFileLine;
					FILE* userCredentialsFile;
					// variables used by some switch cases
					char fileName[BUFFER_SIZE];
					char sendFrom[BUFFER_SIZE];
					char saveToLocation[BUFFER_SIZE];
					long fileSize;
					FILE* fileWithFileNames;
					// auxiliary variables
					int canTheActionBePerformed;
					int canUserPerformTheAction;
					char* filePath;
					size_t userFileLineSize;
					
					numberOfRequestFormFields = 0;
					
					if(receiveRequestFormTCP(connection, buffer, BUFFER_SIZE) != 0) {
						continue;
					} printf("(TCP server) received from a client:\n%s\n", buffer);
					
					requestFormStrtok = buffer;
					
					char requestFormFieldContent[5][BUFFER_SIZE];
					
					while(numberOfRequestFormFields < 5 && (requestFormField = strtok_r(requestFormStrtok, "\n", &requestFormStrtok))) {
						sprintf(requestFormFieldContent[numberOfRequestFormFields++], "%s", requestFormField);
					}
					
					if(numberOfRequestFormFields < 5) {
						puts("(TCP server) invalid request form was passed");
						sprintf(buffer, "DENIED\nThe request form consists of wrong number of fields (ERROR)");
						sendResponseFormTCP(connection, buffer, BUFFER_SIZE);
						continue;
					} puts("(TCP server) after checking request form number of fields");
					
					sprintf(login, "%s", requestFormFieldContent[0]);
					sprintf(password, "%s", requestFormFieldContent[1]);
					sprintf(actionType, "%s", requestFormFieldContent[2]);
					sprintf(sendFrom, "%s", requestFormFieldContent[3]);
					sprintf(saveToLocation, "%s", requestFormFieldContent[3]);
					sprintf(fileName, "%s", requestFormFieldContent[4]);
					
					if((userCredentialsFile = fopen(USER_CREDENTIALS_FILE, "r")) == NULL) {
						puts("(TCP server) fopen failed");
						sprintf(buffer, "DENIED\nSomething went wrong on the server end");
						sendResponseFormTCP(connection, buffer, BUFFER_SIZE);
						continue;
					} puts("(TCP server) opened user credentials file");
					
					canUserPerformTheAction = 0;
					userCredentialsFileLine = NULL;
					userFileLineSize = 0;
					
					// go through file containing users' credentials
					while(getline(&userCredentialsFileLine, &userFileLineSize, userCredentialsFile) >= 0) {
						
						userFileLineField = strtok(userCredentialsFileLine, " ");
						
						// check if the line contains the login specified in request form if not check next line
						if(!(strcmp(login, userFileLineField) == 0)) 
							continue;
						
						userFileLineField = strtok(NULL, " ");
							
						// check if the users' passwords match if not go to the next line
						if(!(strcmp(password, userFileLineField) == 0))
							continue;
						
						userFileLineField = strtok(NULL, " ");
						
						if(
							strcmp(actionType, "LOG_IN") == 0 || 
							strcmp(actionType, "DOWNLOAD") == 0
						)	canUserPerformTheAction = 1;
							
						if(
							(
								strcmp(actionType, "UPLOAD") == 0 || 
								strcmp(actionType, "MKDIR") == 0
							) && 
							strcmp(userFileLineField, "n") == 0
						)	canUserPerformTheAction = 1;
						
					} puts("(TCP server) after checking user's credentials");
					
					free(userCredentialsFileLine);
					fclose(userCredentialsFile);
					
					canTheActionBePerformed = 0;
					
					if(!canUserPerformTheAction) {
						puts("(TCP server) a user can't perform an action");
						sprintf(buffer, "DENIED\nUser can't perform the action");
					}

					if(canUserPerformTheAction && strcmp(actionType, "LOG_IN") == 0) {
						puts("(TCP server) LOG_IN");
						canTheActionBePerformed = 1;
						action = TCP_CONTINUE;
					}

					filePath = connectStrings(SERVER_DIRECTORY, sendFrom, fileName);

					if(
						canUserPerformTheAction && 
						strcmp(actionType, "DOWNLOAD") == 0 && 
						!strstr(sendFrom, ".") && 
						!strstr(sendFrom, "~") && 
						!strstr(fileName, "/") && 
						!(fopen(filePath, "r") == NULL)
					) {
						puts("(TCP server) DOWNLOAD");
						canTheActionBePerformed = 1;
						action = TCP_SEND_FILE;
					}
					else if(strcmp(actionType, "DOWNLOAD") == 0) {
						free(filePath);
						filePath = connectStrings(sendFrom, fileName, "");
						printf("(TCP server) file couldn't be opened:\n%s\n", filePath);
						sprintf(buffer, "DENIED\nFile: '%s' couldn't be opened", filePath);
					} free(filePath);
					
					if(
						canUserPerformTheAction &&
						strcmp(actionType, "UPLOAD") == 0 &&
						!strstr(saveToLocation, ".") && 
						!strstr(saveToLocation, "~") &&
						!strstr(fileName, "/")
					) {
						puts("(TCP server) UPLOAD");
						canTheActionBePerformed = 1;
						action = TCP_RECEIVE_FILE;
					}
					else if(strcmp(actionType, "UPLOAD") == 0) {
						filePath = connectStrings(saveToLocation, fileName, "");
						printf("(TCP server) invalid data was received:\n%s\n", filePath);
						sprintf(buffer, "DENIED\nFiles path can't contain '.', '~'\nFiles name can't contain '/'");
						free(filePath);
					}
					
					if(canUserPerformTheAction && canTheActionBePerformed) {
						if(strcmp(actionType, "LOG_IN") == 0)
							sprintf(buffer, "ACCEPTED\nUser login and password were correct");
						
						if(strcmp(actionType, "DOWNLOAD") == 0)
							sprintf(buffer, "ACCEPTED\nFile can be downloaded");
						
						if(strcmp(actionType, "UPLOAD") == 0)
							sprintf(buffer, "ACCEPTED\nFile can be uploaded");
					}
					
					if(sendResponseFormTCP(connection, buffer, BUFFER_SIZE) != 0) {
						puts("(TCP server) sending reponse form failed");
						continue;
					}
					
					// variables shared between some switch cases
					char* defaultStorageLocation;
					defaultStorageLocation = SERVER_DIRECTORY;
					
					switch(action) {
						case TCP_RECEIVE_FILE: {
							puts("(TCP server) TCP_RECEIVE_FILE");
							
							// char fileName[BUFFER_SIZE]; (declaration on the beginning of internal while)
							// char saveToLocation[BUFFER_SIZE]; (declaration on the beginning of internal while)
							// char* defaultStorageLocation; (declaration is right before the switch statement)
							// char* filePath; (declaration on the beginning of internal while)
							// FILE* fileWithFileNames; (declaration on the beginning of internal while)
														
							// get the files' size in bytes
							if(receiveFileSizeTCP(connection, &fileSize) != 0) {
								puts("(TCP server) receive file size error occured");
								continue;
							}
							
							filePath = connectStrings(defaultStorageLocation, saveToLocation, fileName);
							
							// download the file
							if(receiveFileTCP(connection, fileName, filePath, fileSize, BUFFER_SIZE) != 0) {
								puts("(TCP server) download file error occured");
								free(filePath);
							}
							else {

								printf("(TCP server) a file was downloaded:\n%s\n", filePath);
								
								filePath = connectStrings(defaultStorageLocation, FILE_NAMES, "");
								
								// if suceeded create new path in the file containing available files
								if((fileWithFileNames = fopen(filePath, "a")) == NULL) {
									puts("(TCP server) fopen failed (2)");
									continue;
								} free(filePath);
								
								filePath = connectStrings(saveToLocation, fileName, "\n");
								
								fputs(filePath, fileWithFileNames);
									
								free(filePath);
								fclose(fileWithFileNames);
							}
														
						} break;
						
						case TCP_SEND_FILE: {
							puts("(TCP server) TCP_SEND_FILE");
							
							// char fileName[BUFFER_SIZE]; (declaration on the beginning of internal while)
							// char sendFrom[BUFFER_SIZE]; (declaration on the beginning of internal while)
							FILE* sentFile;
							
							memset(buffer, 0, BUFFER_SIZE);
							
							filePath = connectStrings(defaultStorageLocation, sendFrom, fileName);
							
							if((sentFile = fopen(filePath, "r")) == NULL) {
								puts("(TCP server) fopen failed (3)");
								continue;
							} free(filePath);
							
							// send the files' size in bytes
							if(sendFileSizeTCP(connection, sentFile, MAX_FILE_SIZE) != 0) {
								puts("(TCP server) send file size failed.");
								continue;
							}
							
							// send the file
							if(sendFileTCP(connection, sentFile, BUFFER_SIZE) != 0) {
								puts("(TCP server) send file failed.");
								continue;
							}
														
						}	break;
						
						case TCP_CONTINUE: {
							puts("(TCP server) TCP_CONTINUE");
							
							continue;
						}
					}
				}
			}
		}
	}

	return 0;
}

char* connectStrings(char* str1, char* str2, char* str3) {
	int sizeOfAllStrings = sizeof(str1) + sizeof(str2) + sizeof(str3);
	char* returnString;
	
	returnString = malloc(sizeOfAllStrings + 1);
	sprintf(returnString, "%s%s%s", str1, str2, str3);
	
	return returnString;
}

int sendResponseFormTCP(int socketFd, char* aForm, int formSizeInBytes) {
	if(send(socketFd, aForm, formSizeInBytes, 0) < 0)
		return ERROR_SEND;

	return 0;
}

int sendFileSizeTCP(int socketFd, FILE* aFile, long maxSizeInBytes) {
	fseek(aFile, 0, SEEK_END);
	long fileSize = ftell(aFile);
	fseek(aFile, 0, SEEK_SET);

	if(maxSizeInBytes > maxSizeInBytes)
		return ERROR_FILE_SIZE;

	send(socketFd, &fileSize, sizeof(long), 0);
	
	return 0;
}

int sendFileTCP(int socketFd, FILE* aFile, int bufferSizeInBytes) {
	fseek(aFile, 0, SEEK_END);
	long fileSize = ftell(aFile);
	fseek(aFile, 0, SEEK_SET);

	int numberOfPackages = ceil((double) fileSize / (double) bufferSizeInBytes);
	char buffer[bufferSizeInBytes];

	for(int i=0 ; i<numberOfPackages ; i++) {
		memset(buffer, 0, bufferSizeInBytes);
		fread(buffer, 1, bufferSizeInBytes, aFile);
		send(socketFd, buffer, bufferSizeInBytes, 0);
	}

	return 0;
}

int receiveRequestFormTCP(int socketFd, char* buffer, int formSizeInBytes) {
	if(recv(socketFd, buffer, formSizeInBytes, 0) < 0)
		return ERROR_RECEIVE;

	return 0;
}

int receiveFileSizeTCP(int socketFd, long* fileSize) {
	if(recv(socketFd, fileSize, sizeof(long), 0) < 0)
		return ERROR_RECEIVE;
		
	if(*fileSize > MAX_FILE_SIZE) {
		return ERROR_FILE_SIZE;
	}
		
	return 0;
}

int receiveFileTCP(int socketFd, char* fileName, char* saveToLocation, long fileSize, int bufferSizeInBytes) {
	int numberOfPackages;
	int received;
	char buffer[bufferSizeInBytes];
	char* filePath;
	FILE* newFile;
	
	numberOfPackages = ceil((double) fileSize / (double) bufferSizeInBytes);
	filePath = connectStrings(saveToLocation, fileName, "");
	
	if((newFile = fopen(filePath, "w")) == NULL)
		return ERROR_OPEN_FILE;
	
	free(filePath);
	
	for(int i=0 ; i<numberOfPackages ; i++) {
		received = 0;
		
		while(received < bufferSizeInBytes) {
			received += recv(socketFd, buffer + received, bufferSizeInBytes - received, 0);
		}

		int packageLength = ((i == (numberOfPackages - 1)) ? fileSize % bufferSizeInBytes : bufferSizeInBytes);
		fwrite(buffer, 1, packageLength, newFile);
	}
	
	fclose(newFile);

	return 0;
}