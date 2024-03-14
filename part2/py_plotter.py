import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the data
data = pd.read_csv('nonblocking_p2p_timing_results.csv')

# Extract buffer sizes and times
buffer_sizes = data['BufferSize']
times = data['Time']

# Plotting
plt.figure(figsize=(8, 6))
plt.plot(buffer_sizes, times, marker='o', linestyle='-', color='b')

# Set the x-axis to logarithmic scale base 2
plt.xscale('log', base=2)

# Set the x-axis labels to 2^0, 2^1, ..., 2^n
# Since the x-axis is log scale, we place ticks at the powers of 2
xticks = [2**i for i in range(0, int(np.log2(max(buffer_sizes)))+1)]
xticklabels = [f'$2^{{{i}}}$' for i in range(0, int(np.log2(max(buffer_sizes)))+1)]
plt.xticks(ticks=xticks, labels=xticklabels)

# Set the y-axis label
plt.ylabel('Time (s)')

# Set the x-axis label
plt.xlabel('Buffer Size')

# Add a title
plt.title('Non-Blocking Ping-Pong Communication Time')

# Display the plot with a grid and apply tight layout
plt.grid(True, which="both", ls="--")

# Save the plot as a PDF file
plt.savefig('nonblocking_ping_pong_time.pdf')

# Optionally, display the plot
# plt.show()
