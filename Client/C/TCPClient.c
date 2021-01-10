/* 
// this file was created specifically for "SK_CloudProject" project.
// the "SK_CloudProject" is created as a collage project for a subject "Sieci Komputerowe".
// this file contains source code responsible for client's TCP communication.
*/

#include <arpa/inet.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "TCPClient.h"
#include "../../Shared/Actions.h"
#include "../../Shared/Errors.h"
#include "../../Shared/Paths.h"
#include "../../Shared/Values.h"

int main(int argc, char** argv) {
	int action;
	int namedPipeSend;
	int namedPipeReceive;	
	int portNumber;
	int sizeOfSockaddr_in;
	int socketFd;
	char buffer[BUFFER_SIZE];
	char* ipAddress;
	char* remainingCharacters;
	struct sockaddr_in endpoint;
	
	puts("\n\n\nTCP client program started");

	if(argc < 3) {
		puts("(TCP client) too few arguments were entered");
		puts("(TCP client) please enter server's id and a port number as arguments");
		return ERROR_BAD_ARGUMENTS;
	}

	ipAddress = argv[1];
	remainingCharacters = NULL;
	portNumber = (int) strtol(argv[2], &remainingCharacters, 10);
	sizeOfSockaddr_in = sizeof(struct sockaddr_in);

	if(argv[2] == remainingCharacters || portNumber < 0 || portNumber > 65535) {
		puts("(TCP client) wrong port number was entered");
		return ERROR_WRONG_INPUT;
	}

	printf("(TCP client) ip address: %s\n", ipAddress);
	printf("(TCP client) port number: %d\n", portNumber);

	endpoint.sin_family = AF_INET;
	endpoint.sin_port = htons(portNumber);
	endpoint.sin_addr.s_addr = inet_addr(ipAddress);

	puts("(TCP client) variables prepared");

	if((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		puts("(TCP client) a socket error occured");
		return ERROR_SOCKET;
	} puts("(TCP client) socket prepared");
	
	if(connect(socketFd, (struct sockaddr*) &endpoint, sizeOfSockaddr_in) < 0) {
			puts("(TCP client) a connect error occured");
			return ERROR_CONNECT;
	} puts("(TCP client) after connect");
	
	if(handleNamedPipes(&namedPipeSend, &namedPipeReceive) != 0) {
			puts("(TCP client) named pipes error occured");
			return ERROR_PIPES;
	} puts("(TCP client) named pipes prepared");

	while(1) {
		puts("(TCP client) on while start");
				
		// get initial request form from java module
		if(receiveDataThroughThePipe(namedPipeReceive, buffer, BUFFER_SIZE) != 0) {
			puts("(TCP client) receiving data through a pipe failed");
			continue;
		} printf("(TCP client) received through a pipe:\n%s\n", buffer);

		char* aForm = buffer;

		// send the initial request form to a server
		if(sendRequestFormTCP(socketFd, aForm, BUFFER_SIZE) != 0) {
			puts("(TCP client) sending a form failed");
			continue;
		}

		// get response from the server
		if(receiveResponseFormTCP(socketFd, buffer, BUFFER_SIZE) != 0) {
			puts("(TCP client) receiving response failed");
			continue;
		} printf("(TCP client) received from the server:\n%s\n", buffer);

		char* response = buffer;

		// send response to the java module for processing
		if(sendDataThroughThePipe(namedPipeSend, response, BUFFER_SIZE) != 0) {
			puts("(TCP client) sending data through a pipe failed");
			continue;
		}

		memset(buffer, 0, BUFFER_SIZE);

		// get an action from the java module
		if(receiveDataThroughThePipe(namedPipeReceive, buffer, 4 * sizeof(char)) != 0) {
			puts("(TCP client) receiving data through a pipe failed (2)");
			continue;
		}

		action = (int) strtol(buffer, &remainingCharacters, 10);
		printf("(TCP client) received an action:%d\n", action);

		if(buffer == remainingCharacters || action < 100 || action > 300 || action % 100 != 0) {
			puts("(TCP client) wrong action was entered");
			return ERROR_WRONG_INPUT;
		}
		
		// variables shared between some switch cases
		char* defaultDownloadLocation;
		char fileName[BUFFER_SIZE];
		
		// execute the action
		switch(action) {
			case TCP_RECEIVE_FILE: {
				puts("(TCP client) TCP_RECEIVE_FILE");
				
				char saveToLocation[BUFFER_SIZE];
			 // char* defaultDownloadLocation; (declaration is right before the switch statement)
				long fileSize;
				
				defaultDownloadLocation = CLIENT_DOWNLOAD_DIR;
				
				// get a files' name from the java module
				if(receiveDataThroughThePipe(namedPipeReceive, buffer, BUFFER_SIZE) != 0) {
					puts("(TCP client) receiving data through a pipe failed (3)");
					continue;
				}
				
				sprintf(fileName, "%s", buffer);
				
				// get the files' "saveTo" location from the java module
				if(receiveDataThroughThePipe(namedPipeReceive, buffer, BUFFER_SIZE) != 0) {
					puts("(TCP client) receiving data through a pipe failed (4)");
					continue;
				} 
				
				sprintf(saveToLocation, "%s", buffer);
				memset(buffer, 0, BUFFER_SIZE);
				
				// get the files' size in bytes
				if(receiveFileSizeTCP(socketFd, &fileSize) != 0) {
					puts("(TCP client) receive file size error occured");
					continue;
				}
				
				char* filePath = connectStrings(defaultDownloadLocation, saveToLocation, "");
				
				// download the file
				if(downloadFileTCP(socketFd, fileName, filePath, fileSize, BUFFER_SIZE) != 0) {
					puts("(TCP client) download file error occured");
				}
				else {
					free(filePath);
					filePath = connectStrings(defaultDownloadLocation, saveToLocation, fileName);
					printf("(TCP client) a file was downloaded:\n%s\n", filePath);
				} free(filePath);
				
			}	break;
			
			case TCP_SEND_FILE: {
				puts("(TCP client) TCP_SEND_FILE");
				
				char uploadFrom[BUFFER_SIZE];
				FILE* sentFile;
				
				memset(uploadFrom, 0, BUFFER_SIZE);
				memset(buffer, 0, BUFFER_SIZE);
				
				// get uploaded files' "uploadFrom" location from the java module
				if(receiveDataThroughThePipe(namedPipeReceive, buffer, BUFFER_SIZE) != 0) {
					puts("(TCP client) receiving data through a pipe failed (6)");
					continue;
				}
				
				sprintf(uploadFrom, "%s", buffer);
				memset(buffer, 0, BUFFER_SIZE);
				
				if((sentFile = fopen(uploadFrom, "rb")) == NULL) {
					puts("(TCP client) fopen failed");
					continue;
				}
				
				// send the files' size in bytes
				if(sendFileSizeTCP(socketFd, sentFile, MAX_FILE_SIZE) != 0) {
					puts("(TCP client) send file size failed.");
					continue;
				}
				
				// upload the file
				if(uploadFileTCP(socketFd, sentFile, BUFFER_SIZE) != 0) {
					puts("(TCP client) send file failed.");
					continue;
				}
				
			}	break;

			case TCP_CONTINUE: {
				puts("(TCP client) TCP_CONTINUE");
				continue;
			}
		}
		memset(buffer, 0, BUFFER_SIZE); 
	}
}

char* connectStrings(char* str1, char* str2, char* str3) {
	int sizeOfAllStrings = sizeof(str1) + sizeof(str2) + sizeof(str3);
	char* returnString;
	
	returnString = malloc(sizeOfAllStrings + 1);
	sprintf(returnString, "%s%s%s", str1, str2, str3);
	
	return returnString;
}

int handleNamedPipes(int* namedPipeSend, int* namedPipeReceive) {
	if((*namedPipeSend = open(C_TO_JAVA_NAMED_PIPE, O_WRONLY)) < 0)
		return ERROR_OPEN_FIFO;

	if((*namedPipeReceive = open(JAVA_TO_C_NAMED_PIPE, O_RDONLY)) < 0)
		return ERROR_OPEN_FIFO;

	return 0;
}

int receiveDataThroughThePipe(int aPipe, char* buffer, int bufferSizeInBytes) {
	int numOfBytes = 0;
	while(numOfBytes != bufferSizeInBytes)
		numOfBytes += read(aPipe, buffer + numOfBytes, bufferSizeInBytes - numOfBytes);

	return 0;
}

int sendDataThroughThePipe(int aPipe, char* buffer, int bufferSizeInBytes) {
	if(write(aPipe, buffer, bufferSizeInBytes) < 0)
		return ERROR_WRITE;

	return 0;
}

int sendRequestFormTCP(int socketFd, char* aForm, int formSizeInBytes) {
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

int uploadFileTCP(int socketFd, FILE* aFile, int bufferSizeInBytes) {
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

int receiveFileSizeTCP(int socketFd, long* fileSize) {
	if(recv(socketFd, fileSize, sizeof(long), 0) < 0)
		return ERROR_RECEIVE;
		
	return 0;
}

int receiveResponseFormTCP(int socketFd, char* buffer, int formSizeInBytes) {
	if(recv(socketFd, buffer, formSizeInBytes, 0) < 0)
		return ERROR_RECEIVE;

	return 0;
}

int downloadFileTCP(int socketFd, char* fileName, char* saveToLocation, long fileSize, int bufferSizeInBytes) {
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