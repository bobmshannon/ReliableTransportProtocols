import matplotlib.pyplot as plt

"""
Loss probability: 0.2
X-Axis: Window size(s)
Y-axis: Throughput (ABT, GBN and SR) in one graph/plot.
"""
def graph_one():
	# X-axis (ABT, GBN, SR)
	wsize = [10, 50, 100, 200, 500]
	# Y-axis (ABT)
	abt_throughput = [0.0150863, 0.0150863, 0.0150863, 0.0150863, 0.0150863]
	plt.plot(wsize, abt_throughput, marker='D', color='r', label='ABT')
	# Y-axis (GBN)
	gbn_throughput = [0.0200554, 0.0200554, 0.0200554, 0.0200554, 0.0200554]
	plt.plot(wsize, gbn_throughput, marker='D', color='g', label='GBN')
	# Y-axis (SR)
	#sr_throughput = [0.0198595, 0.0198595, 0.0198595, 0.0198595, 0.0198595]
	sr_throughput = [0.0197506, 0.0197506, 0.0197506, 0.0197506, 0.0197506]
	plt.plot(wsize, sr_throughput, marker='D', color='b', label='SR')
	# Axis labels
	plt.xlabel('Window Size (packets)')
	plt.ylabel('Throughput (packets/time unit)')
	# Legend
	plt.legend()
	# Set Y-axis range
	plt.ylim([.01,.03])
	# Title
	plt.title('Throughput vs. Window Size\n(with loss probability 0.2, corruption probability 0.2, 1000 messages sent)')
	# Show plot
	plt.show()

"""
Loss probability: 0.5
X-Axis: Window size(s)
Y-axis: Throughput (ABT, GBN and SR) in one graph/plot.
"""
def graph_two():
	# X-axis (ABT, GBN, SR)
	wsize = [10, 50, 100, 200, 500]
	# Y-axis (ABT)
	abt_throughput = [0.0097814, 0.0097814, 0.0097814, 0.0097814, 0.0097814]
	plt.plot(wsize, abt_throughput, marker='D', color='r', label='ABT')
	# Y-axis (GBN)
	gbn_throughput = [0.0185386, 0.0186728, 0.0187064, 0.0186839, 0.0186914]
	plt.plot(wsize, gbn_throughput, marker='D', color='g', label='GBN')
	# Y-axis (SR)
	#sr_throughput = [0.0198595, 0.020223, 0.020223, 0.020223, 0.020223]
	sr_throughput = [0.0142709, 0.0199106, 0.0199106, 0.0199106, 0.0199106]
	plt.plot(wsize, sr_throughput, marker='D', color='b', label='SR')
	# Axis labels
	plt.xlabel('Window Size (packets)')
	plt.ylabel('Throughput (packets/time unit)')
	# Legend
	plt.legend()
	# Set Y-axis range
	plt.ylim([0,.05])
	# Title
	plt.title('Throughput vs. Window Size\n(with loss probability 0.5, corruption probability 0.2, 1000 messages sent)')
	# Show plot
	plt.show()

"""
Loss probability: 0.8
X-Axis: Window size(s)
Y-axis: Throughput (ABT, GBN and SR) in one graph/plot.
"""
def graph_three():
	# X-axis (ABT, GBN, SR)
	wsize = [10, 50, 100, 200, 500]
	# Y-axis (ABT)
	abt_throughput = [0.0023928, 0.0023928, 0.0023928, 0.0023928, 0.0023928]
	plt.plot(wsize, abt_throughput, marker='D', color='r', label='ABT')
	# Y-axis (GBN)
	gbn_throughput = [0.0154164, 0.0029316, 0.0032048, 0.0030818, 0.0030682]
	plt.plot(wsize, gbn_throughput, marker='D', color='g', label='GBN')
	# Y-axis (SR)
	#sr_throughput = [0.0154164, 0.0087984, 0.008518, 0.0087438, 0.0087417]
	sr_throughput = [0.0166585, 0.0104501, 0.0090598, 0.0093466, 0.0060229]
	plt.plot(wsize, sr_throughput, marker='D', color='b', label='SR')
	# Axis labels
	plt.xlabel('Window Size (packets)')
	plt.ylabel('Throughput (packets/time unit)')
	# Legend
	plt.legend()
	# Set Y-axis range
	plt.ylim([0,.02])
	# Title
	plt.title('Throughput vs. Window Size\n(with loss probability 0.8, corruption probability 0.2, 1000 messages sent)')
	# Show plot
	plt.show()

#graph_one()
#graph_two()
graph_three()