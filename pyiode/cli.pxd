cdef extern from "cli.cpp":
    void initialize_cli() 
    void finish_cli()
    void set_log_file_cli(char*) 
    void always_continue(bint)
    int get_total_allocated_memory() 
    void alloc_doc_loop()
