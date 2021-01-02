/* 
// this file was created specifically for "SK_CloudProject" project.
// the "SK_CloudProject" is created as a collage project for a subject "Sieci Komputerowe".
// this library contains headers for functions used in a TCPServer program.
*/

// sends a response form through a "socketFd" using TCP protocol
// returns 0 on success, or other values if an error occures
int sendResponseFormTCP(int socketFd, char* aForm, int formSizeInBytes);

// sends "aFile"s size in byte's through a "socketFd" socket using TCP protocol
// if "aFile" size exceeds "maxSizeInBytes" size then an error is returned
// returns 0 on success, or other values if an error occures
int sendFileSizeTCP(int socketFd, FILE* aFile, long maxSizeInBytes);

// sends "aFile" through a "socketFd" using TCP protocol
// the function sends multiple packets of "bufferSizeInBytes" size
// returns 0 on success, or other values if an error occurres
int sendFileTCP(int socketFd, char* fileName, FILE* aFile, int bufferSizeInBytes);

// receives response sent after sending of a request form using TCP protocol
// the response is saved in the "buffer"
// returns 0 on success, or other values if an error occures
int receiveResquestFormTCP(int socketFd, char* buffer, int formSizeInBytes);

// downloads a file from a server through a "socketFd" using TCP protocol
// returns 0 on success, or other values if an error occures
int receiveFileTCP(int socketFd, char* fileName, char* saveToLocation, long fileSize, int bufferSizeInBytes);