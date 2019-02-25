#!/usr/bin/python
import socket, sys
import serial, time

IP = "192.168.1.123"
PORT = 1234

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
payload = open('payload', 'rb').read()
sock.sendto(str(len(payload)).encode(), (IP, PORT))
sock.sendto((sys.argv[1]).encode(), (IP, PORT))
sock.sendto(payload, (IP, PORT))

#Get execution times from the UART
ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=1)

# Get the start flag

while(ser.read().decode() != 'S'):
	pass
start = time.time()
# Get the end flag
while(ser.read().decode() != 'E'):
	pass
end = time.time()
print("Time: " + str(end - start) + "\n")