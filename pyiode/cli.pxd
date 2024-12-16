cdef extern from "cli.cpp":
    void initialize_cli() except +
    void finish_cli() except +
    void set_log_file_cli(char*) except + 
    void always_continue(bint) except +
    int get_total_allocated_memory()
    void alloc_doc_loop() except +
