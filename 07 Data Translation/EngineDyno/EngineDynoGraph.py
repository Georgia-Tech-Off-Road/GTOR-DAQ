import plotly.express as px
from tkinter import ttk
import matplotlib.pyplot as plt
import pandas as pd

def dynoGraph(filePath, dynoWindow, plotlyCheckVar1,colNum):
    # Read CSV file
    progressBar = ttk.Progressbar(dynoWindow, mode="indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    progressBar.start()

    data = pd.read_csv(filePath)
    # Extract time or RPM
    data.iloc[:, 0] = data.iloc[:, 0] / (10 ** 6)
    y = data.columns[colNum]
    y = data.columns[0]
    if plotlyCheckVar1 == 1:
        # Plotly graph
        fig = px.line(data, x=x, y=y, title="Engine Dyno Graph")
        fig.update_layout(
            yaxis_title="Column #"+str(colNum),
            xaxis_title="Microsec",
            legend_title="Engine Dyno",
            legend=dict(title="Legend", orientation="h", yanchor="bottom", y=1.02, xanchor="right", x=1)
        )
        fig.show()
    else:
        # Matplotlib graph
        plt.figure(figsize=(10, 6))
        plt.plot(data.iloc[:, 0], data.iloc[:, colNum], label="Engine Dyno")
        plt.ylabel("Data Column #"+str(colNum))
        plt.xlabel("Time (Sec)")
        plt.title("Engine Dyno Graph")
        plt.legend()
        plt.grid(True)
        plt.show()

    # Close the dyno window if it exists
    dynoWindow.destroy()
