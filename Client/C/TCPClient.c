/* 
// this file was created specifically for "SK_CloudProject" project.
// the "SK_CloudProject" is created as a collage project for a subject "Sieci Komputerowe".
// this library contains source code responsible for client's TCP communication.
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
#include "../../Buffer.h"
#include "../../Errors.h"
#include "Paths.h"

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
	portNumber = (int) strtol(argv[2], &remainingCharacters, 10);
	remainingCharacters = NULL;

	// checking if the argument is a valid port number
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

	if(handleNamedPipes(&namedPipeSend, &namedPipeReceive) != 0) {
		puts("(TCP client) named pipes error occured");
		return ERROR_PIPES;
	} puts("(TCP client) named pipes prepared");

	while(1) {
		if(receiveDataThroughThePipe(namedPipeReceive, buffer, BUFFER_SIZE) != 0) {
			puts("(TCP client) receiving data through a pipe failed");
			continue;
		}

		char* aForm = buffer;

		if(connect(socketFd, (struct sockaddr*) &endpoint, sizeof(struct sockaddr_in) < 0)) {
			puts("(TCP client) a connect error occured");
			return ERROR_CONNECT;
		}

		if(sendRequestFormTCP(socketFd, aForm, BUFFER_SIZE) != 0) {
			puts("(TCP client) sending a form failed");
			continue;
		}

		if(receiveResponseFormTCP(socketFd, buffer, BUFFER_SIZE) != 0) {
			puts("(TCP client) receiving response failed");
			continue;
		}

		char* response = buffer;

		if(sendDataThroughThePipe(namedPipeSend, response, BUFFER_SIZE) != 0) {
			puts("(TCP client) sending data through a pipe failed");
			continue;
		}

		memset(buffer, 0, BUFFER_SIZE);

		if(receiveDataThroughThePipe(namedPipeReceive, buffer, sizeof(int)) != 0) {
			puts("(TCP client) receiving data through a pipe failed (2)");
			continue;
		}

		order = (int) strtol(buffer, &remainingCharacters, 10);

		// checking if input is a valid number that is supported
		if(buffer == remainingCharacters || order > 1000000) {
			puts("(TCP client) wrong port number was entered");
			return ERROR_WRONG_INPUT;
		}
		
		switch(order) {
			case 0:
				return 0;
			case 1:
				// code
				break;
			// other cases
		}
		break;
	}

	return 0;
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

int sendFileTCP(int socketFd, char* fileName, FILE* aFile, int bufferSizeInBytes) {
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