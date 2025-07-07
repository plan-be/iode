#include "s_ydvlp.h"

/*  This file includes the following functions :
 *
 *      - YYFILE *YY_open(filename) : opens one file
 *      - YY_close(YYFILE *)        : close one file
 *      - YY_getc(YYFILE *)         : read a character
 *      - YY_ungetc(int, YYFILE *)  : read a character
 *      - YY_read(YYFILE *)         : read an element of unknown type
 *      - YY_unread(YYFILE *)       : unread an the last read element
 *      - YY_lex(YYFILE *, YYKEYS *): reads an unknown element and return
 *                                    the type found and the value if keyword
 *      - char *YY_error(YYFILE *)  : creates an error message
 *      YY_errno contain the last error code
 */

int     YY_errno = 0;
char    YY_B_COMMENT[5] = "/*",
	YY_E_COMMENT[5] = "*/";
int     YY_CASE_SENSITIVE = 0;


/* ====================================================================
Ouvre un fichier ou un string pour l'interpréter à l'aide des fonctions
de la librairie s_yy. Le paramètre filename donne le nom du fichier ou
le pointeur vers le string à interpréter.

type indique s'il s'agit d'un fichier (YY_FILE), du standard input
(YY_STDIN) ou d'un string en mémoire (YY_MEM). Ces valeurs sont définies
dans s_yy.h :

&EN YY_FILE         0
&EN YY_MEM          1
&EN YY_STDIN        2

table est un pointeur vers un tableau de structures YYKEYS qui
contiennent les définitions des mots-clés reconnus dans l'application.
size est le nombre d'éléments de cette table.

YY_open() alloue une structure YYFILE qui contiendra toutes les
informations utiles en cours de traitement. Cette structure sera libérée
par YY_close().

&RT pointeur vers une structure allouée YYFILE ou NULL si le fichier
n'est pas trouvé ou que la mémoire disponible est insuffisante. Dans ce
dernier cas un message est envoyé vers le stderr.
&EX
    ##include <s_yy.h>

    YYKEYS MY_KEYWS[] = {
	"ADRESSE",  1,
	"CHAFF",    2,
	"CODEPOST", 3,
	"NOM",      4,
	"PRENOM",   5
	};

    main()
    {
	YYFILE  *yy;
	int     type;

	yy = YY_open("test", MY_KEYWS,
		      sizeof(MY_KEYWS) / sizeof(YYKEYS), YY_FILE);
	if(yy == 0) {
	    fprintf(stderr, "Cannot open test\n");
	    exit(1);
	    }
	YY_close(yy);
    }

&TX
&SA YY_close()
=======================================================================*/
#ifdef __cplusplus
YYFILE *YY_open(
char    *filename,
YYKEYS  *table,
int     size,
int     type
)
#else
YYFILE *YY_open(filename, table, size, type)
char    *filename;
YYKEYS  *table;
int     size;
int     type;
#endif
{
    YYFILE  *yy;

    //yy = (YYFILE *)malloc(sizeof(YYFILE));
    yy = (YYFILE *) SCR_malloc(sizeof(YYFILE)); /* JMP 22-08-2012 */
    if(yy == 0) {
	fprintf(stderr, "YY_open() : Memory full\n");
	return(yy);
	}
    if(YY_init(yy, filename, table, size, type) == 0) return(yy);
    //free(yy);
    SCR_free(yy); /* JMP 22-08-2012 */
    return((YYFILE *)0);
}

/*NH*/
/* =+====== YY_INIT ===============================================
    Open a file for use with the YY_.. functions. (init only)
     filename : IN : name of the file
     return   : 0 of OK, -1 otherwise
   ============================================================== */

#ifdef __cplusplus
/*NH*/
int YY_init(
YYFILE  *yy,
char    *filename,
YYKEYS  *table,
int     size,
int     type
)
#else
int YY_init(yy, filename, table, size, type)
YYFILE  *yy;
char    *filename;
YYKEYS  *table;
int     size;
int     type;
#endif
{

    memset(yy, 0, sizeof(YYFILE));
    switch(type) {
	case YY_FILE :
	    yy->yy_fd = fopen(filename, "r");
	    if(yy->yy_fd == 0) return(-1);
	    //yy->yy_name = (unsigned char *)malloc(strlen(filename) + 1);
	    yy->yy_name = (unsigned char *)SCR_malloc((int)strlen(filename) + 1); /* JMP 22-08-2012 */
	    strcpy(yy->yy_name, filename);
	    break;
	case YY_MEM:
	    yy->yy_ptr = filename;
	    break;
	case YY_STDIN :
	    break;
	default:
	    fprintf(stderr, "YY_init() : illegal input type\n");
	    return(-1);
	}

    yy->yy_input_type = type;
    yy->yy_c_line   = 1;
    yy->yy_line     = 1;
    yy->yy_keys     = table;
    yy->yy_nb_keys  = size;
    return(0);
}
/*NH*/
/* =+====== YY_END =================================================
     Close a file previously opened by YY_open.
     yy : IN : YYFILE pointer
   ============================================================== */

int YY_end(yy)
YYFILE  *yy;
{
    int     i;

    if(yy == 0) return(-1);
    YY_close(yy->yy_include);
    if(yy->yy_input_type == YY_FILE) fclose(yy->yy_fd);
    if(DDEFS != 0 && yy->yy_include == 0) {
	for(i = 0 ; i < NBDDEFS ; i++) {
	    //if(DDEFS[i]->yd_def != 0) free(DDEFS[i]->yd_def);
	    if(DDEFS[i]->yd_def != 0) SCR_free(DDEFS[i]->yd_def); /* JMP 22-08-2012 */
	    // free(DDEFS[i]);
	    SCR_free(DDEFS[i]); /* JMP 22-08-2012 */
	    }
	//free(DDEFS);
	SCR_free(DDEFS); /* JMP 22-08-2012 */
	}
    //if(yy->yy_text != 0) free(yy->yy_text);
    if(yy->yy_text != 0) SCR_free(yy->yy_text); /* JMP 22-08-2012 */
    //if(yy->yy_name != 0) free(yy->yy_name);
    if(yy->yy_name != 0) SCR_free(yy->yy_name);  /* JMP 22-08-2012 */
    //if(yy->yy_record != 0) free(yy->yy_record);
    if(yy->yy_record != 0) SCR_free(yy->yy_record); /* JMP 22-08-2012 */
    return(0);
}

/* ====================================================================
Ferme le fichier ouvert à l'aide de YY_open() et libère l'espace alloué
pour la structure YYFILE corespondante (y compris les définitions
trouvées en cours d'interprétation du fichier).

&SA YY_open()
=======================================================================*/

int YY_close(yy)
YYFILE  *yy;
{
    if(yy == 0) return(-1);
    YY_end(yy);
    //free(yy);
    SCR_free(yy); /* JMP 22-08-2012 */
    return(0);
}

/* ====================================================================
Enregistre une suite de caractères dans la structure YYFILE. Ces
caractères seront les premiers à être lus par la suite suivant une
méthode LIFO (Last In First Out). Plusieurs enregistrements successifs
peuvent être réalisés, y compris s'il reste des caractères dans le
buffer d'enregistrement.

En cas de dépassement (YY_MAX_RECORD) de buffer, les caractères
excédentaires sont ignorés et un message est envoyé vers le stderr.

&EX
    YY_record(yy, "F3 {ABORT}");
    YY_lex(yy) retourne d'abord F3
    YY_lex(yy) retourne ensuite {
    YY_lex(yy) retourne ensuite ABORT
    YY_lex(yy) retourne ensuite }
&TX

&SA YY_ungetc(), YY_getc()
=======================================================================*/

int YY_record(yy, txt)
YYFILE  *yy;
unsigned char *txt;
{
    int             lg, i, add;
    unsigned char   *ptr;

    if(txt == 0) return(0); /* JMP 08-09-99 */
    lg = (int)strlen(txt);
    add = 100 * (1 + lg / 100);
    if(lg + yy->yy_rec_lg >= yy->yy_rec_alg) {
	if(add + yy->yy_rec_alg >= YY_MAX_RECORD) {
	    fprintf(stderr, "\nYY_record() : Cannot record more text.\n");
	    return(-1);
	    }
	//ptr = (unsigned char *) malloc(yy->yy_rec_alg + add);
	ptr = (unsigned char *) SCR_malloc(yy->yy_rec_alg + add); /* JMP 22-08-2012 */
	if(ptr == 0) {
	    fprintf(stderr, "\nYY_record() : Memory full.\n");
	    return(-1);
	    }
	if(yy->yy_record != 0) {
	    memcpy(ptr, yy->yy_record, yy->yy_rec_lg);
	    //free(yy->yy_record);
	    SCR_free(yy->yy_record); /* JMP 22-08-2012 */
	    }
	yy->yy_record = ptr;
	yy->yy_rec_alg += add;
	}

    /* SAVE REVERSE BYTES */
    ptr = yy->yy_record + yy->yy_rec_lg;
    for(i = 0 ; i < lg ; i++)
	ptr[i] = txt[lg - i - 1];
    ptr[lg] = 0;
    yy->yy_rec_lg += lg;
    return(0);
}

/* ====================================================================
Replace un caractère dans le texte. Ce caractère sera le premier lu lors
du prochain appel à une des fonctions de lecture.

YY_ungetc() fait appel à YY_record().
&EX
    YY_ungetc('a', yy);
&TX
&SA YY_record()
=======================================================================*/

int YY_ungetc(ch, yy)
int     ch;
YYFILE  *yy;
{
    char    buf[2];

    if(ch == YY_EOF) return(-1);
    buf[0] = ch;
    buf[1] = 0;
    YY_record(yy, buf);
    return(0);
}

/* ====================================================================
Lit le caractère suivant dans le texte. Avant de "consommmer" les
caractères de texte, le buffer constitué par les appels à YY_record() ou
YY_ungetc() est consulté. S'il n'est pas vide, il est utilisé. Dans le
cas contraire, la suite du texte est lue.

La position dans le fichier est maintenue par cette fonction (ligne et
colonne). Certains caractères sont traités de façon spéciale :

&EN TAB : transformé en une suite de blancs (alignés sur 8 colonnes), de
    sorte que plusieurs appels successifs à YY_getc() peuvent retourner
    un blanc pour un seul TAB dans le fichier.

&EN RETURN (\r) : ignoré

&EN \032 (fin de fichier en DOS) : ignoré

Cette fonction est utilisée par les autres fonctions de lecture
(YY_read(), YY_lex(), etc).

&NO Cette fonction est la fonction de lecture de plus bas niveau : elle
    ne gère pas les directives! Seules YY_read() et YY_lex() le font.

&RT le caractère suivant ou YY_EOF en cas de fin de fichier (ou de
    string).
&SA YY_record(), YY_ungetc(), YY_read(), YY_lex()
=======================================================================*/

YY_getc(yy)
YYFILE  *yy;
{
    int     ch;

    if(yy->yy_rec_lg > 0) {
	yy->yy_rec_lg --;
	return(yy->yy_record[yy->yy_rec_lg]);
	}

    if(yy->yy_blks > 0) {
	yy->yy_blks--;
	yy->yy_c_col++;
	return((int)' ');
	}

again :
    switch(yy->yy_input_type) {
	case YY_FILE : ch = getc(yy->yy_fd); break;
	case YY_MEM  : ch = yy->yy_ptr[yy->yy_cpos++];
		       if(ch == 0) {
			    ch = EOF;
			    yy->yy_cpos--;
			    }
		       break;
	case YY_STDIN: ch = getchar(); break;
	}

    if(ch >= ' ') {
	yy->yy_c_col++;
	return(ch);
    }
    switch(ch) {
	case '\t' :
	    yy->yy_blks = 7 - yy->yy_c_col % 8;
	    yy->yy_c_col++;
	    return((int)' ');

	case '\n' :
	    yy->yy_blks = 0;
	    yy->yy_c_line++;
	    yy->yy_c_col = 0;
	    return(ch);

	case '\032' :
	case '\r' : goto again;

	case EOF :
	    return(YY_EOF);

	default :
	    yy->yy_c_col++;
	    return(ch);
	}
}
/*NH*/
int YY_upper(str)
char    *str;
{
    SCR_upper(str);
/*    while(*str) {
	if(islower(*str)) *str = toupper(*str);
	str++;
	}
*/
    return(0);
}
