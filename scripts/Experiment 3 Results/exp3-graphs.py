import matplotlib.pyplot as plt

"""
Loss=0,Corruption=0,t=0.1,m=10000
X-axis: Window size;
Y-axis: Throughput (ABT, GBN and SR) in one graph/plot.
"""
def graph_one():
	# X-axis (ABT, GBN, SR)
	wsize = [10, 50, 100, 200, 500]
	# Y-axis (ABT)
	abt_throughput = [0.0696589, 0.0696589, 0.0696589, 0.0696589, 0.0696589]
	plt.plot(wsize, abt_throughput, marker='D', color='r', label='ABT')
	# Y-axis (GBN)
	gbn_throughput = [0.1412745, 0.0967342, 0.1264804, 0.1380641, 0.1381571]
	plt.plot(wsize, gbn_throughput, marker='D', color='g', label='GBN')
	# Y-axis (SR)
	sr_throughput = [0.0110078, 0.0510701, 0.1015294, 0.1580224, 0.1579091]
	plt.plot(wsize, sr_throughput, marker='D', color='b', label='SR')
	# Axis labels
	plt.xlabel('Window Size (packets)')
	plt.ylabel('Throughput (packets/time unit)')
	# Set Y-axis range
	plt.ylim([0,.2])
	# Legend
	plt.legend()
	# Title
	plt.title('Throughput vs. Window Size\n(with 0.1 time units between each packet sent, \ncorruption probability 0.0, loss probability 0.0, and 10,000 total messages sent)')
	# Show plot
	plt.show()

graph_one()