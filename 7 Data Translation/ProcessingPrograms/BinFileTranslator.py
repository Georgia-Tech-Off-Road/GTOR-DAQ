def binConverter(input_file_name):
    import struct
    import os

    # Define the format for the binary data (including padding to match Arduino's structure size)
    data_format = "Q L 3i 4i 3f 3f 3f 4f 4i 3f 3f 3f 4f 4i"  # Add 4 bytes of padding (x4)
    data_size = struct.calcsize(data_format)

    # Generate output file name by appending ".txt" to the original file name
    output_file_name = os.path.splitext(input_file_name)[0] + ".txt"

    try:
        with open(input_file_name, "rb") as binary_file, open(output_file_name, "w") as text_file:
            print("Writing data...........")
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
        print("File not found")
    except Exception as e:
        print(f"An error occurred: {e}")
