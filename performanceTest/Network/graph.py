import matplotlib.pyplot as plt
import numpy as np

xAxis = [1, 10, 100, 1000, 10000, 20000, 40000, 60000, 80000, 100000]

raspbian = np.genfromtxt('raspbian.times', delimiter=',')
raspbianY = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
for line in raspbian:
	raspbianY[xAxis.index(int(line[0]))] += line[1]
raspbianY = [x / 10 for x in raspbianY]

jonOS = np.genfromtxt('jonOS.times', delimiter=',')
jonOSY = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
for line in jonOS:
	jonOSY[xAxis.index(int(line[0]))] += line[1]
jonOSY = [x / 10 for x in jonOSY]

plt.title('Network time comparation using Echo Server')
plt.plot(xAxis, raspbianY, '-bo', label='Raspbian (Linux)')
plt.plot(xAxis, jonOSY, '-r*', label='jonOS')
plt.xlabel('Number of Requests')
plt.ylabel('Time (seconds)')
plt.xscale('log')
plt.legend()
plt.show()
#plt.savefig('graph.png')