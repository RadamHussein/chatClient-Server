import socket
import sys

#parse command line argument to integer
serverPort = int(sys.argv[1])

serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serverSocket.bind(('', serverPort))
serverSocket.listen(1)
print 'Server started on Port: %d' % serverPort

while 1:
	connectionSocket, addr = serverSocket.accept()
	
	echo = connectionSocket.recv(1024)
	connectionSocket.send(echo)
	connectionSocket.close()
	
