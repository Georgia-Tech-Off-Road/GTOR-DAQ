import json
import statistics
import numpy

# find the polling rate receives a json list, a starting index, and an ending index
def FindPollingRate(input_list, start, end):
    first_entry = json.loads(entry_list[start])
    last_entry = json.loads(entry_list[end])
    first_sec = first_entry["sec"] + first_entry["microsec"] / pow(10,len(str(first_entry["microsec"])))
    last_sec = last_entry["sec"] + last_entry["microsec"] / pow(10,len(str(last_entry["microsec"])))
    polling_rate_json = (end-start) / (last_sec - first_sec)
    return polling_rate_json


file = open("2025-9-11 20_29_59.txt")
file_content = file.read()

# entry list creation
entry_list = file_content.split("{")
counter = 0
for item in entry_list:
    entry_list[counter] = "{" + item
    counter +=1
entry_list.pop(0)

# creates list of all rpms
rpm1_list = []
counter = 0
for item in entry_list:
    rpm1_list.append(json.loads(entry_list[counter])["rpm1"])
    counter+=1

Q1 = numpy.quantile(rpm1_list, 0.25)
Q3 = numpy.quantile(rpm1_list, 0.75)
# creates list of all rpms greater than or equal to the median of the total list
rpm1_list_greater = []
counter = 0
for item in entry_list:
    if json.loads(entry_list[counter])["rpm1"] >= Q1 - (Q3 - Q1)*1.5 and json.loads(entry_list[counter])["rpm1"] <= Q3 + (Q3 - Q1)*1.5:
        rpm1_list_greater.append(json.loads(entry_list[counter])["rpm1"])
    counter+=1

# creates a list of all entries with an rpm greater than the median
entry_list_greater = []
counter = 0
for item in entry_list:
    if json.loads(entry_list[counter])["rpm1"] >= Q1 - (Q3 - Q1)*1.5 and json.loads(entry_list[counter])["rpm1"] <= Q3 + (Q3 - Q1)*1.5:
        entry_list_greater.append(entry_list[counter])
    counter+=1

# finds the index of the greatest contiguous string of entries
temp_index = 0
max_index = 0
contiguous_counter = 1
temp_counter = 1
counter = 0
for item in entry_list_greater:
    if temp_counter == 1:
            temp_index = counter 
    if counter + 1 < len(entry_list_greater) and entry_list_greater[counter] in entry_list and entry_list_greater[counter+1] == entry_list[entry_list.index(entry_list_greater[counter])+1]:
        temp_counter += 1
    else:
        if temp_counter > contiguous_counter:
            max_index = temp_index
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
        

print(rpm1_max)
print(Q1)
print(Q3 + (Q3-Q1)*1.5)