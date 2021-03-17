#include "scr.h"

typedef struct _txtk {
    int     key;
    char    *txt;
} TKEY;

TKEY _tkey[] = {
    SCR_F1,     "F",
    SCR_F20,    "\001",
    SCR_S_F1,   "s-F",
    SCR_S_F20,  "\001",
    SCR_C_F1,   "c-F",
    SCR_C_F20,  "\001",
    SCR_A_F1,   "a-F",
    SCR_A_F20,  "\001",
    SCR_A_A,    "a-",
    SCR_A_Z,    "A",
    SCR_C_A,    "c-",
    SCR_C_Z,    "A",
    SCR_ESCAPE, "Esc",
    SCR_ESCAPE, "",
    SCR_INSERT, "Ins",
    SCR_INSERT, "",
    SCR_DELETE, "Del",
    SCR_DELETE, "",
    SCR_ENTER,  "Enter",
    SCR_ENTER,  "",
    SCR_TAB,    "Tab",
    SCR_TAB,    "",
    SCR_S_TAB,      "s-Tab",
    SCR_S_TAB,      "",
    SCR_CSR_LEFT,   "<-",
    SCR_CSR_LEFT,   "",
    SCR_CSR_RIGHT,  "->",
    SCR_CSR_RIGHT,  "",
    SCR_CSR_UP,     "Up",
    SCR_CSR_UP,     "",
    SCR_CSR_DOWN,   "Down",
    SCR_CSR_DOWN,   "",
    SCR_CSR_PG_UP,  "PgUp",
    SCR_CSR_PG_UP,  "",
    SCR_CSR_PG_DN,  "PgDn",
    SCR_CSR_PG_DN,  "",
    SCR_CSR_HOME,   "Home",
    SCR_CSR_HOME,   "",
    SCR_CSR_END,    "End",
    SCR_CSR_END,    "",
    SCR_C_CSR_HOME, "c-Home",
    SCR_C_CSR_HOME, "",
    SCR_C_CSR_END,  "c-End",
    SCR_C_CSR_END,  "",
    SCR_C_CSR_LEFT  , "c-Lft",
    SCR_C_CSR_LEFT  , "",
    SCR_C_CSR_RIGHT , "c-Rgt",
    SCR_C_CSR_RIGHT , "",
    SCR_BACKSPACE   , "BkSp",
    SCR_BACKSPACE   , "",
    SCR_C_BACKSPACE , "c-BkSp",
    SCR_C_BACKSPACE , "",
    0,          0
};

/*
  Retourne dans un buffer statique le nom de la touche key.

&EX
    SCR_fnk_txt(SCR_ESCAPE);
    SCR_fnk_txt(SCR_F5);
&TX
*/
char *SCR_fnk_txt(key)
int     key;
{
    static char    buf[10];
    int     i, c;
    char    buf2[5];

    for(i = 0 ; _tkey[i].key != 0 ; i += 2) {
	if(key >= _tkey[i].key && key <= _tkey[i + 1].key)
	break;
    }
    if(_tkey[i].key == 0) return("?");
    strcpy(buf, _tkey[i].txt);
    key -= _tkey[i].key;
    c = _tkey[i + 1].txt[0];
    switch(c) {
	case 0:
	    return(buf);
	case 1:
	    sprintf(buf2, "%d", c + key);
	    break;
	default:
	    sprintf(buf2, "%c", c + key);
	    break;
    }

    strcat(buf, buf2);
    return(buf);
}
