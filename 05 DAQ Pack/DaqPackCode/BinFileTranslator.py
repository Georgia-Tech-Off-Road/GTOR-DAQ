import struct
import json
import argparse
# This should correspond with arduino code
sensors = {
	0: "engineRPM",
	1: "frontLeftRPM",
	2: "frontRightRPM",
	3: "auxRPM",
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

# The < symbol is important to specify that the teensy uses big endian
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
		while chunk := in_file.read(PACKET_SIZE):
			if len(chunk) < PACKET_SIZE:
				print(f"Warning incomplete chunk at end of file, skipping.")
				break
			sensor_id, timestamp, value = struct.unpack(PACKET_FORMAT, chunk)
			record = {"microsec": timestamp * 100, sensors[sensor_id]: value}
			if not first:
				out_file.write(",\n")
			out_file.write(json.dumps(record))
			first = False
		out_file.write("]")

if __name__ == "__main__":
	parser = argparse.ArgumentParser(
		prog="BinFileTranslator"
		description="Translates an input binary file to JSON"
	)
	parser.addArgument("in_path")
	parser.addArgument("out_path")

	args = parser.parse_args()
	in_path = args.in_path
	out_path = args.out_path

	print(f"Converting {in_path} to JSON, saving to {out_path}.")
