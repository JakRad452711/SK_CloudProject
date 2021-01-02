/* 
// this file was created specifically for "SK_CloudProject" project.
// the "SK_CloudProject" is created as a collage project for a subject "Sieci Komputerowe".
// this file contains source code responsible for server's TCP communication and logic.
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "TCPServer.h"
#include "../../Actions.h"
#include "../../Errors.h"
#include "../../Paths.h"
#include "../../Values.h"

int main(int argc, char** argv) {
	int connection;
	int action;
	int portNumber;
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

	if(bind(socketFd, (struct sockaddr*) &thisMachine, sizeof(struct sockaddr_in)) < 0) {
		puts("(TCP server) bind error occured");
		return ERROR_BIND;
	} puts("(TCP server) after bind");

	if(listen(socketFd, 100) < 0) {
		puts("(TCP server) listen error occured");
		return ERROR_LISTEN;
	} puts("(TCP server) after listen\n");

	while(1) {
		if((connection = accept(socketFd, (struct sockaddr*) &endpoint, &size_of_sockaddr_in)) < 0) {
			puts("(TCP server) accept error occured");
			return ERROR_ACCEPT;
		}
		else {
			printf("(TCP server) connection from: %s:\n", inet_ntoa(adr.sin_addr));
			
			if(fork() == 0) {
				while(1) {
					if(receiveRequestFormTCP(connection, buffer, BUFFER_SIZE) != 0) {
						exit(ERROR_RECEIVE);
					}
					
					// handle request form
					// check if user can perform an action
					// create response
					
					if(sendResponseFormTCP(connection, buffer, BUFFER_SIZE) != 0) {
						exit(ERROR_SEND);
					}
					
					// variables shared between some switch cases
					char ifSucceeded[BUFFER_SIZE];
					char* defaultDownloadLocation;
		
					memset(ifSucceeded, 0, BUFFER_SIZE);

					
					switch(order) {
						case TCP_RECEIVE_FILE: {
							char fileName[BUFFER_SIZE];
							char saveToLocation[BUFFER_SIZE];
						 // char* defaultStorageLocation; (declaration is right before the switch statement)
						 // char ifSucceeded[BUFFER_SIZE] (declaration is right before the switch statement)
							long fileSize;
							
							defaultStorageLocation = SERVER_DIRECTORY;
							
							// get a files' name from the client
							if(recv(socketFd, buffer, BUFFER_SIZE, 0) < 0) {
								puts("(TCP client) recv error occured");
								a
								continue;
							}
							
							sprintf(fileName, "%s", buffer);
							
							// get the files' "save to" location from the client
							if(recv(socketFd, buffer, BUFFER_SIZE, 0) < 0) {
								puts("(TCP client) recv error occured (2)");
								sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
								continue;
							}
							
							// TODO: CHECK IF LOCATION IS VALID (IF NOT VALID THEN FILE SIZE = 0
							//																AND SEND EMPTY BUFFER)
							sprintf(saveToLocation, "%s", buffer);
							
							// get the files' size in bytes
							if(receiveFileSizeTCP(socketFd, &fileSize) != 0) {
								puts("(TCP client) receive file size error occured");
								sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
								continue;
							}
							
							// download the file
							if(receiveFileTCP(connection, fileName, strcat(defaultStorageLocation, saveToLocation), fileSize, BUFFER_SIZE) != 0) {
								puts("(TCP client) download file error occured");
							}
							else {
								memset(ifSucceeded, 0, BUFFER_SIZE);
								sprintf(ifSucceeded, "%s", "SUCCESS");
								printf("(TCP client) a file was downloaded:\n%s\n", strcat(strcat(defaultStorageLocation, saveToLocation), fileName));
							}
							
							sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
						} break;
						
						case TCP_SERVER_MKDIR: {
							char newDirectoryPath[BUFFER_SIZE];
						 // char* defaultDownloadLocation; (declaration is right before the switch statement)
							
							memset(buffer, 0, BUFFER_SIZE);
							
							defaultDownloadLocation = CLIENT_DOWNLOAD_DIR;
							
							// get directories relative path
							if(recv(socketFd, buffer, BUFFER_SIZE, 0) < 0) {
								puts("(TCP client) recv error occured");
								sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
								continue;
							}
							
							sprintf(newDirectoryPath, "%s", buffer);
							
							// TODO: CHECK IF THE PATH IS VALID
							
							// create new directory and tell the java module if it was successful
							if(mkdir(strcat(defaultDownloadLocation, newDirectoryPath), 0777) < 0) {
								puts("(TCP client) mkdir error occured");
							}
							else {
								memset(ifSucceeded, 0, BUFFER_SIZE);
								sprintf(ifSucceeded, "%s", "SUCCESS");
								printf("(TCP client) created directory:\n%s\n", strcat(defaultDownloadLocation, newDirectoryPath));
							}
							
							sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
							
						}	break;
						
						case TCP_SEND_FILE: {
							char uploadLocation[BUFFER_SIZE];
							char saveToFileNamed[BUFFER_SIZE];
						 // char ifSucceeded[BUFFER_SIZE] (declaration is right before the switch statement)
							FILE* sentFile;
							
							memset(saveToFileNamed, 0, BUFFER_SIZE);
							memset(uploadLocation, 0, BUFFER_SIZE);
							memset(buffer, 0, BUFFER_SIZE);
							
							// get uploaded files name from the java module
							if(receiveDataThroughThePipe(namedPipeReceive, buffer, BUFFER_SIZE) != 0) {
								puts("(TCP client) receiving data through a pipe failed (3)");
								sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
								continue;
							}
							
							sprintf(saveToFileNamed, "%s", buffer);
							memset(buffer, 0, BUFFER_SIZE);
							
							// get uploaded files saving location from the java module
							if(receiveDataThroughThePipe(namedPipeReceive, buffer, BUFFER_SIZE) != 0) {
								puts("(TCP client) receiving data through a pipe failed (4)");
								sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
								continue;
							}
							
							sprintf(uploadLocation, "%s", buffer);
							
							if((sentFile = fopen(uploadLocation, "rb")) == NULL) {
								puts("(TCP client) fopen failed");
								sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
								continue;
							}
							
							// send the files size in bytes
							if(sendFileSizeTCP(socketFd, sentFile, MAX_FILE_SIZE) != 0) {
								puts("(TCP client) send file size failed.");
								sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
								continue;
							}
							
							// send the file
							if(uploadFileTCP(socketFd, saveToFileNamed, sentFile, BUFFER_SIZE) != 0) {
								puts("(TCP client) send file failed.");
								sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
								continue;
							}
							
							// get response from the server
							if(receiveResponseFormTCP(socketFd, buffer, BUFFER_SIZE) != 0) {
								puts("(TCP client) receiving response failed (2)");
								sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
								continue;
							}
							
							// pass the response to the java module
							if(sendDataThroughThePipe(namedPipeSend, buffer, BUFFER_SIZE) != 0) {
								puts("(TCP client) sending data through a pipe failed (2)");
							}
							else {
								memset(ifSucceeded, 0, BUFFER_SIZE);
								sprintf(ifSucceeded, "%s", "SUCCESS");
							}
							
							sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
							
						}	break;
						
						case TCP_CONTINUE: {
							continue;
						}
						
						case TCP_TERMINATE: {
							close(socketFd);
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