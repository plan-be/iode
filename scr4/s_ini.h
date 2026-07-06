#ifndef _S_INI_H_
#define _S_INI_H_

#include <stdio.h>

typedef struct _inifile_ {
    char            *if_fname;      /* Filename          */
    FILE            *if_fd;         /* File descriptor   */
    unsigned char   *if_section,    /* Last Read section */
		    *if_param;      /* Last Read Param   */
    short   if_type;                /* Last Read type    */
    short   if_unread;              /* Unread active     */
    short   if_usection;            /* Section Uppercase */
    short   if_uparam;              /* Param Uppercase   */
    short   if_cline;               /* Current line      */
} INIFILE;

#define INI_SECTION 1
#define INI_PARAM   2

/* FUNCTIONS */
#include <s_proini.h>

#endif /* _S_INI_H_ */












