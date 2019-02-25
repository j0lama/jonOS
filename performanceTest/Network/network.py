#!/usr/bin/python
import socket, sys, time

#Server
IPserver = '0.0.0.0'
PORTserver = 8888

#Client
IP = '192.168.1.222'
PORT = 1234

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((IPserver, PORTserver))

ntimes = int(sys.argv[1])

start = time.time()
while(ntimes):
	sock.sendto('Echo1'.encode(), (IP, PORT))
	answer, _ = sock.recvfrom(32)
	ntimes -= 1
end = time.time()
print("Time: " + str(end - start) + "\n")