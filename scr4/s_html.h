#ifndef _S_HTML_H_
#define _S_HTML_H_

#define JH_EOF  -1
#define JH_TEXT 0
#define JH_TAG  1
#define JH_B    2
#define JH_BODY 3
#define JH_FONT 4
#define JH_HEAD 6
#define JH_HREF 7
#define JH_HTML 8
#define JH_IMG  9
#define JH_META 10
#define JH_NAME 11
#define JH_P    12
#define JH_SRC  13
#define JH_U    14
#define JH_VAR  15
#define JH_END  16
#define JH_EQ   17
#define JH_A    18
#define JH_H1   20
#define JH_H2   21
#define JH_H3   22
#define JH_H4   23
#define JH_H5   24
#define JH_H6   25
#define JH_HR   26
#define JH_SCMT 27
#define JH_ECMT 28

typedef struct _hfile_ {
    U_ch    *h_filename;
    FILE    *h_fd;
    U_ch    *h_buf;
    U_ch    *h_val;
    int     h_bufsize;
    int     h_maxbufsize;
    int     h_type;
    int     h_reread;
} HFILE;

typedef struct _href_ {
    U_ch    *hr_filename;
    int     hr_protocol;  /* 0: relative, 1:http: ou ftp: */
    int     hr_type;      /* 0: browser, 1:graph */
    int     hr_exist;     /* 0: no : 1 : yes */
} HREF;

extern HREF    **JHRefs;
extern int     JHRefsNb;

/* FUNCTIONS */

#ifndef SCRPROTO

HFILE   *JHOpen();
U_ch    *JHRefFullPath();
U_ch    *SCR_std_filename();

#else
#include <s_prohtm.h>
#endif

#endif




