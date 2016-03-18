import matplotlib.pyplot as plt

"""
Window size: 10; 
X-axis: Loss probability;
Y-axis: Throughput (ABT, GBN and SR) in one graph/plot.
"""
def graph_one():
	# X-axis (ABT, GBN, SR)
	loss = [0.1, 0.2, 0.4, 0.6, 0.8]
	# Y-axis (ABT)
	abt_throughput = [0.0160615, 0.0150863, 0.0119061, 0.0073909, 0.0023928]
	plt.plot(loss, abt_throughput, marker='D', color='r', label='ABT')
	# Y-axis (GBN)
	#gbn_throughput = [0.0199211, 0.0184729, 0.0072286, 0.0027154, 0.0012851, 0.0012817]
	gbn_throughput = [0.0200937, 0.0200554, 0.0201345, 0.0159847, 0.0154164]
	plt.plot(loss, gbn_throughput, marker='D', color='g', label='GBN')
	# Y-axis (SR)
	#sr_throughput = [0.0001821, 0.0001964, 0.0001618, 0.0001414, 0.0001474]
	#plt.plot(loss, sr_throughput, marker='D', color='b', label='SR')
	# Axis labels
	plt.xlabel('Loss Probability (%)')
	plt.ylabel('Throughput (packets/time unit)')
	# Set Y-axis range
	plt.ylim([0,.05])
	# Legend
	plt.legend()
	# Title
	plt.title('Throughput vs. Loss Probability\n(with window size 10, corruption probability 0.2, 1000 messages sent)')
	# Show plot
	plt.show()

"""
Window size: 50;
X-axis: Loss probability;
Y-axis: Throughput (ABT, GBN and SR) in one graph/plot.
"""
def graph_two():
	# X-axis (ABT, GBN, SR)
	loss = [0.1, 0.2, 0.4, 0.6, 0.8]
	# Y-axis (ABT)
	abt_throughput = [0.0160615, 0.0150863, 0.0119061, 0.0073909, 0.0023928]
	plt.plot(loss, abt_throughput, marker='D', color='r', label='ABT')
	# Y-axis (GBN)
	#gbn_throughput = [0.0199211, 0.0187055, 0.0084213, 0.0043626, 0.0026713, 0.0001018]
	gbn_throughput = [0.0200937, 0.0200554, 0.0201345, 0.0156285, 0.0029316]
	plt.plot(loss, gbn_throughput, marker='D', color='g', label='GBN')
	# Y-axis (SR)
	#sr_throughput = [0.0006111, 0.0016197, 0.000195, 0.0001702, 0.0001454]
	#plt.plot(loss, sr_throughput, marker='D', color='b', label='SR')
	# Axis labels
	plt.xlabel('Loss Probability (%)')
	plt.ylabel('Throughput (packets/time unit)')
	# Set Y-axis range
	plt.ylim([0,.05])
	# Legend
	plt.legend()
	# Title
	plt.title('Throughput vs. Loss Probability\n(with window size 50, corruption probability 0.2, 1000 messages sent)')
	plt.show()

graph_one()
#graph_two()