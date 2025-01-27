# Creates a line graph of brake pressure vs. time

def rpm(fname):
    print("Creating visuals...")
    import pandas as pd
    import matplotlib.pyplot as plt

    # Create df from a text file
    #file = 'Test2.txt.csv'
    df = pd.read_csv(fname, delimiter=',')

    time = df.iloc[:, 0]
    val = df.iloc[:, 1]

    # Create line graph
    plt.plot(time, val)
    plt.title('RPM')
    plt.xlabel('Microseconds')
    plt.ylabel('RPM')
    plt.show() # Displays graph
    print("Done")
