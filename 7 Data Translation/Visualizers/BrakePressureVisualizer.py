import warnings

warnings.filterwarnings("ignore", category=UserWarning)
def brake(fname):
    print("Creating visuals...")
    import pandas as pd
    import matplotlib.pyplot as plt

    # Read the CSV file into a DataFrame
    df = pd.read_csv(fname, delimiter=',')

    # Extract time and ADC values
    time = df.iloc[:, 0]
    val = df.iloc[:, 4]
    # val = 50 + (((val / 32767) * 4.096 - 0.5) / 4) * 7950  # Uncomment to convert ADC to PSI

    # Create the line graph
    plt.plot(time, val, label="Brake Pressure",color="green")
    plt.title("Brake Pressure")
    plt.xlabel("Microseconds")
    plt.ylabel("Pressure")
    plt.grid(True)
    plt.tight_layout()

    # Show the graph
    plt.show()
    print("Done")
