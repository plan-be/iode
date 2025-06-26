#include <stdio.h>
#include <stdlib.h>
#include "s_dir.h"
#include "s_isc.h"
#include "s_strs.h"

#ifdef LINUX
#include <errno.h>
#endif
/*================================================
Duplicie un fichier avec ou sans bufferisation (de l'output).

&EN in = filename input
&EN out = filename output
&EN buf : -1 pour non bufferisé, 0 pour la bufferisation par défaut,
	  n pour une bufferisation sur n bytes

&RT
&EN 0 : OK
&EN -1 : Cannot open input file
&EN -2 : Cannot open output file
&EN -3 : Error reading input file
&EN -4 : Error writing output file
================================================ */

SCR_copy_file_ex(in, out, buf)
char    *in, *out;
int     buf;
{
    FILE    *fi, *fo;
    int     ch, rc = 0;

#ifdef DOS
    fi = ISC_fopen(in, "rb");
#else
    fi = ISC_fopen(in, "r");
#endif

    if(fi == 0) return(-1);

#ifdef DOS
    fo = ISC_fopen(out, "wb+");
#else
    fo = ISC_fopen(out, "w+");
#endif

    if(fo == 0) {
	ISC_fclose(fi);
	return(-2);
	}

    switch(buf) {
	case -1 : setvbuf(fo, (char *)0, _IONBF, 0); break;
	case 0  : break;
	default : setvbuf(fo, (char *)0, _IOFBF, buf); break;
	}

    while(1) {
	ch = ISC_getc(fi);          /* JMP 11-08-98 */
	if(ch == EOF) break;
	ch = ISC_putc(ch, fo);
	if(ch == EOF) {
	    Debug("putc(ch, %s) = %d\n", out, ch); /* JMP 02-12-00 */
	    break;
	    }
	}
/*
    if(ISC_ferror(fi)) rc = -3;
    if(ISC_ferror(fo)) rc = -4;
*/
    ISC_fclose(fi);
    if(ISC_fclose(fo)) rc = -4;
    return(rc);
}


/*================================================
Duplicie un fichier.

&RT
&EN 0 : OK
&EN -1 : Cannot open input file
&EN -2 : Cannot open output file
&EN -3 : Error reading input file
&EN -4 : Error writing output file
================================================ */

SCR_copy_file(in, out)
char    *in, *out;
{
    return(SCR_copy_file_ex(in, out, 0));
}


/*================================================
Duplicie un fichier en flushant tous les buf bytes.

&EN in = filename input
&EN out = filename output
&EN buf : flush tous les buf bytes

&RT
&EN 0 : OK
&EN -1 : Cannot open input file
&EN -2 : Cannot open output file
&EN -3 : Error reading input file
&EN -4 : Error writing output file
================================================ */

SCR_copy_file_flush(in, out, buf)
char    *in, *out;
int     buf;
{
    FILE    *fi, *fo;
    int     ch, rc = 0;
    long    nb = 0;

#ifdef DOS
    fi = fopen(in, "rb");
#else
    fi = fopen(in, "r");
#endif

    if(fi == 0) return(-1);

#ifdef DOS
    fo = fopen(out, "wb+");
#else
    fo = fopen(out, "w+");
#endif

    if(fo == 0) {
	fclose(fi);
	return(-2);
	}

    while(1) {
	ch = getc(fi);          /* JMP 11-08-98 */
	if(ch == EOF) break;
	ch = putc(ch, fo);
	if(ch == EOF) break;
	nb++;
	if(nb >= buf) {
	    if(fflush(fo) != 0) break;
	    nb = 0;
	    }
	}

    if(ferror(fi)) rc = -3;
    if(ferror(fo)) rc = -4;

    fclose(fi);
    if(fclose(fo)) rc = -4;
    return(rc);
}

/*================================================
Append un fichier en flushant tous les buf bytes.

&EN in = filename input
&EN out = filename output
&EN buf : flush tous les buf bytes

&RT
&EN 0 : OK
&EN -1 : Cannot open input file
&EN -2 : Cannot open output file
&EN -3 : Error reading input file
&EN -4 : Error writing output file
================================================ */

SCR_append_file_flush(in, out, buf)
char    *in, *out;
int     buf;
{
    FILE    *fi, *fo;
    int     ch, rc = 0;
    long    nb = 0;

#ifdef DOS
    fi = fopen(in, "rb");
#else
    fi = fopen(in, "r");
#endif

    if(fi == 0) return(-1);

#ifdef DOS
    fo = fopen(out, "ab+");
#else
    fo = fopen(out, "a+");
#endif

    if(fo == 0) {
	fclose(fi);
	return(-2);
	}

    while(1) {
	ch = getc(fi);          /* JMP 11-08-98 */
	if(ch == EOF) break;
	ch = putc(ch, fo);
	if(ch == EOF) break;
	nb++;
	if(nb >= buf) {
	    if(fflush(fo) != 0) break;
	    nb = 0;
	    }
	}

    if(ferror(fi)) rc = -3;
    if(ferror(fo)) rc = -4;

    fclose(fi);
    if(fclose(fo)) rc = -4;
    return(rc);
}


/*================================================
Append un fichier.

&RT
&EN 0 : OK
&EN -1 : Cannot open input file
&EN -2 : Cannot open output file
&EN -3 : Error reading input file
&EN -4 : Error writing output file
================================================ */

int SCR_append_file(char* in, char* out)
{
    return(SCR_append_file_flush(in, out, 2048));
}



