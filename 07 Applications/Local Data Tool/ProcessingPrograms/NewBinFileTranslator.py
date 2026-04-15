import struct
import json
import argparse
import math
# This should correspond with arduino code
sensors = {
	0: "engineRPM",
	1: "frontLeftRPM",
	2: "frontRightRPM",
	3: "rearRPM",
	4: "rearBrakePressure",
	5: "frontBrakePressure",
	6: "LDSFrontRight",
	7: "LDSFrontLeft",
	8: "LDSRearRight",
	9: "LDSRearLeft",
	10: "CVTemp",
	11: "rearTCTemp",
	12: "teensyTemp",
}

# The < symbol is important to specify that the teensy uses little endian
PACKET_FORMAT = "<BIf"
PACKET_SIZE = struct.calcsize(PACKET_FORMAT)

def translate_and_write(in_path: str, out_path: str):
	"""
	Take in an input file and translate to JSON format
	Arguments:
		in_path: path to the input raw binary file
		out_path: path to the resulting translated file
	"""
	with open(in_path, 'rb') as in_file, open(out_path, 'w', encoding='utf-8') as out_file:
		first = True
		out_file.write("[")
		prev_raw_timestamp = 0
		base_offset = 0
		overflow_accumulator = 0
		while chunk := in_file.read(PACKET_SIZE):
			if len(chunk) < PACKET_SIZE:
				print(f"Warning incomplete chunk at end of file, skipping.")
				break
			# Unpack values from struct
			sensor_id, timestamp, value = struct.unpack(PACKET_FORMAT, chunk)
			raw_timestamp = int(timestamp)
			if first:
				base_offset = raw_timestamp
			# uint32 overflow: raw value wrapped around
			elif raw_timestamp < prev_raw_timestamp:
				overflow_accumulator += (2**32 // 100)
			adjusted = raw_timestamp - base_offset + overflow_accumulator
			record = {"microsec": adjusted * 100, sensors[sensor_id]: value}
			if not first:
				out_file.write(",\n")
			out_file.write(json.dumps(record))
			prev_raw_timestamp = raw_timestamp
			if first:
				first = False
		out_file.write("]")

if __name__ == "__main__":
	parser = argparse.ArgumentParser(
		prog="BinFileTranslator",
		description="Translates an input binary file to JSON"
	)
	parser.add_argument("in_path")
	parser.add_argument("out_path")

	args = parser.parse_args()
	in_path = args.in_path
	out_path = args.out_path

	print(f"Converting {in_path} to JSON, saving to {out_path}.")
	translate_and_write(in_path, out_path)
	print(f"Succesfully saved to {out_path}.")
