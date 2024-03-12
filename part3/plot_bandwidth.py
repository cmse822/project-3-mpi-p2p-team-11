import matplotlib.pyplot as plt
import pandas as pd

def plot_line(df):
    x = [] 
    y = []
    for i in range(len(df.iloc[:,0])):
        num_procs = df.iloc[i,0]
        message_size = 2 ** df.iloc[i,1]
        average_time = df.iloc[i,2] / 100
        x.append(message_size)
        y.append(message_size / average_time)
    plt.plot(x, y)

def main():
    df = pd.read_csv("Blocking_Ring_Shift_Wall_Times.txt", sep="\t", header=0)
    plt.xscale("log", base=2)
    plt.yscale("log")
    plt.title("Ring Shift Bandwidth v. Message Size")
    plt.xlabel("Message Size (B)")
    plt.ylabel("Bandwidth (B/s)")
    legend_labels = []
    for i in range(8):
        sub_df = df.iloc[20*i:20*(i+1),:]
        plot_line(sub_df)
        legend_labels.append(str(2**(i+1)) + " Processes")
    plt.legend(legend_labels, loc="lower right")
    plt.savefig("Blocking_Ring_Shift_Bandwidth_Plot")

if __name__ == "__main__":
    main()
