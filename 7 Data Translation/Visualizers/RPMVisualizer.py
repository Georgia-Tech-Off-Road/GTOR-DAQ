import warnings
import pandas as pd
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk

warnings.filterwarnings("ignore", category=UserWarning)
# Creates a line graph of brake pressure vs. time

def rpm(fname, rpmVisualizationPage):
    #create a label to say Analyzing File
    label1 = tk.Label(rpmVisualizationPage, text="Creating RPM Graph")
    label1.pack()
    #create a progress bar that just bounces back and forth
    progressBar = ttk.Progressbar(rpmVisualizationPage, mode = "indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    #start the bouncing
    progressBar.start()

    # Create df from a text file
    df = pd.read_csv(fname, delimiter=',')

    time = df.iloc[:, 1] * 10**(-6) - df.iloc[0, 1]* 10**(-6)

    val = df.iloc[:, 2]

    # Create line graph
    plt.plot(time, val)
    plt.title('RPM')
    plt.xlabel('Time (Seconds)')
    plt.ylabel('RPM')
    rpmVisualizationPage.destroy()
    plt.show() # Displays graph
