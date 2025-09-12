import json
import statistics

file = open("2025-9-11 20_29_59.txt")
file_content = file.read()


entry_list = file_content.split("{")

counter = 0
for item in entry_list:
    entry_list[counter] = "{" + item
    counter +=1

rpm1_max = json.loads(entry_list[1])["rpm1"]
rpm1_min = json.loads(entry_list[1])["rpm1"]
max_index = 0
min_index = 0

rpm1_list = []
rpm1_list_greater = []

r_counter = 1
for item in entry_list:
    if r_counter == len(entry_list):
        break
    rpm1_list.append(json.loads(entry_list[r_counter])["rpm1"])
    r_counter+=1

r_counter = 1
for item in entry_list:
    if r_counter == len(entry_list):
        break
    if json.loads(entry_list[r_counter])["rpm1"] > 1200:
        rpm1_list_greater.append(json.loads(entry_list[r_counter])["rpm1"])
    r_counter+=1

rpm1_counter = 2;
for item in entry_list:
    if rpm1_counter == len(entry_list):
        break
    if json.loads(entry_list[rpm1_counter])["rpm1"] > rpm1_max:
        rpm1_max = json.loads(entry_list[rpm1_counter])["rpm1"]
        max_index = rpm1_counter
    if json.loads(entry_list[rpm1_counter])["rpm1"] < rpm1_min:
        rpm1_min = json.loads(entry_list[rpm1_counter])["rpm1"]
        min_index = rpm1_counter
    rpm1_counter+=1
        



first_entry = json.loads(entry_list[1])
last_entry = json.loads(entry_list[len(entry_list) - 1])

first_sec = first_entry["sec"] + first_entry["microsec"]/ pow(10,len(str(first_entry["microsec"])))
last_sec = last_entry["sec"] + last_entry["microsec"]/ pow(10,len(str(last_entry["microsec"])))

polling_rate_json = (len(entry_list) - 1) / (last_sec - first_sec)


print(statistics.mean(rpm1_list_greater))
print(statistics.mode(rpm1_list_greater))
print(statistics.median(rpm1_list_greater))
#print(f"The polling rate is {polling_rate_json}")

#print(len(entry_list) - 1)
#print(first_sec)
#print(last_sec)
#print(rpm1_max)
#print(rpm1_min)
#print(f"rpm was greatest at entry {max_index}")
#print(entry_list[max_index])
#print(rpm1_list[3092])


