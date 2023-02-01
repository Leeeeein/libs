import ctypes
import threading

# Load the shared library
lib = ctypes.CDLL('./libwork.so')

lib.work_init()
lib.log_init()
lib.connect_manager()

# Define the function prototype
lib.thread_ipc.argtypes = []
lib.thread_ipc.restype = ctypes.c_void_p

# Start the thread
thread = threading.Thread(target=lib.thread_ipc)
thread.start()

# Wait for the thread to finish
thread.join()
