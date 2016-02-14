#!/usr/bin/env python

import socket
import time


#TCP_IP = 'testpit.benclouser.com'
TCP_IP = '192.168.1.14'
TCP_PORT = 5011
BUFFER_SIZE = 1024



while True:
	MESSAGE = "Sending Time: " + time.strftime("%H:%M:%S")
	print MESSAGE;
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((TCP_IP, TCP_PORT))
	s.send(MESSAGE)
	data = s.recv(BUFFER_SIZE)
	print str(data)
	s.close()
	time.sleep(5)

