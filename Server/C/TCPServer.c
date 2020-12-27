/* 
// this file was created specifically for "SK_CloudProject" project.
// the "SK_CloudProject" is created as a collage project for a subject "Sieci Komputerowe".
// this library contains headers for functions used in a TCPClient program.
*/

#include "TCPServer.h"
#include "../../Errors.h"

int main(int argv, char** argc) {
	// variables

	puts("TCP client program started");

	if(argc < 2) {
		puts("too few arguments were entered");
		puts("please enter server's id and a port number as arguments");
		return ERROR_WRONG_INPUT;
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

int sendResponseFormTCP(int socketFD, char* aForm, int formSizeInBytes) {
	// definition
	return 0;
}

int sendFileSizeTCP(int socketFD, FILE* aFile, long maxSizeInBytes) {
	// definition
	return 0;
}

int sendFileTCP(int socketFD, char* fileName, FILE* aFile, int bufferSizeInBytes) {
	// definition
	return 0;
}

int receiveResponseFormTCP(int socketFD, char* buffer, int formSizeInBytes) {
	// definition
	return 0;
}

int downloadFileTCP(int socketFD, char* fileName, int bufferSizeInBytes) {
	// definition
	return 0;
}