import os
import sys
import subprocess
import importlib.util
import pycuda.driver as cuda
import platform
import shutil

cuda_paths = ["/usr/local/cuda/bin"]  # or "/usr/local/cuda-12.2/bin" if needed
os.environ["PATH"] = os.pathsep.join(cuda_paths + [os.environ.get("PATH", "")])

# Add the parent directory of ProcessingPrograms to sys.path
project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
if project_root not in sys.path:
    sys.path.insert(0, project_root)

# ---- CUDA check ----
def check_cuda_available():
    try:
        cuda.init()
        device_count = cuda.Device.count()
        if device_count == 0:
            print("No CUDA devices found.")
            return False
        print(f"CUDA is installed and found {device_count} device(s).")
        return True
    except Exception as e:
        print(f"CUDA not available: {e}")
        return False

def find_nvcc():
    """
    Detect the CUDA compiler (nvcc) automatically.
    Works on Linux, macOS, and Windows.
    Returns the full path to nvcc or raises RuntimeError if not found.
    """
    system = platform.system().lower()
    
    if system in ["linux", "darwin"]:
        # Linux / macOS
        nvcc_path = shutil.which("nvcc")
    elif system == "windows":
        # Windows
        try:
            output = subprocess.check_output(["where", "nvcc"], shell=True, text=True)
            nvcc_path = output.splitlines()[0].strip()
        except subprocess.CalledProcessError:
            nvcc_path = None
    else:
        nvcc_path = None

    if nvcc_path is None or not shutil.which(nvcc_path):
        raise RuntimeError("CUDA compiler (nvcc) not found. Make sure CUDA is installed and nvcc is on your PATH.")

    return nvcc_path

def check_compiled(module_name, source_file):
    cuda_setup_dir = os.path.dirname(os.path.abspath(__file__))
    # Path to the fancyCudaStuff folder
    fancy_cuda_dir = os.path.join(cuda_setup_dir, "fancyCudaStuff")

    # Path to the .cu source file
    source_file = os.path.join(fancy_cuda_dir, "cudaConvolver.cu")
    print(f"\n--- Checking for compiled CUDA module: {module_name} ---")

    # First, see if module import already works
    try:
        module_spec = importlib.util.find_spec(module_name)
        if module_spec is not None:
            print(f"{module_name} is already compiled and importable at: {module_spec.origin}")
            return True
    except ModuleNotFoundError:
        print(f"{module_name} not found — will attempt to compile.")

    import pybind11
    import sysconfig

    include_path = pybind11.get_include()
    python_inc = sysconfig.get_paths()["include"]
    ext_suffix = sysconfig.get_config_var("EXT_SUFFIX")

    nvcc_path = find_nvcc()

    if not os.path.exists(nvcc_path):
        raise RuntimeError(f"CUDA compiler not found at {nvcc_path}")

    output_dir = fancy_cuda_dir
    output_file = module_name.split(".")[-2] + ext_suffix  # cudaConvolver.cpython-310-x86_64-linux-gnu.so
    output_path = os.path.join(output_dir, output_file)

    # nvcc_path is something like /usr/local/cuda/bin/nvcc
    cuda_bin_dir = os.path.dirname(nvcc_path)
    cuda_home = os.path.dirname(cuda_bin_dir)  # /usr/local/cuda

    cuda_include = os.path.join(cuda_home, "include")
    cuda_lib64 = os.path.join(cuda_home, "lib64")  # Linux/macOS
    if sys.platform == "win32":
        cuda_lib64 = os.path.join(cuda_home, "lib", "x64")  # Windows convention

    # Compile command
    cmd = [
        nvcc_path,
        "-std=c++17",
        "-shared",
        "-Xcompiler", "-fPIC",
        f"-I{include_path}",      # pybind11
        f"-I{python_inc}",        # Python headers
        f"-I{cuda_include}",      # CUDA headers
        "-lcudart",
        f"-L{cuda_lib64}",        # CUDA lib
        source_file,
        "-o", output_path,
    ]


    print("\nRunning NVCC command:")
    print(" ".join(cmd))
    print()

    result = subprocess.run(cmd, capture_output=True, text=True)
    print("--- NVCC stdout ---")
    print(result.stdout)
    print("--- NVCC stderr ---")
    print(result.stderr)

    if result.returncode != 0:
        print(f"❌ Compilation failed with code {result.returncode}")
        return False

    if not os.path.exists(output_path):
        print(f"❌ Expected output file not found: {output_path}")
        return False

    print(f"✅ Build succeeded: {output_path}")
    return True

