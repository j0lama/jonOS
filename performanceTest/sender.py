#!/usr/bin/python
import socket

IP = "0.0.0.0"
PORT = 8080

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
payload = open('payload', 'rb').read()
sock.sendto(str(len(payload)).encode(), (IP, PORT))
sock.sendto(payload, (IP, PORT))