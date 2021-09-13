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

#ifndef SCRPROTO
INIFILE *IniOpen();
INIFILE *IniOpenSection();
U_ch    **IniReadCurSection();
U_ch    **IniReadSection();
U_ch    *IniError();
U_ch    **IniReadAllSections();

#else
#include <s_proini.h>
#endif

#endif /* _S_INI_H_ */












