/* 
// this file was created specifically for "SK_CloudProject" project.
// the "SK_CloudProject" is created as a collage project for a subject "Sieci Komputerowe".
// this file contains source code responsible for client's TCP communication and logic.
*/

#include "TCPServer.h"
#include "../../Buffer.h"
#include "../../Errors.h"
#include "../../Orders.h"

int main(int argc, char** argv) {
	// variables

	puts("TCP client program started");

	if(argc < 2) {
		puts("too few arguments were entered");
		puts("please enter server's id and a port number as arguments");
		return ERROR_BAD_ARGUMENTS;
	}

	// rest of the definition

	return 0;
}

int handleNamedPipes() {
	// definition
	return 0;
}

int receiveDataThroughThePipe(char* buffer, int bufferSizeInBytes) {
	// definition
	return 0;
}

int sendDataThroughThePipe(char* buffer, int bufferSizeInBytes) {
	// definition
	return 0;
}

int sendResponseFormTCP(int socketFd, char* aForm, int formSizeInBytes) {
	// definition
	return 0;
}

int sendFileSizeTCP(int socketFd, FILE* aFile, long maxSizeInBytes) {
	// definition
	return 0;
}

int sendFileTCP(int socketFd, char* fileName, FILE* aFile, int bufferSizeInBytes) {
	// definition
	return 0;
}

int receiveResponseFormTCP(int socketFd, char* buffer, int formSizeInBytes) {
	// definition
	return 0;
}

int downloadFileTCP(int socketFd, char* fileName, int bufferSizeInBytes) {
	// definition
	return 0;
}