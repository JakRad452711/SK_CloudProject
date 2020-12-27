/* 
// this file was created specifically for "SK_CloudProject" project.
// the "SK_CloudProject" is created as a collage project for a subject "Sieci Komputerowe".
// this library contains headers for functions used in a TCPClient program.
*/

// creates a named pipe that needs to be handeled
// connects to a named pipe created by the c# server module
// returns 0 on success, or other values if an error occures
int handleNamedPipes();

// receives data from the pipe created by the c# server module
// saves the data in "buffer"
// returns 0 on success, or other values if an error occures
int receiveDataThroughThePipe(char* buffer, int bufferSizeInBytes);

// sends data from "buffer" through the named pipe created by the handleNamedPipes function
// sends "bufferSizeInBytes" bytes to the c# server module
// returns 0 on success, or other values if an error occures
int sendDataThroughThePipe(char* buffer, int bufferSizeInBytes)

// sends a response form through a "socketFD" using TCP protocol
// returns 0 on success, or other values if an error occures
int sendResponseFormTCP(int socketFD, char* aForm, int formSizeInBytes);

// sends "aFile"s size in byte's through a "socketFD" socket using TCP protocol
// if "aFile" size exceeds "maxSizeInBytes" size then an error is returned
// returns 0 on success, or other values if an error occures
int sendFileSizeTCP(int socketFD, FILE* aFile, long maxSizeInBytes);

// sends "aFile" through a "socketFD" using TCP protocol
// the function sends multiple packets of "bufferSizeInBytes" size
// returns 0 on success, or other values if an error occurres
int sendFileTCP(int socketFD, char* fileName, FILE* aFile, int bufferSizeInBytes);

// receives response sent after sending of a request form using TCP protocol
// the response is saved in the "buffer"
// returns 0 on success, or other values if an error occures
int receiveResponseFormTCP(int socketFD, char* buffer, int formSizeInBytes);

// downloads a file from a server through a "socketFD" using TCP protocol
// returns 0 on success, or other values if an error occures
int downloadFileTCP(int socketFD, char* fileName, int bufferSizeInBytes);