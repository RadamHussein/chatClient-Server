import socket
import sys

#parse command line argument to integer
serverPort = int(sys.argv[1])

serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#Bind socket
try:
	serverSocket.bind(('', serverPort))
except socket.error as msg:
	print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]

#Start listening on socket	
serverSocket.listen(1)
print 'Server started on Port: %d' % serverPort

while 1:
	connectionSocket, addr = serverSocket.accept()
	print 'Connected with ' + addr[0] + ':' + str(addr[1])
	
	clientMessage = connectionSocket.recv(1024)
	print 'Client > %s' % clientMessage
	serverMessage = raw_input("Enter message: ")
	print 'Server > %s' % serverMessage
	connectionSocket.send(serverMessage)
	#connectionSocket.close()
	
