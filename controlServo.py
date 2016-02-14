#!/usr/bin/env python

import socket
import time


#TCP_IP = 'testpit.benclouser.com'
TCP_IP = '192.168.128.174'
TCP_PORT = 5011
BUFFER_SIZE = 1024

dutyPercent = 0
ascending = True
while True:
	MESSAGE = "{\"servo1\":"+str(dutyPercent)+"}:"
	print MESSAGE;
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((TCP_IP, TCP_PORT))
	s.send(MESSAGE)
	data = s.recv(BUFFER_SIZE)
	print str(data)
	s.close()
	time.sleep(0.1)

	if( (dutyPercent > 100) or (dutyPercent < 0) ):
		ascending = not ascending
	
	if(ascending):
		dutyPercent += 1
	else:
		dutyPercent -= 1

