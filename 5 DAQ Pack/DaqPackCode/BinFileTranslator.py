import struct

# Define the format for the binary data (including padding to match Arduino's structure size)
data_format = "Q L 4i 3i 3f 3f 3f 4f 4i 4x"  # Add 4 bytes of padding (x4)
data_size = struct.calcsize(data_format)

# Main function
def main():
    input_file_name = "2025-1-19 20_4_38"
    output_file_name = "output.txt"

    try:
        with open(input_file_name, "rb") as binary_file, open(output_file_name, "w") as text_file:

            # Read and process binary data
            while chunk := binary_file.read(data_size):
                if len(chunk) != data_size:
                    print("Incomplete data chunk encountered. Skipping.")
                    continue

                # Unpack the binary data
                data = struct.unpack(data_format, chunk)

                # Write the unpacked data as a comma-separated line
                text_file.write(",".join(map(str, data)) + "\n")

        print(f"Data successfully written to {output_file_name}")

    except FileNotFoundError:
        print(f"File not found: {input_file_name}")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    main()
