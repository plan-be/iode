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

void set_curtag(char* c_curtag)
{
    memset(A2M_CURTAG, '\0', 41);
    strncpy((char*) A2M_CURTAG, c_curtag, 40);
}

void set_a2m_html_body(char* c_a2m_html_body, size_t length_a2m_html_body)
{
    if(A2M_HTML_BODY != NULL) 
        free(A2M_HTML_BODY);
    A2M_HTML_BODY = (char*) malloc(length_a2m_html_body);
    strcpy(A2M_HTML_BODY, c_a2m_html_body);
}

void set_a2m_html_title(char* c_a2m_html_title, size_t length_a2m_html_title)
{
    if(A2M_HTML_TITLE != NULL) 
       free(A2M_HTML_TITLE);
    A2M_HTML_TITLE = (unsigned char*) malloc(length_a2m_html_title);
    strcpy((char*) A2M_HTML_TITLE, c_a2m_html_title);
}
