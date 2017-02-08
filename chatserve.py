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
	sys.exit()

#Start listening on socket	
serverSocket.listen(1)
print 'Server started on ' + addr[0] + ': %d' % serverPort

while True:
	connectionSocket, addr = serverSocket.accept()
	print 'Connected with ' + addr[0] + ':' + str(addr[1])
	
	try:
		while True:
			clientMessage = connectionSocket.recv(1024)
			if clientMessage:
				print '%s' % clientMessage
				serverMessage = raw_input("Enter message: ")
				print 'Server > %s' % serverMessage
				connectionSocket.send(serverMessage)
			else: 
				print >>sys.stderr, 'no more data from client'
				break
	finally:
		connectionSocket.close()
	
