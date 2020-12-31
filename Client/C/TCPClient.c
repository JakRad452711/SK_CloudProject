/* 
// this file was created specifically for "SK_CloudProject" project.
// the "SK_CloudProject" is created as a collage project for a subject "Sieci Komputerowe".
// this file contains source code responsible for client's TCP communication.
*/

#include <arpa/inet.h>
#include <errno.h>
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
#include "../../Errors.h"
#include "../../Orders.h"
#include "../../Paths.h"
#include "../../Values.h"

int main(int argc, char** argv) {
	int namedPipeSend;
	int namedPipeReceive;	
	int order;
	int portNumber;
	int socketFd;
	char* ipAddress;
	char* remainingCharacters;
	struct sockaddr_in endpoint;
	
	char buffer[BUFFER_SIZE];

	puts("TCP client program started");

	if(argc < 3) {
		puts("(TCP client) too few arguments were entered");
		puts("(TCP client) please enter server's id and a port number as arguments");
		return ERROR_BAD_ARGUMENTS;
	}


	ipAddress = argv[1];
	remainingCharacters = NULL;
	portNumber = (int) strtol(argv[2], &remainingCharacters, 10);

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
	
	if(connect(socketFd, (struct sockaddr*) &endpoint, sizeof(struct sockaddr_in) < 0)) {
			puts("(TCP client) a connect error occured");
			return ERROR_CONNECT;
	}

	if(handleNamedPipes(&namedPipeSend, &namedPipeReceive) != 0) {
		puts("(TCP client) named pipes error occured");
		return ERROR_PIPES;
	} puts("(TCP client) named pipes prepared");

	while(1) {
		// get initial request form from java module
		if(receiveDataThroughThePipe(namedPipeReceive, buffer, BUFFER_SIZE) != 0) {
			puts("(TCP client) receiving data through a pipe failed");
			continue;
		}

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
		}

		char* response = buffer;

		// send response to the java module for processing
		if(sendDataThroughThePipe(namedPipeSend, response, BUFFER_SIZE) != 0) {
			puts("(TCP client) sending data through a pipe failed");
			continue;
		}

		memset(buffer, 0, BUFFER_SIZE);

		// get an order from the java module
		if(receiveDataThroughThePipe(namedPipeReceive, buffer, sizeof(int)) != 0) {
			puts("(TCP client) receiving data through a pipe failed (2)");
			continue;
		}

		order = (int) strtol(buffer, &remainingCharacters, 10);

		if(buffer == remainingCharacters || order < -6 || order > 0) {
			puts("(TCP client) wrong port number was entered");
			return ERROR_WRONG_INPUT;
		}
		
		// variables shared between some switch cases
		char ifSucceeded[BUFFER_SIZE];
		char* defaultDownloadLocation;
		
		memset(ifSucceeded, 0, BUFFER_SIZE);

		// execute the order
		switch(order) {
			case TCP_DOWNLOAD_FILE: {
				char fileName[BUFFER_SIZE];
				char fileLocation[BUFFER_SIZE];
			 // char* defaultDownloadLocation; (declaration is right before the switch statement)
			 // char ifSucceeded[BUFFER_SIZE] (declaration is right before the switch statement)
				long fileSize;
				
				defaultDownloadLocation = CLIENT_DOWNLOAD_DIR;
				
				// get a files name from the server
				if(recv(socketFd, buffer, BUFFER_SIZE, 0) < 0) {
					puts("(TCP client) recv error occured (2)");
					sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
					continue;
				}
				
				sprintf(fileName, "%s", buffer);
				
				// get the files location from the server
				if(recv(socketFd, buffer, BUFFER_SIZE, 0) < 0) {
					puts("(TCP client) recv error occured (2)");
					sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
					continue;
				}
				
				sprintf(fileLocation, "%s", buffer);
				
				// get the files size in bytes
				if(receiveFileSizeTCP(socketFd, &fileSize) != 0) {
					puts("(TCP client) receive file size error occured");
					sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
					continue;
				}
				
				// download the file
				if(downloadFileTCP(socketFd, fileName, strcat(defaultDownloadLocation, fileLocation), fileSize, BUFFER_SIZE) != 0) {
					puts("(TCP client) download file error occured");
				}
				else {
					memset(ifSucceeded, 0, BUFFER_SIZE);
					sprintf(ifSucceeded, "%s", "SUCCESS");
					printf("(TCP client) a file was downloaded:\n%s\n", strcat(strcat(defaultDownloadLocation, fileLocation), fileName));
				}
				
				sendDataThroughThePipe(namedPipeSend, ifSucceeded, sizeof(char));
			}	break;
				
			case TCP_CLIENT_MKDIR: {
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
		
		break; // <- - - this break is only for testing purposes. once everything works properly it will be removed
	}
}

int handleNamedPipes(int* namedPipeSend, int* namedPipeReceive) {
	if((*namedPipeSend = open(C_TO_JAVA_NAMED_PIPE, O_WRONLY)) < 0)
		return ERROR_OPEN_FIFO;

	if((*namedPipeReceive = open(JAVA_TO_C_NAMED_PIPE, O_RDONLY)) < 0)
		return ERROR_OPEN_FIFO;

	return 0;
}

int receiveDataThroughThePipe(int aPipe, char* buffer, int bufferSizeInBytes) {
	if(read(aPipe, buffer, bufferSizeInBytes) < 0)
		return ERROR_READ;

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

int uploadFileTCP(int socketFd, char* fileName, FILE* aFile, int bufferSizeInBytes) {
	fseek(aFile, 0, SEEK_END);
	long fileSize = ftell(aFile);
	fseek(aFile, 0, SEEK_SET);

	int numberOfPackages = ceil((double) fileSize / (double) BUFFER_SIZE);
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

int downloadFileTCP(int socketFd, char* fileName, char* filePath, long fileSize, int bufferSizeInBytes) {
	int numberOfPackages;
	int received;
	char buffer[bufferSizeInBytes];
	FILE* newFile;
	
	numberOfPackages = ceil((double) fileSize / (double) bufferSizeInBytes);
	if((newFile = fopen(strcat(filePath, fileName), "wb")) == NULL)
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