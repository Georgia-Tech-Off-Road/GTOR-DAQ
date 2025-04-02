
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('doodoo.csv', delimiter=',')
#df = df[~df.iloc[:, 13].between(65534.99999999999999999999, 65535.1)]

# Convert timestamps to relative time
time = df.iloc[:, 1]/(10**6)

# Get sensor data
val = df.iloc[:, 2]
val2 = df.iloc[:, 3]

# Plot data
plt.figure()
plt.plot(time, val, label='Left')
plt.plot(time, val2, label='Right')
plt.title('RPM')
plt.ylabel('Rot/Min')
plt.xlabel('Time (Seconds)')
plt.grid(True)
plt.legend()
plt.show()
