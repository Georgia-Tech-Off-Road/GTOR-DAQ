import warnings

warnings.filterwarnings("ignore", category=UserWarning)
def accel(fname):
    import pandas as pd
    import matplotlib.pyplot as plt
    print("Creating visuals...")

    # Create df from a text file
    df = pd.read_csv(fname, delimiter=',')
    time = df.iloc[:, 0]

    # Get ADC values from DF
    accelx = df.iloc[:, 11]
    accely = df.iloc[:, 12]
    accelz = df.iloc[:, 13]

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
    plt.savefig("Acceleration.png")
    print("Saved figure as Acceleration.png")
    plt.show()
    """

    # Plot Accel X
    plt.figure()
    plt.plot(time, accelx, label='Accel X')
    plt.title('Accel X')
    plt.xlabel('Time')
    plt.ylabel('Accel (X)')
    plt.grid(True)
    plt.savefig("AccelX.png")

    # Plot Accel Y
    plt.figure()
    plt.plot(time, accely, label='Accel Y', color='orange')
    plt.title('Accel Y')
    plt.xlabel('Time')
    plt.ylabel('Accel (Y)')
    plt.grid(True)
    plt.savefig("AccelY.png")

    # Plot Accel Z
    plt.figure()
    plt.plot(time, accelz, label='Accel Z', color='green')
    plt.title('Accel Z')
    plt.xlabel('Time')
    plt.ylabel('Accel (Z)')
    plt.grid(True)
    plt.savefig("AccelZ.png")

    print("Saved figures as AccelX,Y,Z.png")

    # Display all figures at once
    plt.show()
    print("Done")
        
