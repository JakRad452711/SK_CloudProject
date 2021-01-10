/* 
// this file was created specifically for "SK_CloudProject" project.
// the "SK_CloudProject" is created as a collage project for a subject "Sieci Komputerowe".
// this library contains headers for functions used in a TCPClient program.
*/

// returns string created from concatenating str1, str2 and str3
// str1, str2, str3 values are not changed in process
// after use allocated memory needs to be freed
// to free memory you need to do: "free(variableToWhichYouSavedTheString);"
char* connectStrings(char* str1, char* str2, char* str3);

// opens pipes created by c# module and saves their descriptors to variables
// returns 0 on success, or other values if an error occures
int handleNamedPipes(int* namedPipeCToJavaFd, int* namedPipeJavaToCFd);

// receives data from the pipe created by the c# client module
// saves the data in "buffer"
// returns 0 on success, or other values if an error occures
int receiveDataThroughThePipe(int aPipe, char* buffer, int bufferSizeInBytes);

// sends data from "buffer" through the named pipe created by the handleNamedPipes function
// sends "bufferSizeInBytes" bytes to the c# client module
// returns 0 on success, or other values if an error occures
int sendDataThroughThePipe(int aPipe, char* buffer, int bufferSizeInBytes);

// sends a request form through a "socketFd" using TCP protocol
// returns 0 on success, or other values if an error occures
int sendRequestFormTCP(int socketFd, char* aForm, int formSizeInBytes);

// sends "aFile"s size in byte's through a "socketFd" socket using TCP protocol
// if "aFile" size exceeds "maxSizeInBytes" size then an error is returned
// returns 0 on success, or other values if an error occures
int sendFileSizeTCP(int socketFd, FILE* aFile, long maxSizeInBytes);

// uploads "aFile" through a "socketFd" using TCP protocol
// the function sends multiple packets of "bufferSizeInBytes" size
// returns 0 on success, or other values if an error occurres
int uploadFileTCP(int socketFd, FILE* aFile, int bufferSizeInBytes);

// receives response sent after sending of a request form using TCP protocol
// the response is saved in the "buffer"
// returns 0 on success, or other values if an error occures
int receiveResponseFormTCP(int socketFd, char* buffer, int formSizeInBytes);

// receives files size and saves it to "fileSize"
// returns 0 on success, or other values if an error occures
int receiveFileSizeTCP(int socketFd, long* fileSize);

// downloads a file from a server through a "socketFd" using TCP protocol
// returns 0 on success, or other values if an error occures
int downloadFileTCP(int socketFd, char* fileName, char* saveToLocation, long fileSize, int bufferSizeInBytes);