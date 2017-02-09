#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

/*
* This function prints an error message when called.
* The error is to alert the user that their input
* for the user handle was invalid"
*/
void displayUserHandleInputError(){
	printf("Error: Your handle must be 10 characters or less\n");
}

/*
* This function prints an error message when called.
* The error is to alert the user that their input
* for the message was invalid"
*/
void displayUserMessageInputError(){
	printf("Error: Your message must be 500 characters or less\n");
}

/*
* This function checks the lenght of user input
* It returns 0 if the input is bad and 1 if it 
* is good.
*/
int checkInputLength(char *input, int maxLength){
	if (strlen(input) > maxLength){
		//printf("Error: Your handle must be 10 characters or less!\n");
		return 0;
	}
	else{
		return 1;
	}
}

/*
* This function gets input from the user that will
* server as their on screen handle.
* It calls the checkInputLength function to perform
* validation on the input from the user.
*/
void getUserHandle(char *input){
	int validInput = 0;
	int input_length_limit = 10;

	while (validInput == 0){

		printf("Enter your handle (up to 10 characters): ");
		fgets(input, 21, stdin);

		//check the length of the input
		validInput = checkInputLength(input, input_length_limit);

		//if input is invalid, print error
		if (validInput == 0){
			displayUserHandleInputError();
		}
	}
}

/*
* This function gets input from the user that will
* server as the message to be sent to the server.
* It calls the checkInputLength function to perform
* validation on the input from the user.
*/
void getUserMessage(char *prompt, char *message_with_handle){
	int validInput = 0;
	int input_length_limit = 501;
	char message[1024];

	while (validInput == 0){

		printf("%s ", prompt);										//get message from user
		fgets(message, 1024, stdin);

		//check the length of the input
		validInput = checkInputLength(message, input_length_limit);

		//if input is invalid, print error
		if (validInput == 0){
			displayUserMessageInputError();
		}
	}
		sprintf(message_with_handle, "%s %s", prompt, message);		//prepend handle to message
	
}

int main(int argc, char *argv[])
{

	int socketFD, portNumber, charsWritten, charsRead;
	int quit = 0;							//0 while the user wishes to continue, 1 if the user wishes to quit the program

	//socket structs
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

	char returnBuffer[514];					//array to hold string response received from server
	char authenticator[2] = {'#', '\0'};	//a string that should match the authentication message sent from the server
	char handle[20];						//a string to hold the user's handle
	//char message[501];						//a string containing the user's message
	char prompt[20];						//a string containing the prompt for user entry
	char message_with_handle[514];			//a string containing the message with the handle prepended

	
	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[2]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname(argv[1]);	//ip is localhost
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

	//greet user and get a handle
	printf("Welcome!\n");
	getUserHandle(handle);

	printf("Your hanlde will be %s \n", handle);
	handle[strcspn(handle, "\n")] = '\0'; //Remove the trailing \n 
	sprintf(prompt, "%s >", handle);	 //append ">" to handle

	//loop accepting messages until user wishes to quit
	while (quit == 0){
		//printf("%s ", prompt);										//get message from user
		//fgets(message, 501, stdin);
		//sprintf(message_with_handle, "%s %s", prompt, message);		//prepend handle to message

		getUserMessage(prompt, message_with_handle);

		// Send message to server
		charsWritten = send(socketFD, message_with_handle, strlen(message_with_handle), 0); // Write to the server
		if (charsWritten < 0) {
			fprintf(stderr, "CLIENT ERROR writing to socket\n");
		}

		//check for completion
		if (charsWritten < strlen(message_with_handle)) {
			fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
		}

		// Get return message from server
		memset(returnBuffer, '\0', sizeof(returnBuffer)); // Clear out the buffer again for reuse
		charsRead = recv(socketFD, returnBuffer, sizeof(returnBuffer) - 1, 0); // Read data from the socket, leaving \0 at end
		if (charsRead < 0) { fprintf(stderr, "CLIENT: ERROR reading from socket\n"); }

		printf("Server > %s\n", returnBuffer);			  //display returned message

		memset(returnBuffer, '\0', sizeof(returnBuffer)); // Clear out the buffer again for reuse

	}
	
	return 0;
}