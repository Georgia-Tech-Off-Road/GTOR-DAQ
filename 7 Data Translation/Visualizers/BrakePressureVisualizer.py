import warnings
import pandas as pd
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk

warnings.filterwarnings("ignore", category=UserWarning)
def brake(fname, brakeVisualizationPage):
    print(fname)
    #create a label to say Analyzing File
    label1 = tk.Label(brakeVisualizationPage, text="Creating Brake Pressure Graph")
    label1.pack()
    #create a progress bar that just bounces back and forth
    progressBar = ttk.Progressbar(brakeVisualizationPage, mode = "indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    #start the bouncing
    progressBar.start()

    # Read the CSV file into a DataFrame
    df = pd.read_csv(fname, delimiter=',')

    # Extract time and ADC values
    # df = df[~((df.iloc[:, 4] > -20.0312) & (df.iloc[:, 4] < -20.0310))] #removes values that seem to be "zeroed".....
    time = df.iloc[:, 1]
    val = df.iloc[:, 5]

    # val = 50 + (((val / 32767) * 4.096 - 0.5) / 4) * 7950  # Uncomment to convert ADC to PSI (already done in data conversion)

    # Create the line graph
    plt.plot(time, val, label="Brake Pressure",color="green")
    plt.title("Brake Pressure")
    plt.xlabel("Time")
    plt.ylabel("Pressure")
    plt.grid(True)
    plt.tight_layout()

    # Show the graph
    brakeVisualizationPage.destroy()
    plt.show()
