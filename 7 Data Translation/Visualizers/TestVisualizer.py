
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('', delimiter=',')
df = df[~df.iloc[:, 13].between(65534.99999999999999999999, 65535.1)]

"""
#Uncomment to filter out the most frequent number (the error default) or second most frequent if most frequent is 0

mostFreq = df.iloc[:, 13].mode()[0]
nextFreq = df.iloc[:, 13].mode()[1]
if mostFreq != 0:
    df = df[~df.iloc[:, 13].between(mostFreq - 0.001, mostFreq + 0.001)]
else:
    df = df[~df.iloc[:, 13].between(nextFreq - 0.001, nextFreq + 0.001)]
"""

# Convert timestamps to relative time
time = df.iloc[:, 1]/(10**6)

# Get sensor data

#HALL EFFECT (RPM)
val = df.iloc[:, 2]
val2 = df.iloc[:, 3]

#STEERING ANGLE
#val=df.iloc[:, 13]

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
