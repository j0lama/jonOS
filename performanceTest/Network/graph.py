import matplotlib.pyplot as plt
import numpy as np

xAxis = [1, 10, 100, 1000, 10000, 20000, 40000, 60000, 80000, 100000]

raspbian = np.genfromtxt('raspbian.times', delimiter=',')
raspbianY = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
for line in raspbian:
	raspbianY[xAxis.index(int(line[0]))] += line[1]
raspbianY = [x / 10 for x in raspbianY]

raspbian_peer2peer = np.genfromtxt('raspbian_peer2peer.times', delimiter=',')
raspbian_peer2peerY = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
for line in raspbian_peer2peer:
	raspbian_peer2peerY[xAxis.index(int(line[0]))] += line[1]
raspbian_peer2peerY = [x / 10 for x in raspbian_peer2peerY]

jonOS = np.genfromtxt('jonOS.times', delimiter=',')
jonOSY = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
for line in jonOS:
	jonOSY[xAxis.index(int(line[0]))] += line[1]
jonOSY = [x / 10 for x in jonOSY]

jonOS_peer2peer = np.genfromtxt('jonOS_peer2peer.times', delimiter=',')
jonOS_peer2peerY = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
for line in jonOS_peer2peer:
	jonOS_peer2peerY[xAxis.index(int(line[0]))] += line[1]
jonOS_peer2peerY = [x / 10 for x in jonOS_peer2peerY]

plt.title('Network time comparation using Echo Server')
plt.plot(xAxis, raspbianY, '-bo', label='Raspbian (Linux) with Router')
plt.plot(xAxis, jonOSY, '-r*', label='jonOS with Router')
plt.plot(xAxis, raspbian_peer2peerY, '-g^', label='Raspbian (Linux) with P2P')
plt.plot(xAxis, jonOS_peer2peerY, '-ys', label='jonOS with P2P')
plt.xlabel('Number of Requests')
plt.ylabel('Time (seconds)')
plt.xscale('log')
plt.legend()
plt.show()
#plt.savefig('graph.png')