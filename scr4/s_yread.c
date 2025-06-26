#include "s_ydvlp.h"

int YY_NO_COMMENT2 = 0; /* JMP 03-03-2004 */

/*NH*/
/* =+====== YY_COMMENT ==========================================
     Reads one comment
     yy : IO : YYFILE pointer
   ============================================================== */

YY_comment(yy)
YYFILE  *yy;
{
    int     ch;

cmt :
    do ch = YY_getc(yy);
    while( (ch != YY_E_COMMENT[0]) && (ch != YY_EOF));
    if(ch == YY_EOF) {
	yy->yy_type = YY_EOF;
	return(0);
	}
    if(YY_E_COMMENT[1] == 0) goto fin;
    ch = YY_getc(yy);
    if(ch != YY_E_COMMENT[1]) {
	YY_ungetc(ch, yy);
	goto cmt;
	}
fin:
    yy->yy_type = YY_COMMENT;
    return(0);
}

/*NH*/
/* =+====== YY_COMMENT2 ==========================================
     Reads one comment type //
     yy : IO : YYFILE pointer
   ============================================================== */

YY_comment2(yy)
YYFILE  *yy;
{
    YY_skip_to_eol(yy);
    yy->yy_type = YY_COMMENT;
    return(0);
}

/*NH*/
YY_add_to_text(yy, pos, ch)
YYFILE  *yy;
int     pos, ch;
{
    unsigned char   *ptr;

    if(pos >= yy->yy_text_lg) { /* JMP 22-10-01 */
	if(pos >= YY_MAX_STRING_LG) {
	    if(yy->yy_msg == 0)
		fprintf(stderr, "%s : string exceeds %d chars\n",
				  YY_error(yy), YY_MAX_STRING_LG); /* JMP 16-10-01 */
	    yy->yy_msg = 1;
	    return(-1);
	    }
	//ptr = malloc(yy->yy_text_lg + 100);
	ptr = SCR_malloc(yy->yy_text_lg + 100); /* JMP 22-08-2012 */
	if(ptr == 0) {
	    fprintf(stderr, "%s : memory full\n", YY_error(yy));
	    return(-1);
	    }
	if(yy->yy_text != 0) {
	    memcpy(ptr, yy->yy_text, yy->yy_text_lg);
	    //free(yy->yy_text);
	    SCR_free(yy->yy_text); /* JMP 22-08-2012 */
	    }
	yy->yy_text = ptr;
	yy->yy_text_lg += 100;
	}
    yy->yy_text[pos] = ch;
    yy->yy_msg = 0;
    return(0);
}

/* ====================================================================
Saute la suite du texte jusqu'au prochain caractère non considéré comme
espace. Les espaces sont :

&EN les blancs,
&EN les TAB,
&EN les Vertical TAB,
&EN les Newline,
&EN les Carriage Return,
&EN les FormFeed

Le pointeur vers le texte est placé sur le premier caractère suivant.
&SA YY_skip_to_eol(), YY_skip_to_char(), YY_read_to_char(),
    YY_read_to_string()
=======================================================================*/

int YY_skip_spaces(yy)
YYFILE  *yy;
{
    int     ch;

    do ch = YY_getc(yy);
    // while(isspace(ch));
    while(SCR_is_space(ch));
    if(ch != YY_EOF) YY_ungetc(ch, yy);
    return(0);
}


/* ====================================================================
Retient le dernier élément lu pour lui permettre d'être relu lors de la
prochaine utilisation de YY_lex() ou de YY_read(). Cette fonction n'a
d'effet que pour YY_lex() et YY_read().

Cela permet de simplifier les analyses syntaxiques dans le cas ou une
élément logique n'est pas terminé par un caractère spécial. Ainsi, pour
la lecture de séries statistiques, il suffit d'utiliser de lire tant que
des nombres sont trouvés, puis de faire YY_unread() pour laisser le
dernier élément lu disponible pour la lecture suivante.

&EX
    switch(YY_lex()) {
	case YY_DOUBLE : ...
	case ...
	default : YY_unread(yy);
		  return;
	}

&TX
&SA YY_read(), YY_lex()
=======================================================================*/

int YY_unread(yy)
YYFILE  *yy;
{
    yy->yy_reread = 1;
    return(0);
}

/* ====================================================================
Saute la suite du texte jusqu'au prochain \n ou jusqu'à la fin du
fichier (to End Of Line).

Le pointeur vers le texte est placé sur le premier caractère suivant.

&SA YY_skip_spaces(), YY_skip_to_char(), YY_read_to_char(),
    YY_read_to_string()
=======================================================================*/

int YY_skip_to_eol(yy)
YYFILE  *yy;
{
    YY_skip_to_char(yy, '\n');
    return(0);
}

/* ====================================================================
Saute la suite du texte jusqu'au prochain caractère spécifié ou jusqu'à la
fin du fichier.

Le pointeur vers le texte est placé sur le premier caractère suivant.

&SA YY_skip_spaces(), YY_skip_to_eol(), YY_read_to_char(),
    YY_read_to_string()
=======================================================================*/

int YY_skip_to_char(yy, c)
YYFILE  *yy;
int     c;
{
    int     ch;

    while(1) {
	ch = YY_getc(yy);
	if(ch == EOF || ch == c) return(0);
	}

}

/* ====================================================================
Lit la suite du texte jusqu'au caractère spécifié ou jusqu'à la fin du
fichier ou jusqu'à ce que YY_MAX_STRING_LG caractères ne soient lus.

Les caractères lus sont placés dans le buffer yy->yy_text.
Le caractère limite n'est pas placé dans ce buffer.

Si la taille du buffer (YY_MAX_STRING_LG) est dépassée, la lecture
continue mais les caractères ne sont pas sauvés dans yy->yy_text,
même si le caractère désiré n'a pas été rencontré.

Le pointeur vers le texte est placé sur le premier caractère suivant.

&RT le dernier caractère lu.

&SA YY_skip_to_eol(), YY_skip_spaces(), YY_read_to_char(),
    YY_read_to_string(), YY_read_to_chars()
=======================================================================*/


int YY_read_to_char(yy, c)
YYFILE  *yy;
int     c;
{
    unsigned char  chars[2];

    chars[0] = c;
    chars[1] = 0;
    return(YY_read_to_chars(yy, chars));
}
/*NH*/
int YY_isin(ch, chars)
int             ch;
unsigned char   *chars;
{
    int     i;

    for(i = 0 ; chars[i] ; i++)
	if(chars[i] == ch) return(1);
    return(0);
}

/* ====================================================================
Lit la suite du texte jusqu'à l'un des caractères spécifiés ou jusqu'à
la fin du fichier ou jusqu'à ce que YY_MAX_STRING_LG caractères ne
soient lus.

Les caractères lus sont placés dans le buffer yy->yy_text.
Le caractère limite n'est pas placé dans ce buffer.

Si la taille du buffer (YY_MAX_STRING_LG) est dépassée, la lecture
continue mais les caractères ne sont pas sauvés dans yy->yy_text,
même si le caractère désiré n'a pas été rencontré.

Le pointeur vers le texte est placé sur le premier caractère suivant.

&RT le dernier caractère lu.

&SA YY_skip_to_eol(), YY_skip_spaces(), YY_read_to_char(),
    YY_read_to_string()
=======================================================================*/

int YY_read_to_chars(yy, chars)
YYFILE        *yy;
unsigned char *chars;
{
    int     ch, lg = 0;

    while(1) {
	ch = YY_getc(yy);
	if(ch == EOF || YY_isin(ch, chars)) {
	    yy->yy_lg = lg;
	    YY_add_to_text(yy, lg, 0);
	    return(ch);
	    }
	YY_add_to_text(yy, lg, ch);
	lg++;
	}
}

/* ====================================================================
Lit la suite du texte jusqu'au string spécifié ou jusqu'à la fin du
fichier ou jusqu'à ce que YY_MAX_STRING_LG caractères ne soient lus.

Les caractères lus sont placés dans le buffer yy->yy_text.
Le string limite n'est pas placé dans ce buffer.

Si la taille du buffer (YY_MAX_STRING_LG) est dépassée, la lecture
continue jusqu'au string désiré. Un message d'erreur en affiché.

Le pointeur vers le texte est placé sur le premier caractère suivant le
string cible.

&SA YY_skip_to_eol(), YY_skip_spaces(), YY_read_to_char(),
    YY_skip_to_string()
=======================================================================*/


int YY_read_to_string(yy, str)
YYFILE          *yy;
unsigned char   *str;
{
    int     ch, lg = 0, str_lg = (int)strlen(str);

    while(1) {
	ch = YY_getc(yy);
	if(ch == EOF) {
	    yy->yy_lg = lg;
	    YY_add_to_text(yy, lg, 0);
	    fprintf(stderr, "%s : Closing string not found\n", YY_error(yy));
	    return(-1);
	    }
	YY_add_to_text(yy, lg++, ch);
	if(lg >= str_lg &&
	    memcmp(yy->yy_text + lg - str_lg, str, str_lg) == 0) {
		YY_add_to_text(yy, lg - str_lg, 0);
		yy->yy_lg = lg - str_lg;
		return(0);
		}
	}
}

/* ====================================================================
Lit le prochain élément du texte. Le type est retourné et la valeur
est sauvée dans la structure YYFILE. La liste suivante reprend les types
possibles et l'élément de la structure YYFILE qui les contient.

Les strings et les lettres peuvent contenir des caractères spéciaux du
type de ceux acceptés en C ('\n', '\033', '^X', ...)

Le type est toujours sauvé dans yy->yy_type.

&EN YY_EOF : fin de fichier

&EN YY_LONG (123) : entier (toujours positif) sauvé dans yy->yy_long

&EN YY_DOUBLE (1.23) : réel contenant un point décimal ou au format
    exponentiel (1E10) sauvé dans yy->yy_double

&EN YY_STRING ("xxxxx") : string entouré de doubles quotes sauvé dans
    yy->yy_text

&EN YY_SPECIAL (*) : caractère non alphabétique sauvé dans
    yy->yy_text[0]

&EN YY_COMMENT : commentaire compris entre YY_B_COMMENT et
    YY_E_COMMENT et non sauvé dans la structure

&EN YY_LETTER ('\033') : lettre entre simple quotes sauvé dans
    yy->yy_text[0]

&EN YY_WORD (TITLE) : mot (commençant par une lettre et ne contenant que
    des chiffres, des lettres ou le caractère _) sauvé dans yy->yy_text

&EN YY_UNKNOWN : directive incorrectement placée (##endif sans ##ifdef
    par exemple)

&EN YY_MEMORY_FULL en cas de problèmes d'allocation en général suite à
    un ##include file

&EN YY_ERROR : directive inconnue ou erreur dans la définition des
    directives


&RT type de l'élément lu
&EX
    if(YY_read(yy) != YY_STRING)
	fprintf(stderr, "%s :string expected\n", YY_error(yy));
&TX
&SA YY_lex()
=======================================================================*/

int YY_read(yy)
YYFILE  *yy;
{
    int     ch, ch1;
    YYFILE  *yy_root;

    if(yy->yy_reread != 0) {
	yy->yy_reread = 0;
	return(yy->yy_type);
	}
again:
    YY_skip_spaces(yy);
    ch = YY_getc(yy);
    yy->yy_line = yy->yy_c_line;
    yy->yy_col = yy->yy_c_col;

/* Comments : old version
    if(ch == YY_B_COMMENT[0]) {
	if(YY_B_COMMENT[1] != 0) {
	    ch1 = YY_getc(yy);
	    if(ch1 != YY_B_COMMENT[1]) {
		if(ch1 != YY_EOF) YY_ungetc(ch1, yy);
		goto cnt;
		}
	    }
	YY_comment(yy);
	return(yy->yy_type);
	}
*/

/* Comments : new version */
    if(ch == YY_B_COMMENT[0]) {
	ch1 = YY_getc(yy);
	if(ch1 == YY_B_COMMENT[1]) {
	    YY_comment(yy);
	    return(yy->yy_type);
	    }

	if(ch1 == YY_B_COMMENT[0] && YY_NO_COMMENT2 == 0) { /* JMP 03-03-2004 */
	    YY_comment2(yy);
	    return(yy->yy_type);
	    }

	if(ch1 != YY_EOF) YY_ungetc(ch1, yy);
	}
//cnt:
    switch(ch) {
	case YY_STRING_DELIMITER :
	    YY_read_string(yy, ch);
	    yy->yy_type = YY_STRING;
	    return(YY_STRING);

	case '\'' :
	    YY_read_letter(yy);
	    yy->yy_type = YY_LETTER;
	    return(YY_LETTER);

	case '$' :
	    if(YY_find_def(yy) == 0) goto again;
	    YY_add_to_text(yy, 0, '$');
	    YY_add_to_text(yy, 1, 0);
	    return(yy->yy_type = YY_SPECIAL);

	case '#' :
	    YY_read_word(yy);
	    if(strcmp((char *)yy->yy_text, "include") == 0) {
		if(YY_include(yy) != 0) return(YY_ERROR);
		goto again;
		}
	    if(strcmp((char *)yy->yy_text, "define") == 0) {
		if(YY_define(yy) != 0) return(YY_ERROR);
		goto again;
		}
	    if(strcmp((char *)yy->yy_text, "ifdef") == 0) {
		if(YY_ifdef(yy, 1) != 0) return(YY_ERROR);
		goto again;
	    }
	    if(strcmp((char *)yy->yy_text, "ifndef") == 0) {
		if(YY_ifdef(yy, 0) != 0) return(YY_ERROR);
		goto again;
	    }
	    if(strcmp((char *)yy->yy_text, "endif") == 0) {
		if(NBIF < 1) return(YY_UNKNOWN);
		NBIF--;
		return(YY_COMMENT);
	    }
	    if(strcmp((char *)yy->yy_text, "else") == 0) {
		if(NBIF < 1) return(YY_UNKNOWN);
		if(YY_find_end_else(yy)) return(YY_UNKNOWN);
		return(YY_COMMENT);
	    }
	    if(strcmp((char *)yy->yy_text, "debug") == 0) {
		YY_debug(yy);       /* JMP 27-10-91 */
		YY_skip_to_eol(yy); /* JMP 27-10-91 */
		return(YY_COMMENT);
	    }
	    return(YY_ERROR);

	case YY_EOF :
	    if(yy->yy_include != 0) {
		if(yy->yy_input_type == YY_FILE) fclose(yy->yy_fd);
		yy_root = yy->yy_include;

		/* SAVE DEFINES */
		yy_root->yy_nb_dyndefs = yy->yy_nb_dyndefs;
		yy_root->yy_dyndefs =    yy->yy_dyndefs;
		memcpy((char *)yy, (char *)yy_root, sizeof(YYFILE));
		//free(yy_root);
		SCR_free(yy_root); /* JMP 22-08-2012 */
		goto again;
		}
	    yy->yy_type = YY_EOF;
	    return(YY_EOF);

	default :
	    // if( isdigit(ch)) {
	    if(ch >= '0' && ch <= '9') {  /* JMP 29-03-00 */
		    YY_ungetc(ch, yy);
		    YY_read_num(yy, 0);
		    return(yy->yy_type);
		    }

	    // if( isalpha(ch) || ch == '_') {
	    if(SCR_is_alpha(ch) || ch == '_') { /* JMP 29-03-00 */
		    YY_ungetc(ch, yy);
		    YY_read_word(yy);
		    yy->yy_type = YY_WORD;
		    return(yy->yy_type);
		    }

	    YY_add_to_text(yy, 0, ch);
	    YY_add_to_text(yy, 1, 0);
	    yy->yy_type = YY_SPECIAL;
	    if(ch == '.') {
		ch = YY_getc(yy);
		YY_ungetc(ch, yy);
		if(ch >= '0' && ch <= '9') YY_read_num(yy, 1);
		}
	    return(yy->yy_type);

	}

}

/*NH*/
/* =+===== YY_READ_CONTROL ===============================================
    Read a control character in the data stream yy->yy_fd. The ^ must be
    already read. Only ^a to ^z and ^[ are recognized. Other control must
    be represented in octal or hexa (see YY_read_escape). If the
    letter read is not in a-z or A-Z, the function returns ^ and
    ungets the character.
   ====================================================================== */

YY_read_control(yy)
YYFILE  *yy;
{
    int     ch;

    ch = YY_getc(yy);
    if(ch == '[') return(033); /* escape */
    if(ch <= 'Z' && ch >= 'A') return(ch - 'A' + 1);
    if(ch <= 'z' && ch >= 'a') return(ch - 'a' + 1);
    if(ch != YY_EOF) YY_ungetc(ch, yy);
    return('^');
}
/*NH*/
/* =+===== YY_READ_ESCAPE ==============================================
    Read an escape sequence in the data stream yy->yy_fd. The \ must be
    already read. Possible escapes are :
       \0.. : octal
       \x.. : hexa
       \d... : decimal
       \n, \t, ... : standard C sequences + '\^'
    yy : IO : pointer to YYFILE
    Return : integer = character represented by the sequence
   ==================================================================== */

YY_read_escape(yy)
YYFILE  *yy;
{
    char    buf[4];
    int     ch;
    int     i;

    ch = YY_getc(yy);
    switch(ch) {
	case YY_EOF  : return(YY_EOF);
	case '\'' : return('\'');
	case '\"' : return('\"');
	case '\\' : return('\\');
	case 'a'  : return('\a');
	case 'b'  : return('\b');
	case 'f'  : return('\f');
	case 'n'  : return('\n');
	case 'r'  : return('\r');
	case 't'  : return('\t');
	case 'v'  : return('\v');
	case '^'  : return('^');
	case 'x'  :
	case 'X'  :
	    for(i = 0 ; i < 2 ; i++) {
		buf[i] = YY_getc(yy);
		if(buf[i] == YY_EOF) return(YY_EOF);
		}
	    buf[2] = 0;
	    return(U_hex_to_dec(buf));
	case 'd'  :
	case 'D'  :
	    for(i = 0 ; i < 3 ; i++) {
		buf[i] = YY_getc(yy);
		if(buf[i] == YY_EOF) return(YY_EOF);
		}
	    buf[3] = 0;
	    return(atoi(buf));
	case '0' :
	case '1' :
	case '2' :
	case '3' :
	case '4' :
	case '5' :
	case '6' :
	case '7' :
	    buf[0] = ch;
	    for(i = 1 ; i < 3 ; i++) {
		buf[i] = YY_getc(yy);
		if(buf[i] == YY_EOF) return(YY_EOF);
		}
	    buf[3] = 0;
	    return(U_octal_to_dec(buf));

	default : return(ch);
	}
}
/*NH*/
/* =+====== YY_read_word ===========================================
     Reads a word on the input steam. A '\0' is placed at the end of the word.
     It is terminated by an alphanumeric
     character (undescore is allowed in a word). The last character read is
     ungot to the input stream.
     yy     : IO : pointer to the yyfile descriptor
     return : length of the word read
   ============================================================== */

YY_read_word(yy)
YYFILE  *yy;
{
    int     i = 0;
    int     ch = 0;

    while(1)
    {
	ch = YY_getc(yy);
	//if( !(isalpha(ch) || isdigit(ch) || ch == '_') ) {
	if( !(SCR_is_alpha(ch) || SCR_is_num(ch) || ch == '_') ) {
	    if(ch != YY_EOF) YY_ungetc(ch, yy);
	    YY_add_to_text(yy, i, 0);
	    yy->yy_lg = i;
	    return(i);
	    }
	YY_add_to_text(yy, i++, ch);
    }
}
/*NH*/
/* =+====== YY_read_letter ===========================================
     Reads a character on the input steam (enclosed between single quotes).
     yy     : IO : file descriptor of the input stream
     letter : OU : pointer where the letter is to be written
     return : 0
   ============================================================== */

YY_read_letter(yy)
YYFILE  *yy;
{
    int     ch;

    ch = YY_getc(yy);
    switch(ch) {
	case '\\' :
	    ch = YY_read_escape(yy);
	    break;
	case '^' :
	    ch = YY_read_control(yy);
	    break;
	default :
	    break;
	}

    YY_add_to_text(yy, 0, ch);
    YY_add_to_text(yy, 1, 0);

    do ch = YY_getc(yy);
    while( (ch != '\'') && (ch != YY_EOF) );
    return(0);
}
/*NH*/
/* =+====== YY_read_num ================================================
     Reads an non signed numeric on the input steam. The last character
     read is unget to the input stream.
     yy     : IO : YYFILE pointer
     dbl    : IN : 1 only if '.' already read
     return : type read (YY_LONG or YY_DOUBLE)
   ============================================================== */

YY_read_num(yy, dbl)
YYFILE  *yy;
int     dbl;
{
    int     i = 0;
    int     ch;
    int     ee = 0;
    int     quit = 0;

    if(dbl) i = 1;
    while(quit == 0) {
	ch = YY_getc(yy);
	if(ch <= '9' && ch >= '0')
	    YY_add_to_text(yy, i++, ch);
	else switch(ch) {
	    case 'e' :
	    case 'E' :
		if(ee != 0) {
		    YY_ungetc(ch, yy);
		    quit = 1;
		    }
		else {
		    YY_add_to_text(yy, i++, 'E');
		    dbl = ee = 1;
		    ch = YY_getc(yy);
		    if(ch == '-' || ch == '+')
			YY_add_to_text(yy, i++, ch);
		    else YY_ungetc(ch, yy);
		    }
		break;

	    case '.' :
		if(dbl != 0) {
		    YY_ungetc(ch, yy);
		    quit = 1;
		    }
		else {
		    YY_add_to_text(yy, i++, '.');
		    dbl = 1;
		    }
		break;

	    default :
		if(ch != YY_EOF) YY_ungetc(ch, yy);
		quit = 1;
		break;
	    }
	}

    YY_add_to_text(yy, i, 0);
    if(dbl == 0 && i < 10) {            /*JMP 2/93*/
	sscanf(yy->yy_text, "%ld", &yy->yy_long);
	yy->yy_type = YY_LONG;
	}
    else {
	yy->yy_double = SCR_atof((char *)yy->yy_text);
	yy->yy_type = YY_DOUBLE;
	}
    return(yy->yy_type);
}
/*NH*/
/* =+====== YY_read_string ==================================================
     Reads a string on the input steam. A '\0' is placed a the end of the string.
     The input stream is read to the next limit character.
     This character preceeded by a backslash is taken in the string.
     The stream pointer is placed after the limit character.
     The tabs are replaced by n blanks (1 tab = 8 blanks).
     The newline, caret are skipped.

	yy     : IN : yyfile descriptor
	quote  : string delimiter
	return : length of the string read
   ============================================================== */

YY_read_string(yy, quote)
YYFILE  *yy;
int     quote;
{
    int     i = 0;
    int     ch = 0;

    while(1) {
	ch = YY_getc(yy);
	switch(ch) {
	    case '\\' :
		ch = YY_read_escape(yy);
		break;

	    case '^' :
		ch = YY_read_control(yy);
		break;

	    default  :
		if(ch == quote || ch == YY_EOF) { /* JMP 15-09-10 */
		    YY_add_to_text(yy, i, 0);
		    YY_add_to_text(yy, i + 1, 0);
		    yy->yy_lg = i;
		    return(i);
		    }
		break;
	     }

	YY_add_to_text(yy, i++, ch);
	}
}
/*NH*/
U_hex_to_dec(buf)
char    *buf;
{
    int     res = 0;
    int     i;
    int     mult = 1;

    for(i = 1 ; i >= 0 ; i--) {
	if( ('0' <= buf[i]) &&  ('9' >= buf[i]) )
	     res += mult * (buf[i] - '0');
	else res += mult * (10 + buf[i] - 'A');
	mult *= 16;
	}
    return(res);
}
/*NH*/
U_octal_to_dec(buf)
char    *buf;
{
    int     res = 0;
    int     i;
    int     mult = 1;

    for(i = 2 ; i >= 0 ; i--) {
	res += mult * (buf[i] - '0');
	mult *= 8;
	}
    return(res);
}







