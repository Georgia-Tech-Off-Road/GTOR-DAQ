import json
import statistics
import numpy
import tkinter as tk
from tkinter import ttk
import pandas as pd
import matplotlib.pyplot as plt
# find the polling rate receives a json list, a starting index, and an ending index

def PollingRateList(filePath, prPage, sensorType):
    #sensorType = input("Type the string of the sensor you want to graph (use Show File Indeces button): ")
    with open(filePath,"r") as infile:
        text = infile.read()
        data = json.loads(text)
        df = pd.json_normalize(data)

    df["deltaSec"] = (df['sec'].shift(-1) + df['microsec'].shift(-1)/1e6) - (df['sec'] + df['microsec']/1e6) #deltaSec is the difference between
    df["pollingrate"] = 1 / df['deltaSec'] #converts seconds to Hertz (Polling Rates!)
    startTime = (df['sec'].iloc[0] + df['microsec'].iloc[0]/1e6) #converts absolute time to relative time
    df["time"] = (df['sec'] + df['microsec']/1e6) - startTime #so the time starts at 0
    x = df['time'] #assigns the x-axis to microseconds column
    ySensor = df.iloc[:, sensorType]  # gets the actual data from the column index
    yPolling = df["pollingrate"] #assigns right y-axis to polling rate
    fig, ax1 = plt.subplots(figsize=(8,5)) #creates plot based on figure and axes
    ax1.plot(x, ySensor, color="tab:blue", marker="o", label="Sensor Value") #plots sensor values with time
    ax1.set_xlabel("Seconds")
    ax1.set_ylabel("Sensor Value", color="tab:blue")
    ax1.tick_params(axis="y", labelcolor="tab:blue")    #formatting...
    ax2 = ax1.twinx()
    ax2.plot(x, yPolling, color="tab:red", marker="x", label="Polling Rate") #plots polling rate with microseconds
    ax2.set_ylabel("Polling Rate (Hz)", color="tab:red")
    ax2.tick_params(axis="y", labelcolor="tab:red")     #formatting...

    plt.title("Sensor Value, PR vs. Seconds") #titles graph
    plt.grid(True) #creates grid
    plt.show() #displays plot


    prPage.destroy() #kills window dead gone bye bye

def FindPollingRate(filePath, prPage):
    # Load entire file as JSON
    with open(filePath) as f:
        data = json.load(f)  # data is a list of dicts

    # Get first and last entries
    first_entry = data[0]
    last_entry = data[-1]

    first_sec = first_entry["sec"] + first_entry["microsec"] / 1e6
    last_sec = last_entry["sec"] + last_entry["microsec"] / 1e6
    output = (len(data) - 1) / (last_sec - first_sec)

    print("Average polling rate: "+ str(round(output,5))+"Hz")
    prPage.destroy()

"""
def ListByPercentile(rpm_list, input_list, start, end):
    counter = 0
    output_list = []
    for item in input_list:
        if json.loads(input_list[counter])["rpm1"] >= numpy.quantile(rpm_list, start) and json.loads(input_list[counter])["rpm1"] <= numpy.quantile(rpm_list, end):
          output_list.append(json.loads(input_list[counter])["rpm1"])
          #output_list.append(input_list[counter])
        counter+=1
    return output_list
"""
"""
# creates list of all rpms
rpm1_list = []
counter = 0
for item in entry_list:
    rpm1_list.append(json.loads(entry_list[counter])["rpm1"])
    counter+=1

Q1 = numpy.quantile(rpm1_list, 0.25)
Q3 = numpy.quantile(rpm1_list, 0.75)

# creates a list of all entries with outliers removed
entry_list_greater = []
counter = 0
for item in entry_list:
    if json.loads(entry_list[counter])["rpm1"] >= Q1 - (Q3 - Q1)*1.5 and json.loads(entry_list[counter])["rpm1"] <= Q3 + (Q3 - Q1)*1.5:
        entry_list_greater.append(entry_list[counter])
    counter+=1

# finds the index of the greatest contiguous string of entries
temp_index = 0
cont_index = 0
contiguous_counter = 1
temp_counter = 1
counter = 0
for item in entry_list_greater:
    if temp_counter == 1:
            temp_index = counter 
    if counter + 1 < len(entry_list_greater) and entry_list_greater[counter+1] == entry_list[entry_list.index(entry_list_greater[counter])+1]:
        temp_counter += 1
    else:
        if temp_counter > contiguous_counter:
            cont_index = temp_index
            contiguous_counter = temp_counter
        temp_counter = 1
    counter+=1

# finds the maximum and minimum rpm in the file
rpm1_max = json.loads(entry_list_greater[0])["rpm1"]
rpm1_min = json.loads(entry_list_greater[0])["rpm1"]
max_index = 0
min_index = 0

rpm1_counter = 0;
for item in entry_list:
    if rpm1_counter == len(entry_list_greater):
        break
    if json.loads(entry_list_greater[rpm1_counter])["rpm1"] > rpm1_max:
        rpm1_max = json.loads(entry_list_greater[rpm1_counter])["rpm1"]
        max_index = rpm1_counter
    if json.loads(entry_list_greater[rpm1_counter])["rpm1"] < rpm1_min:
        rpm1_min = json.loads(entry_list_greater[rpm1_counter])["rpm1"]
        min_index = rpm1_counter
    rpm1_counter+=1
"""
        
#print(PollingRateList(entry_list))
#print((ListByPercentile(rpm1_list, entry_list, .9, .99)))
#print(f"Max RPM is: {rpm1_max}")
#print(f"Polling rate is: {FindPollingRate(entry_list_greater,cont_index,cont_index+contiguous_counter-1)}")

