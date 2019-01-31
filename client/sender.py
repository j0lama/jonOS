#!/usr/bin/python
import serial
import sys, socket

UDP_IP_ADDRESS = "192.168.1.222"
UDP_PORT = 1234

def main():
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.sendto(str(1+len(sys.argv[1])), (UDP_IP_ADDRESS, UDP_PORT))
	sock.sendto(sys.argv[1], (UDP_IP_ADDRESS, UDP_PORT))

if __name__ == '__main__':
	main()