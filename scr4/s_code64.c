#include "scr4.h"

static char charCodes[] = {
    'A','B','C','D','E','F','G','H',
    'I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X',
    'Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3',
    '4','5','6','7','8','9','+','/',
    0};

/******************************************************************************
Code un buffer en base64.

&EN in : buffer input
&EN lg : longueur du buffer input
&EN out : buffer ouput

Le buffer résultat (out) doit être suffisamment grand pour contenir 1+ (4 x lg) / 3 bytes.

&EX
    SCR_code64(pg->pg_rec, pg->pg_rec_len, buf);
    printf("out='%s'\n", buf);
    SCR_deecode64(buf, pg->pg_rec_len, pg->pg_rec);
&TX

&SA SCR_decode64()
*******************************************************************************/

SCR_code64(U_ch *in, int lg, U_ch *out)
{
    int             i = 0, ch, nc = 0, nct = 0;
    unsigned char   bufin[3];

    while(1) {
	ch = in[i];
	if(i == lg) {
	    switch(nc) {
		case 0 : break;
		case 1 :
		    out[nct + 0] = charCodes[((bufin[0] >> 2) & 0x3f)];
		    out[nct + 1] = charCodes[((bufin[0] & 0x3) << 4)];
		    out[nct + 2] = '=';
		    out[nct + 3] = '=';
		    out[nct + 4] = 0;
		    break;
		case 2 :
		    out[nct + 0] = charCodes[((bufin[0] >> 2) & 0x3f)];
		    out[nct + 1] = charCodes[((bufin[0] & 0x3) << 4)|((bufin[1] >> 4) & 0xf)];
		    out[nct + 2] = charCodes[(bufin[1] & 0xf) << 2];
		    out[nct + 3] = '=';
		    out[nct + 4] = 0;
		    break;

		}
	    break;
	    }
	bufin[nc++] = ch;
	if(nc == 3) {
	    out[nct + 0] = charCodes[bufin[0] >> 2];
	    out[nct + 1] = charCodes[((bufin[0] << 6 & 0xff) >> 2) | (bufin[1] >> 4)];
	    out[nct + 2] = charCodes[((bufin[1] & 0xf) << 2) | (bufin[2] >> 6)];
	    out[nct + 3] = charCodes[bufin[2] & 0x3f];
	    out[nct + 4] = 0;
	    nct += 4;
	    nc = 0;
	    }
	i++;
	}
    return(0);
}

/******************************************************************************
Decode un buffer codé en base64 par SCR_code64().

&EN in : buffer input (codé 64)
&EN lg : longueur du buffer output (!!)
&EN out : buffer output (décodé)

Le buffer résultat (out) doit être suffisamment grand pour contenir lg bytes.
Attention, lg est bien la longueur du buffer AVANT codage (celle de out,
donc après décodage).

&EX
    SCR_code64(pg->pg_rec, pg->pg_rec_len, buf);
    printf("out='%s'\n", buf);
    SCR_decode64(buf, pg->pg_rec_len, pg->pg_rec);
&TX

&SA SCR_code64()
*******************************************************************************/

SCR_decode64(U_ch *in, int lg, U_ch *out)
{
    int             i = 0, ch, nc = 0, nct = 0;
    int             bufin[5];

    if(lg == 0) return(0);
    lg =  4 * (1 + (lg - 1) / 3);

    while(1) {
	bufin[nc] = U_pos(in[i], charCodes);
	nc++;
	if(nc == 4) {
	    out[nct + 0] = (bufin[0] << 2 & 0xff)| bufin[1] >> 4;
	    if(bufin[2] < 0) return(0);
	    out[nct + 1] = (bufin[1] << 4 & 0xff) | bufin[2] >> 2;
	    if(bufin[3] < 0) return(0);
	    out[nct + 2] = (bufin[2] << 6 & 0xff) | bufin[3];
	    nct += 3;
	    nc = 0;
	    }
	i++;
	if(i == lg) break;
	}
    return(0);
}




// Base 64 autre source 2014
/*
int SCR_base64_encode_string(const char *in, int in_len, char *out, int out_size)
{
    char encode[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			     "abcdefghijklmnopqrstuvwxyz0123456789+/"; 
    char decode[] = "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW"
			     "$$$$$$XYZ[\\]^_`abcdefghijklmnopq";
	unsigned char triple[3];
	int i;
	int len;
	int line = 0;
	int done = 0;

	while (in_len) {
		len = 0;
		for (i = 0; i < 3; i++) {
			if (in_len) {
				triple[i] = *in++;
				len++;
				in_len--;
			} else
				triple[i] = 0;
		}
		if (!len)
			continue;

		if (done + 4 >= out_size)
			return -1;

		*out++ = encode[triple[0] >> 2];
		*out++ = encode[((triple[0] & 0x03) << 4) |
					     ((triple[1] & 0xf0) >> 4)];
		*out++ = (len > 1 ? encode[((triple[1] & 0x0f) << 2) |
					     ((triple[2] & 0xc0) >> 6)] : '=');
		*out++ = (len > 2 ? encode[triple[2] & 0x3f] : '=');

		done += 4;
		line += 4;
	}

	if (done + 1 >= out_size)
		return -1;

	*out++ = '\0';

	return done;
}
*/











