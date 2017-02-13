'''
Project 1 - chatserve.py
Adam Smith
CS372 - Winter 2017

Description: This is a chat server program. When executed, 
it will open a server socket, listening for a TCP connection request. 
This program will allow the exchange of messages between a client
application. It allows for messages of up to 500 characters in length.
If the user of the server application wants to terminate a client connection,
this can be done by sending "\quit" in a message to the client. 
To quit the server all together, enter ctrl c on the keyboard.
'''
import socket
import sys

#this function is used for binding the socket to the port number.
#It takes a socket object as an argument. 
def BindSocket(socket):
	try:
		serverSocket.bind(('', serverPort))
	except socket.error as msg:
		print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
		sys.exit()

#this function starts the server listening on the specified port
#It takes a socket object as an argument.
def ListenSocket(socket):
	serverSocket.listen(1)
	print 'Server is listening on port #: %d' %serverPort

#this function is used to establish a connection with a client
#It takes a socket object as an argument
def ConnectToClient(socket):
	connectionSocket, addr = serverSocket.accept()
	print 'Connected with ' + addr[0] + ':' + str(addr[1])
	print '\n'
	return connectionSocket

#this function is used to receive messages from the client
#It takes a connection object as an argument. 
def ReceiveMessage(connectionSocket):
	message = connectionSocket.recv(1024)
	return message

#this function is used to get input from the user. This will
#be the response message sent back to the client.
def GetResponseMessage():
	messageTooLong = True

	#loop until user enters a message that is less than 500 characters in length
	while messageTooLong == True:

		response = raw_input("Server > ")

		#check the length of the message
		messageTooLong = VerifyMessageLength(response)

	return response

'''
this function is used to check that the user 
entered a message of 500 characters or less.
If the message is too long it returns true, 
else it returns false. It takes a string as 
as argument.
'''
def VerifyMessageLength(message):
	messageLength = len(message)
	if messageLength > 500:
		print "Your message must be 500 character or less!"
		return True
	else:
		return False

#this function is used to send a message back to the client
#It takes a connection object and a string as arguments.
def SendMessage(connectionSocket, response):
	connectionSocket.send(response)

#this function compares the message entered to a string the user
#would enter to quit the program. It returns true if they are
#equal (the user wishes to quit) and false if not equal (the user
#does not wish to quit)
def CheckForQuit(message):
	quit = "\quit"
	if message == quit:
		return True
	else:
		return False

#parse command line argument to integer
serverPort = int(sys.argv[1])

#create a socket
serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#bind the socket
BindSocket(serverSocket)

#Start listening on socket	
ListenSocket(serverSocket)

try:
	while True:
		#connect to client 
		connection = ConnectToClient(serverSocket)
		
		try:
			while True:
				#receive message from client
				clientMessage = ReceiveMessage(connection)

				if clientMessage:
					#check if client's message is a quit message
					quit = CheckForQuit(clientMessage)

					#display received message
					print '%s' % clientMessage

					#if client sent "\quit" terminate connection
					if quit == True:
						print "closing connection..."
						connection.close()
						break

					#get a response message from the user
					serverMessage = GetResponseMessage()

					#check if user wishes to quit
					quit = CheckForQuit(serverMessage)

					#if the user wants to quit, close the connection
					if quit == True:
						SendMessage(connection, serverMessage)
						connection.close()
						print "closing connection..."
						break

					#sent response message back to client
					SendMessage(connection, serverMessage)
				else: 
					print >>sys.stderr, 'no more data from client'
					connection.close()
					break
		except:
			connection.close()
			serverSocket.close()
except:
	connection.close()
	serverSocket.close()
	print '\n'
	print 'Goodbye!\n'

	
