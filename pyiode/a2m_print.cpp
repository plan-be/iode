#include "scr4/s_a2m.h"

void set_page_header(char* c_page_header, size_t length_page_header)
{
    if(A2M_PGHEAD != NULL)
        free(A2M_PGHEAD);
    A2M_PGHEAD = (unsigned char*) malloc(length_page_header);
    strcpy((char*) A2M_PGHEAD, c_page_header);
}

void set_page_footer(char* c_page_footer, size_t length_page_footer)
{
    if(A2M_PGFOOT != NULL)
        free(A2M_PGFOOT);
    A2M_PGFOOT = (unsigned char*) malloc(length_page_footer);
    strcpy((char*) A2M_PGFOOT, c_page_footer);
}
