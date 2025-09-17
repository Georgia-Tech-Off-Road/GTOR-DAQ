import json
import statistics
import numpy

# find the polling rate receives a json list, a starting index, and an ending index

def PollingRateList(filePath,rpmPage):
    file = open(filePath)
    file_content = file.read()

    # entry list creation
    input_list = file_content.split("{")
    counter = 0
    for item in input_list:
        input_list[counter] = "{" + item
        counter +=1
    input_list.pop(0)
    output_list = []
    counter = 0
    for item in input_list:
        if counter + 1 == len(input_list) - 1:
            break 
        first_sec = json.loads(input_list[counter])["sec"] + json.loads(input_list[counter])["microsec"] / pow(10,6)
        last_sec = json.loads(input_list[counter+1])["sec"] + json.loads(input_list[counter+1])["microsec"] / pow(10,6)
        output_list.append(1/(last_sec-first_sec))
        counter+=1
    print(output_list)
    rpmPage.destroy()

def FindPollingRate(filePath,rpmPage):
    file = open(filePath)
    file_content = file.read()

    # entry list creation
    input_list = file_content.split("{")
    counter = 0
    for item in input_list:
        input_list[counter] = "{" + item
        counter +=1
    input_list.pop(0)
    first_entry = json.loads(input_list[0])
    last_entry = json.loads(input_list[len(input_list)-1])
    first_sec = first_entry["sec"] + first_entry["microsec"] / pow(10,6)
    last_sec = last_entry["sec"] + last_entry["microsec"] / pow(10,6)
    output = (len(input_list)-1) / (last_sec - first_sec)
    print(output)
    rpmPage.destroy()

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

