import matplotlib.pyplot as plt
import numpy as np

xAxis = [1, 10, 100, 1000, 10000, 100000, 200000, 400000, 600000, 800000, 1000000]

raspbian = np.genfromtxt('raspbianUART.times', delimiter=',')
raspbianY = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
for line in raspbian:
	raspbianY[xAxis.index(int(line[0]))] += line[1]
raspbianY = [x / 10 for x in raspbianY]

jonOS = np.genfromtxt('jonOS.times', delimiter=',')
jonOSY = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
for line in jonOS:
	jonOSY[xAxis.index(int(line[0]))] += line[1]
jonOSY = [x / 10 for x in jonOSY]

plt.title('Execution time comparation using MD5 Hash calculation')
plt.plot(xAxis, raspbianY, '-bo', label='Raspbian (Linux)')
plt.plot(xAxis, jonOSY, '-r*', label='jonOS')
plt.xlabel('Number of Executions')
plt.ylabel('Real time (seconds)')
#plt.xscale('log')
plt.legend()
#plt.show()
plt.savefig('graphUART.png')