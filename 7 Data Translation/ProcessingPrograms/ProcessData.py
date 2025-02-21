import subprocess
import os

# Returns 0 if success, -1 otherwise
def processData(config_file, input_file, output_file, verbose=True):
    executable_location = os.path.join('Build', 'Windows', 'dataprocess.exe')
    cmd = [executable_location,"-c", config_file, "-i", input_file, "-o", output_file]
    if verbose:
        cmd.append("-v")
    print(cmd)
    completed_result = subprocess.run(cmd, shell=True)
    if completed_result.returncode != 0:
        return -1
    return 0
