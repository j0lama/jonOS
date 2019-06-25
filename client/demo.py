#!/usr/bin/python
import socket, sys, time

#Server
IPserver = '0.0.0.0'
PORTserver = 8888

#Client
IP = '123.123.123.123'
PORT = 1234

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((IPserver, PORTserver))

sock.sendto((' ' + sys.argv[1] + '\n\n\n').encode(), (IP, PORT))