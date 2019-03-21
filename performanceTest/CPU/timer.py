import serial, time

#Get execution times from the UART
ser = serial.Serial("/dev/tty.SLAB_USBtoUART", 115200, timeout=1)

# Get the start flag
print(ser.read().decode())

while 1:
	while(ser.read().decode() != 'S'):
		pass
	start = time.time()
	# Get the end flag
	while(ser.read().decode() != 'E'):
		pass
	end = time.time()
	print("Time: " + str(end - start) + "\n")