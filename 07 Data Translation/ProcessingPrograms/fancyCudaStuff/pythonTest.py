import subprocess
import os

def check_cuda():
    try:
        # Check if nvcc (CUDA compiler) exists
        subprocess.run(["nvcc", "--version"], check=True, stdout=subprocess.DEVNULL)
        print("CUDA is installed.")
        return True
    except (FileNotFoundError, subprocess.CalledProcessError):
        print("CUDA not found.")
        return False

check_cuda()

EXT_PATH = os.path.join(os.path.dirname(__file__), "fancyCudaStuff", "cudaConvolver.so")

def check_extension():
    if not os.path.exists(EXT_PATH):
        print("Compiled extension not found.")
        return False
    try:
        import fancyCudaStuff.cudaConvolver
        print("Extension loaded successfully.")
        return True
    except Exception as e:
        print(f"Extension failed to load: {e}")
        return False

check_extension()

import subprocess

def compile_extension():
    print("Compiling CUDA extension...")
    cwd = os.path.join(os.path.dirname(__file__), "fancyCudaStuff")
    try:
        subprocess.run([sys.executable, "setup.py", "build_ext", "--inplace"], cwd=cwd, check=True)
        print("Compilation successful.")
    except subprocess.CalledProcessError:
        print("Compilation failed. Check CUDA installation and Python version.")
        sys.exit(1)