#!/usr/bin/python
import socket, sys

#Server
IPserver = '0.0.0.0'
PORTserver = 1234

#Client
IP = '0.0.0.0'
PORT = 8888

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((IPserver, PORTserver))

while(1):
	msg, _ = sock.recvfrom(32)
	sock.sendto(msg, (IP, PORT))