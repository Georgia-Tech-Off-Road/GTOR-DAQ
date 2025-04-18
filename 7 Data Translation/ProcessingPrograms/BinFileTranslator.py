def binConverter(input_file_name,chosePath,outputPath,settingsData):
    import struct
    import os

    # Define the format for the binary data (including padding to match Arduino's structure size)
    data_format = "Q L 3f 4i 4i 1f 1f"  # Add 4 bytes of padding (4x)
    data_size = struct.calcsize(data_format)
    output_file_name = os.path.splitext(os.path.basename(input_file_name))[0] + ".txt"
    # Check if the user has selected a directory
    print(settingsData)
    if chosePath:
        outfile = os.path.normpath(os.path.join(outputPath, output_file_name))
    else:
        outfile = output_file_name
    try:
        with open(input_file_name, "rb") as binary_file, open(outfile, "w") as text_file:
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

        print(f"Data successfully written to {outfile}")

    except FileNotFoundError:
        print("File not found")
    except Exception as e:
        print(f"An error occurred: {e}")
