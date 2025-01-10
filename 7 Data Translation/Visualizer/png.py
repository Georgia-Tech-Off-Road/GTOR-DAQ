import matplotlib.pyplot as plt
import pandas as pd

# Ask for the file name at the beginning



# Create a function that generates Matplotlib line graphs
def generate_plot(fname):
    # Read the provided CSV file
    df = pd.read_csv(fname)
    df.columns = ['Seconds', 'Column2', 'Column3', 'Column4', 'RPM', 'Column6']
    df = df.sort_values(by="Seconds")
    df = df[:500000]
    df["Speed"] = df["RPM"] * 3.141592653589 / 48

    df['Seconds'] = df['Seconds'].astype(float)
    df['RPM'] = df['RPM'].astype(float)

    # Create the first plot (RPM/Time)
    plt.figure()
    plt.plot(df['Seconds'], df['RPM'], label='RPM')
    plt.title('RPM/Time')
    plt.xlabel('Seconds')
    plt.ylabel('RPM')
    plt.legend()
    plt.grid(True)
    plt.show()

    # Save the first plot as a PNG
    plt.savefig('RPMFig.png')

    # Create the second plot (Speed/Time)
    plt.figure()
    plt.plot(df['Seconds'], df['Speed'], label='Speed (mph)', color='red')
    plt.title('Speed (mph)/Time')
    plt.xlabel('Seconds')
    plt.ylabel('Speed (mph)')
    plt.legend()
    plt.grid(True)
    plt.show()

    # Save the second plot as a PNG
    plt.savefig('SpeedFig.png')

