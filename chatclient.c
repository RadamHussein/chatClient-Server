/************************************************************
* Project 1 - chatclient.c
* Adam Smith
* CS372 - Winter 2017
*
* Description: This is a chat client program. It can be used
* to exchange messages with a server program running elsewhere.
* This program requires 2 command line arguments. First the
* <hostname> of the chat server, and second the <port number> of 
* the chat server. The program allows more messages with lengths 
* of up to 500 character. To exit the program and close the 
* connection between the client and server, type "\quit" and 
* send it as your last message. 
************************************************************/

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
* This function checks the lenght of user input.
* It returns 0 if the input is bad and 1 if it 
* is good.
*/
int checkInputLength(char *input, int maxLength){
	if (strlen(input) > maxLength){
		return 0;
	}
	else{
		return 1;
	}
}

/*
* This function gets input from the user that will
* serve as their on screen handle.
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
void getUserMessage(char *prompt, char *validMessage){
	int validInput = 0;
	int input_length_limit = 501;
	char message[1024];

	//loop until input is valid
	while (validInput == 0){

		memset(message, '\0', sizeof(message)); // Clear out the buffer again for reuse
		
		//get message from user
		printf("%s ", prompt);
		fgets(message, 1024, stdin);

		//check the length of the input
		validInput = checkInputLength(message, input_length_limit);

		//if input is invalid, print error
		if (validInput == 0){
			displayUserMessageInputError();
		}
	}

	//copy validated message into the validMessage string
	sprintf(validMessage, "%s", message);	
}

/*
* This function sets up and opens
* the client socket.
*/
void setUpSocket(int *sockFD){
	// Set up the socket
	*sockFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (sockFD < 0) {
		fprintf(stderr, "CLIENT: ERROR opening socket\n");
	}
}

/*
* This function connects the socket to the server
* address struct.
*/
void connectToServer(int *sockFD, struct sockaddr_in serverAddress){
	if (connect(*sockFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) { // Connect socket to address
		fprintf(stderr, "CLIENT: ERROR connectiong\n");
	}
}

/*
* This function handles sending messages to the server
* from the client. It returns the number of chars
* written into the connection to check for completion.
*/
int sendMessage(int *sockFD, int charsWritten, char *message){
	charsWritten = send(*sockFD, message, strlen(message), 0); // Write to the server
	if (charsWritten < 0) {
		fprintf(stderr, "CLIENT ERROR writing to socket\n");
	}

	return charsWritten;
}

/*
* This function checks that the entire
* message was sent by comparing the charsWritten
* variable to the message length.
*/
void checkForCompletion(int charsWritten, char *message){
	if (charsWritten < strlen(message)) {
			fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
		}
}

/*
* This function receives the message sent back from the 
* server. 
*/
void receiveMessage(int *sockFD, int charsRead, char *message){
	charsRead = recv(*sockFD, message, sizeof(message) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) { fprintf(stderr, "CLIENT: ERROR reading from socket\n"); }
}

/*
* This function takes the message the user entered,
* compares it to the hard coded "\quit" string,
* and returns a 1 if equal and a 0 if not equal.
*/
int checkForQuit(char *message){
	char quit[] = "\\quit\n";					//string to compare
	int equalsQuit = strcmp(quit, message);		//compare strings
	if (equalsQuit == 0){
		return 1;								
	}
	else{
		return 0;
	}
}

/*
* This function checks the message sent back from the server to
* see if it is a "\quit" message. If it is, this function
* will close the connection.
*/
int checkForServerQuitMessage(char *message){
	char quit[] = "\\quit";						//string to compare
	int equalsQuit = strcmp(quit, message);		//compare strings
	if (equalsQuit == 0){
		return 1;
	}
	else{
		return 0;
	}
}

//this function will close the socket connection 
void handleQuit(int *sockFD, char *message){
	int charsWritten;

	printf("Goodbye!\n");

	//remove the trailing \n before sending
	message[strcspn(message, "\n")] = '\0';

	//send a goodbye message to the server
	charsWritten = sendMessage(sockFD, charsWritten, message);
	checkForCompletion(charsWritten, message);

	//close the connection
	close(*sockFD);
}

int main(int argc, char *argv[])
{

	int socketFD, portNumber, charsWritten, charsRead;
	int quit = 0;							//0 while the user wishes to continue, 1 if the user wishes to quit the program

	//socket structs
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

	char returnBuffer[514];					//array to hold string response received from server
	char message[501];
	char authenticator[2] = {'#', '\0'};	//a string that should match the authentication message sent from the server
	char handle[20];						//a string to hold the user's handle
	char prompt[20];						//a string containing the prompt for user entry
	char message_with_handle[514];			//a string containing the message with the handle prepended

	//check for both required command line arguments
	if (argv[1] == NULL || argv[2] == NULL){
		perror("Invalid arguments!\n");
		exit(0);
	}

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); 	// Clear out the address struct
	portNumber = atoi(argv[2]); 									// Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; 							// Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); 					// Store the port number
	serverHostInfo = gethostbyname(argv[1]);						//ip is localhost
	if (serverHostInfo == NULL) { 
		fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); 
	}
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	//set up the socket
	setUpSocket(&socketFD);

	// Connect to server
	connectToServer(&socketFD, serverAddress);

	//greet user and get a handle
	printf("Welcome!\n");
	getUserHandle(handle);

	printf("Your hanlde will be %s \n", handle);
	handle[strcspn(handle, "\n")] = '\0'; //Remove the trailing \n 
	sprintf(prompt, "%s >", handle);	 //append ">" to handle

	//loop accepting messages until user wishes to quit
	while (quit == 0){
	
		//have user enter a message
		getUserMessage(prompt, message);

		//check if the user entered "\quit"
		quit = checkForQuit(message);

		//if the user wants to quit
		if (quit == 1){
			//close the connection and quit
			handleQuit(&socketFD, message);

			//get out of the loop
			break;
		}

		//prepend handle to message for sending
		sprintf(message_with_handle, "%s %s", prompt, message);

		//Remove the trailing \n before sending
		message_with_handle[strcspn(message_with_handle, "\n")] = '\0';  

		//clear out message string for reuse
		memset(message, '\0', sizeof(message));

		//send message to server
		charsWritten = sendMessage(&socketFD, charsWritten, message_with_handle);

		//check that charsWritten == message length
		checkForCompletion(charsWritten, message_with_handle);

		// Get return message from server
		memset(returnBuffer, '\0', sizeof(returnBuffer)); // Clear out the buffer again for reuse
		charsRead = recv(socketFD, returnBuffer, sizeof(returnBuffer) - 1, 0); // Read data from the socket, leaving \0 at end
		if (charsRead < 0) { fprintf(stderr, "CLIENT: ERROR reading from socket\n"); }

		//see if the server sent a quit message
		quit = checkForServerQuitMessage(returnBuffer);

		//if server quit, close connection
		if (quit == 1){
			printf("Server closed the connection...\n");
			close(socketFD);
			exit(0);
		}

		//display returned message
		printf("Server > %s\n", returnBuffer);

		// Clear out the buffer again for reuse
		memset(returnBuffer, '\0', sizeof(returnBuffer)); 

	}
	
	return 0;
}