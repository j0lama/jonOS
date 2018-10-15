#!/usr/bin/python
import serial
import sys
import re
import time

def main():
	ser = serial.Serial("/dev/tty.SLAB_USBtoUART", 115200, timeout=1)

	with open('function.o', 'rb') as f:
		text = f.read()
		input()
		ser.write(text) #Send the binary function

	while True:
		print(ser.read().decode())
		input()
		ser.write("b\r\n".encode())

if __name__ == '__main__':
	main()