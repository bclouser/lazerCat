#!/usr/bin/env python

import socket
import time


#TCP_IP = 'testpit.benclouser.com'
TCP_IP = '192.168.1.14'
TCP_PORT = 5011
BUFFER_SIZE = 1024
MESSAGE = "{\"coords\":{\"lat\":34.546978, \"lng\":-77.945678}}"

lat = 38.954577;
lng = -77.346357;

MESSAGE = "{\"coords\":{\"lat\":"+str(lat)+", \"lng\":"+str(lng)+"}}"
print MESSAGE;

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
s.send(MESSAGE)
data = s.recv(BUFFER_SIZE)
print str(data)
s.close()
