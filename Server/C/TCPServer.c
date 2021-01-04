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

	puts("TCP server program started");

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
				while(1) {
					// variables used to check if user can perform an action
					int numberOfRequestFormFields;
					char actionType[BUFFER_SIZE];
					char login[BUFFER_SIZE];
					char password[BUFFER_SIZE];
					char* requestForm;
					char* requestFormField;
					// variables used by some switch cases
					char fileName[BUFFER_SIZE];
					char sendFrom[BUFFER_SIZE];
					char newDirectoryPath[BUFFER_SIZE];
					// auxiliary variables
					char fileSizeChar[BUFFER_SIZE];
					
					numberOfRequestFormFields = 0;
					
					if(receiveRequestFormTCP(connection, buffer, BUFFER_SIZE) != 0) {
						continue;
					}
					
					requestForm = strtok(requestForm, "\n");
					requestFormField = requestForm;
					
					char requestFormFieldContent[6][BUFFER_SIZE];
					
					while(requestFormField != NULL && numberOfRequestFormFields < 6) {
						requestFormField = strtok(NULL, "\n");
						sprintf(requestFormFieldContent[numberOfRequestFormFields++], "%s", requestFormField);
					}
					
					if(numberOfRequestFormFields < 6) {
						puts("(TCP server) invalid request form was passed");
						sprintf(buffer, "DENIED\nThe request form consists of wrong number of fields (ERROR)");
						sendResponseFormTCP(connection, buffer, BUFFER_SIZE);
						continue;
					}
					
					login = requestFormFieldContent[0];
					password = requestFormFieldContent[1];
					actionType = requestFormFieldContent[2];
					newDirectoryPath = requestFormFieldContent[3];
					sendFrom = requestFormFieldContent[3];
					fileSizeChar = requestFormFieldContent[4];
					fileName = requestFormFieldContent[5];
					
					remainingCharacters = NULL;
					fileSize = (int) strtol(fileSizeChar, &remainingCharacters, 10);

					if(fileSizeChar == remainingCharacters || fileSize > MAX_FILE_SIZE) {
						puts("(TCP server) wrong file size number was entered");
						continue;
					}
					
					// check if user can perform an action
					// create response
					
					if(sendResponseFormTCP(connection, buffer, BUFFER_SIZE) != 0) {
						continue;
					}
					
					// variables shared between some switch cases
					char* defaultStorageLocation;exit
					
					switch(action) {
						case TCP_RECEIVE_FILE: {
						 // char fileName[BUFFER_SIZE]; (declaration on the beginning of internal while)
							char saveToLocation[BUFFER_SIZE];
						 // char* defaultStorageLocation; (declaration is right before the switch statement)
							long fileSize;
							
							defaultStorageLocation = SERVER_DIRECTORY;
							
							// get the files' size in bytes
							if(receiveFileSizeTCP(connection, &fileSize) != 0) {
								puts("(TCP server) receive file size error occured");
								continue;
							}
							
							// download the file
							if(receiveFileTCP(connection, fileName, strcat(defaultStorageLocation, saveToLocation), fileSize, BUFFER_SIZE) != 0) {
								puts("(TCP server) download file error occured");
							}
							else {
								printf("(TCP server) a file was downloaded:\n%s\n", strcat(strcat(defaultStorageLocation, saveToLocation), fileName));
							}
														
						} break;
						
						case TCP_MKDIR: {
						 // char newDirectoryPath[BUFFER_SIZE]; (declaration on the beginning of internal while)
						 // char* defaultStorageLocation; (declaration is right before the switch statement)
							
							memset(buffer, 0, BUFFER_SIZE);
							
							defaultStorageLocation = SERVER_DIRECTORY;
							
							// create new directory
							if(mkdir(strcat(defaultStorageLocation, newDirectoryPath), 0777) < 0) {
								puts("(TCP server) mkdir error occured");
							}
							else {
								printf("(TCP server) created directory:\n%s\n", strcat(defaultStorageLocation, newDirectoryPath));
							}
														
						}	break;
						
						case TCP_SEND_FILE: {
						 // char fileName[BUFFER_SIZE]; (declaration on the beginning of internal while)
						 // char sendFrom[BUFFER_SIZE]; (declaration on the beginning of internal while)
							FILE* sentFile;
							
							memset(sendFrom, 0, BUFFER_SIZE);
							memset(buffer, 0, BUFFER_SIZE);
							
							if((sentFile = fopen(sendFrom, "rb")) == NULL) {
								puts("(TCP server) fopen failed");
								continue;
							}
							
							// send the files' size in bytes
							if(sendFileSizeTCP(connection, sentFile, MAX_FILE_SIZE) != 0) {
								puts("(TCP server) send file size failed.");
								continue;
							}
							
							// send the file
							if(sendFileTCP(connection, fileName, sentFile, BUFFER_SIZE) != 0) {
								puts("(TCP server) send file failed.");
								continue;
							}
							
							// get response from the server
							if(receiveRequestFormTCP(connection, buffer, BUFFER_SIZE) != 0) {
								puts("(TCP server) receiving response failed (2)");
								continue;
							}
														
						}	break;
						
						case TCP_CONTINUE: {
							continue;
						}
						
						case TCP_TERMINATE: {
							close(connection);
							return 0;
						}
					}
				}
			}
		}
	}

	return 0;
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

int sendFileTCP(int socketFd, char* fileName, FILE* aFile, int bufferSizeInBytes) {
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
	FILE* newFile;
	
	numberOfPackages = ceil((double) fileSize / (double) bufferSizeInBytes);
	if((newFile = fopen(strcat(saveToLocation, fileName), "wb")) == NULL)
		return ERROR_OPEN_FILE;
	
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