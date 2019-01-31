#!/usr/bin/python
import sys, socket

UDP_IP_ADDRESS = "192.168.1.123"
UDP_PORT = 1234

def main():
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	payload = open('payload', 'rb').read()
	sock.sendto(str(len(payload)), (UDP_IP_ADDRESS, UDP_PORT))
	sock.sendto(payload, (UDP_IP_ADDRESS, UDP_PORT))

if __name__ == '__main__':
	main()