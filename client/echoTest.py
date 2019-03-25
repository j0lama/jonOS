#!/usr/bin/python
import socket, sys, time

#Server
IPserver = '0.0.0.0'
PORTserver = 8888

#Client
IP = '192.168.1.123'
PORT = 1234

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((IPserver, PORTserver))

sock.sendto('\n' + sys.argv[1] + '\n\n\n'.encode(), (IP, PORT))