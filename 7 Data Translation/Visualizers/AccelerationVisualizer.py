import warnings
import pandas as pd
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk

warnings.filterwarnings("ignore", category=UserWarning)
def accel(fname, accelVisualizationPage):
    #create a label to say Analyzing File
    label1 = tk.Label(accelVisualizationPage, text="Creating Acceleration Graphs")
    label1.pack()
    #create a progress bar that just bounces back and forth
    progressBar = ttk.Progressbar(accelVisualizationPage, mode = "indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    #start the bouncing
    progressBar.start()

    # Create df from a text file
    df = pd.read_csv(fname, delimiter=',')
    time = df.iloc[:, 1] * 10**(-6) - df.iloc[0, 1]* 10**(-6)

    # Get ADC values from DF
    accelx = df.iloc[:, 12]
    accely = df.iloc[:, 13]
    accelz = df.iloc[:, 14]

    # Plot all on one figure
    """
    plt.figure(figsize=(10, 6))
    plt.plot(time, accelx, label='Accel X', color='blue')
    plt.plot(time, accely, label='Accel Y', color='orange')
    plt.plot(time, accelz, label='Accel Z', color='green')

    plt.title('Acceleration Data')
    plt.xlabel('Time')
    plt.ylabel('Acceleration')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()
    """

    # Plot Accel X
    plt.figure()
    plt.plot(time, accelx, label='Accel X')
    plt.title('Accel X')
    plt.xlabel('Time (Seconds)')
    plt.ylabel('Accel (X)')
    plt.grid(True)

    # Plot Accel Y
    plt.figure()
    plt.plot(time, accely, label='Accel Y', color='orange')
    plt.title('Accel Y')
    plt.xlabel('Time (Seconds)')
    plt.ylabel('Accel (Y)')
    plt.grid(True)

    # Plot Accel Z
    plt.figure()
    plt.plot(time, accelz, label='Accel Z', color='green')
    plt.title('Accel Z')
    plt.xlabel('Time (Seconds)')
    plt.ylabel('Accel (Z)')
    plt.grid(True)

    # Display all figures at once
    accelVisualizationPage.destroy()
    plt.show()
        
