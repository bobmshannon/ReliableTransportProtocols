import matplotlib.pyplot as plt

"""
Loss=0,Corruption=0,t=5
X-axis: Window size;
Y-axis: Throughput (ABT, GBN and SR) in one graph/plot.
"""
def graph_one():
	# X-axis (ABT, GBN, SR)
	wsize = [10, 50, 100, 200, 500]
	# Y-axis (ABT)
	abt_throughput = [0.063484, 0.063484, 0.063484, 0.063484, 0.063484]
	plt.plot(wsize, abt_throughput, marker='D', color='r', label='ABT')
	# Y-axis (GBN)
	gbn_throughput = [0.104128, 0.1134196, 0.128549, 0.1288381, 0.1294141]
	plt.plot(wsize, gbn_throughput, marker='D', color='g', label='GBN')
	# Y-axis (SR)
	#sr_throughput = [0.0001821, 0.0001964, 0.0001618, 0.0001414, 0.0001474]
	#plt.plot(loss, sr_throughput, marker='D', color='b', label='SR')
	# Axis labels
	plt.xlabel('Window Size (packets)')
	plt.ylabel('Throughput (packets/time unit)')
	# Set Y-axis range
	plt.ylim([0,.2])
	# Legend
	plt.legend()
	# Title
	plt.title('Throughput vs. Window Size\n(with 2 time units between each packet sent, \ncorruption probability 0.0, loss probability 0.0, and 1000 total messages sent)')
	# Show plot
	plt.show()

graph_one()