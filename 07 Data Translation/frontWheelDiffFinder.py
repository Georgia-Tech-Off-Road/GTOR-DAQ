import json

# Load from file
with open("C:/Users/andre/Downloads/Lower Track S1 2025-11-2 14_41_24.txt", "r") as f:
    data = json.load(f)

# Compute the difference
for entry in data:
    try:
        entry["frontRpmDiff"] = entry["frontLeftRPM"] - entry["frontRightRPM"]
    except:
        entry["frontRPMDiff"] = None
    try:
        entry["frontRpmAvg"] = (entry["frontLeftRPM"] + entry["frontRightRPM"])/2
    except:
        entry["frontRpmAvg"] = None

# Save back
with open("data_with_diff.json", "w") as f:
    json.dump(data, f, indent=2)

print("Done. Added frontRpmDif")
