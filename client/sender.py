#!/usr/bin/python
import socket, sys

IP = "192.168.1.123"
PORT = 1234

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
payload = open('payload', 'rb').read()
sock.sendto(str(len(payload)).encode(), (IP, PORT))
sock.sendto(payload, (IP, PORT))

exit()