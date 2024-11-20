from pyiode.cli cimport initialize_cli as cpp_initialize_cli
from pyiode.cli cimport finish_cli as cpp_finish_cli
from pyiode.cli cimport set_log_file_cli as cpp_set_log_file_cli
from pyiode.cli cimport always_continue as cpp_always_continue
from pyiode.cli cimport get_total_allocated_memory as cpp_get_total_allocated_memory
from pyiode.cli cimport alloc_doc_loop as cpp_alloc_doc_loop


def initialize_cli():
    # Initialize the command line interface
    cpp_initialize_cli()

def finish_cli():
    # Clean up and finish the command line interface
    cpp_finish_cli()

def set_log_file_cli(log_file: str):
    # Set the log file for the command line interface
    cpp_set_log_file_cli(log_file.encode('utf-8'))

def always_continue(value: bool):
    # Determine if the program should always continue
     cpp_always_continue(<bint>value)

def get_total_allocated_memory() -> int:
    # Get the total allocated memory for the program in bytes
    return cpp_get_total_allocated_memory()

def alloc_doc_loop():
    # ???
    cpp_alloc_doc_loop()
