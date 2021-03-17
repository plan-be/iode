#include "iode.h"

/*
    Reads an ascii file with the following format

	sample 1980Y1 1990Y1
	A 1 2 3 na na 4 5
	B 0.1 0.2 -3.14
	.....
*/

#define ASC_SMPL 1

YYKEYS IMP_ASC_KEYS[] = {
    "sample",   ASC_SMPL
};


int IMP_hd_asc(yy, smpl)
YYFILE  *yy;
SAMPLE  *smpl;
{
    SAMPLE  *rsmpl;

    if (YY_lex(yy) != ASC_SMPL)  return(-1);

    rsmpl = K_read_smpl(yy);
    if(rsmpl == NULL) return(-1);

    memcpy(smpl, rsmpl, sizeof(SAMPLE));
    SW_nfree(rsmpl);
    return(0);
}


int IMP_vec_asc(yy, name, dim, vector)
YYFILE  *yy;
char    *name;
int     dim; IODE_REAL    *vector;
{
    int i, rc;

    while(1) {
	switch(YY_lex(yy))  {
	    case YY_EOF : return(-1);
	    case YY_WORD :
		if(strcmp(yy->yy_text, "na") == 0) continue;

		SCR_strlcpy(name, yy->yy_text, 79); /* JMP 13-02-2013 */
		name[80] = '\0';
		for(i = 0; i < dim; i++) vector[i] = K_read_real(yy);
		return(0);
	}
    }
}

IMPDEF IMP_ASC = {
    IMP_ASC_KEYS,
    1,
    IMP_hd_asc,
    IMP_vec_asc,
    NULL,
    NULL
};

/*CMT */
YYFILE  *AYY;

IMP_hd_casc(impdef, file, lang)
IMPDEF  *impdef;
char    *file;
int     lang;
{
    SCR_strip(file);
    AYY = YY_open(file, impdef->imp_keys, impdef->imp_dim, YY_FILE);

    if(AYY == 0) {
	kerror(0,"Cannot open '%s'", file);
	return(-1);
    }

    return(0);
}


IMP_vec_casc(name, cmt)
char    *name;
char    **cmt;
{
    YYFILE  *yy = AYY;
    int     key;

    key = YY_lex(yy);
    if(key == YY_EOF) goto err;

    if(key == YY_WORD || key == YY_STRING) {
	SCR_strlcpy(name, yy->yy_text, 79); /* JMP 13-02-2013 */
	name[80] = '\0';
    }
    else goto err;

    key = YY_lex(yy);
    if(key == YY_EOF) return(-1);

    if(key == YY_WORD || key == YY_STRING)
	*cmt =  SCR_stracpy(yy->yy_text);
    else goto err;

    return(0);

err :
    return(-1);
}

IMPDEF IMP_ASC_CMT = {
    NULL,
    0,
    IMP_hd_casc,
    IMP_vec_casc,
    NULL,
    NULL
};


