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
    time = df.iloc[:, 1] * 10**(-6)
    val = df.iloc[:, 5]

    # Create the line graph
    plt.plot(time, val, label="Brake Pressure",color="green")
    plt.title("Brake Pressure")
    plt.xlabel("Time (Seconds)")
    plt.ylabel("Pressure (PSI)")
    plt.grid(True)
    plt.tight_layout()

    # Show the graph
    brakeVisualizationPage.destroy()
    plt.show()
