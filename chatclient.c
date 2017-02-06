#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

/*
* This function checks the lenght of user input
* It returns 0 if the input is bad and 1 if it 
* is good.
*/
int checkHandleLength(char *input){
	if (strlen(input) > 10){
		printf("Error: Your handle must be 10 characters or less!\n");

		return 0;
	}
	else{
		return 1;
	}
}

/*
* This function gets input from the user that will
* server as their on screen handle.
* It calls the checkHandleLength function to perform
* validation on the input from the user.
*/
void getUserInput(char *input){
	int validInput = 0;

	while (validInput == 0){

		printf("Enter your handle (up to 10 characters): ");
		fgets(input, 21, stdin);

		validInput = checkHandleLength(input);
	}
}

int main(int argc, char *argv[])
{

	int socketFD, portNumber, charsWritten, charsRead;

	//socket structs
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

	char returnBuffer[501];					//array to hold string response received from server
	char authenticator[2] = {'#', '\0'};	//a string that should match the authentication message sent from the server
	char handle[20];
	char message[501];
	char prompt[20];

	printf("Welcome!\n");
	getUserInput(handle);

	printf("Your hanlde will be %s \n", handle);
	handle[strcspn(handle, "\n")] = '\0'; //Remove the trailing \n 
	sprintf(prompt, "%s >", handle);	 //append ">" to handle
	printf("Enter a message: ");		//get message from user
	fgets(message, 500, stdin);
	printf("%s ", prompt);
	printf("%s\n", message);

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost");	//ip is localhost
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) {
		fprintf(stderr, "CLIENT: ERROR opening socket\n");
	}
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) { // Connect socket to address
		fprintf(stderr, "CLIENT: ERROR connectiong\n");
	}	

/*
	//send authentication message to server
	charsWritten = send(socketFD, authenticator, strlen(authenticator), 0); // Write to the server
	if (charsWritten < 0) {
		fprintf(stderr, "CLIENT ERROR writing to socket\n");
	}

	//check for completion
	if (charsWritten < strlen(authenticator)) {
		fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
	}

	// Get return message from server
	memset(returnBuffer, '\0', sizeof(returnBuffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, returnBuffer, sizeof(returnBuffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) { fprintf(stderr, "CLIENT: ERROR reading from socket\n"); }
	if (returnBuffer[0] != '#'){
		//print rejection message from server
		fprintf(stderr, "ERROR: Client is not authorized to connect.\n");
		exit(1);
	}

	buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing \n from buffer
*/

	// Send message to server
	charsWritten = send(socketFD, message, strlen(message), 0); // Write to the server
	if (charsWritten < 0) {
		fprintf(stderr, "CLIENT ERROR writing to socket\n");
	}

	//check for completion
	if (charsWritten < strlen(message)) {
		fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
	}

	// Get return message from server
	memset(returnBuffer, '\0', sizeof(returnBuffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, returnBuffer, sizeof(returnBuffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) { fprintf(stderr, "CLIENT: ERROR reading from socket\n"); }

	printf("Server responds: %s", message);
	
	return 0;
}