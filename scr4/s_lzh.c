/*
 * LZHUF.C English version 1.0
 * Based on Japanese version 29-NOV-1988
 * LZSS coded by Haruhiko OKUMURA
 * Adaptive Huffman Coding coded by Haruyasu YOSHIZAKI
 * Edited and translated to English by Kenji RIKITAKE
 * Rewritten for SCR by JMPaul 06-93
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "s_lzh.h"

#ifndef SCRPROTO
int (*LzhDisplayFn)() = 0;
extern long LzhGetSize();
#else
int (*LzhDisplayFn)(short) = 0;
extern long LzhGetSize();
#endif

unsigned long int  LZH_textsize = 0, LZH_codesize = 0, LZH_printcount = 0;

/* LZSS Parameters */

#define N             4096      /* Size of string buffer */
#define F               60      /* Size of look-ahead buffer */
#define THRESHOLD       2
#define NIL             N       /* End of tree's node  */

short             LZH_match_position, LZH_match_length;
unsigned char   *LZH_text_buf;
short             *LZH_lson, *LZH_rson, *LZH_dad;

/*NH*/
void LzhInitTree()  /* Initializing tree */
{
	short  i;

	for (i = N + 1; i <= N + 256; i++)
		LZH_rson[i] = NIL;                  /* root */
	for (i = 0; i < N; i++)
		LZH_dad[i] = NIL;                   /* node */
}

/*NH*/
void LzhInsertNode(r)  /* Inserting node to the tree */
short     r;
{
	short  i, p, cmp;
	unsigned char  *key;
	unsigned c;

	cmp = 1;
	key = &LZH_text_buf[r];
	p = N + 1 + key[0];
	LZH_rson[r] = LZH_lson[r] = NIL;
	LZH_match_length = 0;
	for ( ; ; ) {
		if (cmp >= 0) {
			if (LZH_rson[p] != NIL)
				p = LZH_rson[p];
			else {
				LZH_rson[p] = r;
				LZH_dad[r] = p;
				return;
			}
		} else {
			if (LZH_lson[p] != NIL)
				p = LZH_lson[p];
			else {
				LZH_lson[p] = r;
				LZH_dad[r] = p;
				return;
			}
		}
		for (i = 1; i < F; i++)
			if ((cmp = key[i] - LZH_text_buf[p + i]) != 0)
				break;
		if (i > THRESHOLD) {
			if (i > LZH_match_length) {
				LZH_match_position = ((r - p) & (N - 1)) - 1;
				if ((LZH_match_length = i) >= F)
					break;
			}
			if (i == LZH_match_length) {
				if ((c = ((r - p) & (N - 1)) - 1) < LZH_match_position) {
					LZH_match_position = c;
				}
			}
		}
	}
	LZH_dad[r] = LZH_dad[p];
	LZH_lson[r] = LZH_lson[p];
	LZH_rson[r] = LZH_rson[p];
	LZH_dad[LZH_lson[p]] = r;
	LZH_dad[LZH_rson[p]] = r;
	if (LZH_rson[LZH_dad[p]] == p)
		LZH_rson[LZH_dad[p]] = r;
	else
		LZH_lson[LZH_dad[p]] = r;
	LZH_dad[p] = NIL;  /* remove p */
}

/*NH*/
void LzhDeleteNode(p)  /* Deleting node from the tree */
short     p;
{
	short  q;

	if (LZH_dad[p] == NIL)
		return;                 /* unregistered */
	if (LZH_rson[p] == NIL)
		q = LZH_lson[p];
	else
	if (LZH_lson[p] == NIL)
		q = LZH_rson[p];
	else {
		q = LZH_lson[p];
		if (LZH_rson[q] != NIL) {
			do {
				q = LZH_rson[q];
			} while (LZH_rson[q] != NIL);
			LZH_rson[LZH_dad[q]] = LZH_lson[q];
			LZH_dad[LZH_lson[q]] = LZH_dad[q];
			LZH_lson[q] = LZH_lson[p];
			LZH_dad[LZH_lson[p]] = q;
		}
		LZH_rson[q] = LZH_rson[p];
		LZH_dad[LZH_rson[p]] = q;
	}
	LZH_dad[q] = LZH_dad[p];
	if (LZH_rson[LZH_dad[p]] == p)
		LZH_rson[LZH_dad[p]] = q;
	else
		LZH_lson[LZH_dad[p]] = q;
	LZH_dad[p] = NIL;
}

/* Huffman coding parameters */

#define N_CHAR          (256 - THRESHOLD + F)
				/* character code (= 0..N_CHAR-1) */
#define T               (N_CHAR * 2 - 1)        /* Size of table */
#define R               (T - 1)                 /* root position */
#define MAX_FREQ        0x8000
					/* update when cumulative frequency */
					/* reaches to this value */

typedef unsigned char U_char;

/*
 * Tables for encoding/decoding upper 6 bits of
 * sliding dictionary pointer
 */
/* encoder table */
U_char LZH_p_len[64] = {
	0x03, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08
};

U_char LZH_p_code[64] = {
	0x00, 0x20, 0x30, 0x40, 0x50, 0x58, 0x60, 0x68,
	0x70, 0x78, 0x80, 0x88, 0x90, 0x94, 0x98, 0x9C,
	0xA0, 0xA4, 0xA8, 0xAC, 0xB0, 0xB4, 0xB8, 0xBC,
	0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE,
	0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE,
	0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
	0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

/* decoder table */
U_char LZH_d_code[256] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
	0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
	0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D,
	0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F,
	0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11,
	0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13,
	0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15,
	0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17,
	0x18, 0x18, 0x19, 0x19, 0x1A, 0x1A, 0x1B, 0x1B,
	0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1E, 0x1F, 0x1F,
	0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23,
	0x24, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27,
	0x28, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B,
	0x2C, 0x2C, 0x2D, 0x2D, 0x2E, 0x2E, 0x2F, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};

U_char LZH_d_len[256] = {
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};

unsigned short LZH_freq[T + 1];   /* cumulative freq table */

/*
 * pointing parent nodes.
 * area [T..(T + N_CHAR - 1)] are pointers for leaves
 */

short LZH_prnt[T + N_CHAR];

/* pointing children nodes (LZH_son[], LZH_son[] + 1)*/
short LZH_son[T];

unsigned short LZH_getbuf = 0;
U_char LZH_getlen = 0;

/*NH*/
short LzhGetBit()        /* get one bit */
{
	short i;

	while (LZH_getlen <= 8) {
		if ((i = LzhGetChar()) < 0) i = 0;
		LZH_getbuf |= i << (8 - LZH_getlen);
		LZH_getlen += 8;
	}
	i = LZH_getbuf;
	LZH_getbuf <<= 1;
	LZH_getlen--;
	return (i < 0);
}

/*NH*/
short LzhGetByte()       /* get a byte */
{
	unsigned i;

	while (LZH_getlen <= 8) {
		if ((i = LzhGetChar()) < 0) i = 0;
		LZH_getbuf |= i << (8 - LZH_getlen);
		LZH_getlen += 8;
	}
	i = LZH_getbuf;
	LZH_getbuf <<= 8;
	LZH_getlen -= 8;
	return i >> 8;
}

unsigned short LZH_putbuf = 0;
U_char LZH_putlen = 0;

/*NH*/
LzhPutCode(l, c)         /* output c bits */
short         l;
unsigned short    c;
{
    LZH_putbuf |= c >> LZH_putlen;
    if((LZH_putlen += l) >= 8) {
	if(LzhPutChar(LZH_putbuf >> 8)) return(-1);
	if((LZH_putlen -= 8) >= 8) {
	    if(LzhPutChar((LZH_putbuf << 8) >> 8)) return(-1); /* JMP 04-01-94 */
	    LZH_codesize += 2;
	    LZH_putlen -= 8;
	    LZH_putbuf = c << (l - LZH_putlen);
	    }
	else {
	    LZH_putbuf <<= 8;
	    LZH_codesize++;
	    }
	}
    return(0);
}


/*NH*/
/* initialize freq tree */

void LzhStartHuff()
{
	short i, j;

	for (i = 0; i < N_CHAR; i++) {
		LZH_freq[i] = 1;
		LZH_son[i] = i + T;
		LZH_prnt[i + T] = i;
	}
	i = 0; j = N_CHAR;
	while (j <= R) {
		LZH_freq[j] = LZH_freq[i] + LZH_freq[i + 1];
		LZH_son[j] = i;
		LZH_prnt[i] = LZH_prnt[i + 1] = j;
		i += 2; j++;
	}
	LZH_freq[T] = 0xffff;
	LZH_prnt[R] = 0;
}


/*NH*/
/* reconstruct LZH_freq tree */

void LzhReconst()
{
	short i, j, k;
	unsigned short f, l;

	/* halven cumulative LZH_freq for leaf nodes */
	j = 0;
	for (i = 0; i < T; i++) {
		if (LZH_son[i] >= T) {
			LZH_freq[j] = (LZH_freq[i] + 1) / 2;
			LZH_son[j] = LZH_son[i];
			j++;
		}
	}
	/* make a tree : first, connect children nodes */
	for (i = 0, j = N_CHAR; j < T; i += 2, j++) {
		k = i + 1;
		f = LZH_freq[j] = LZH_freq[i] + LZH_freq[k];
		for (k = j - 1; f < LZH_freq[k]; k--);
		k++;
		l = (j - k) * 2;

		/* movmem() is Turbo-C dependent
		   rewritten to memmove() by Kenji */

		/* movmem(&LZH_freq[k], &LZH_freq[k + 1], l); */
		memmove(&LZH_freq[k + 1], &LZH_freq[k], l);
		LZH_freq[k] = f;
		/* movmem(&LZH_son[k], &LZH_son[k + 1], l); */
		memmove(&LZH_son[k + 1], &LZH_son[k], l);
		LZH_son[k] = i;
	}
	/* connect parent nodes */
	for (i = 0; i < T; i++) {
		if ((k = LZH_son[i]) >= T) {
			LZH_prnt[k] = i;
		} else {
			LZH_prnt[k] = LZH_prnt[k + 1] = i;
		}
	}
}


/*NH*/
/* update LZH_freq tree */

void LzhUpdate(c)
short     c;
{
	short i, j, k, l;

	if (LZH_freq[R] == MAX_FREQ) {
		LzhReconst();
	}
	c = LZH_prnt[c + T];
	do {
		k = ++LZH_freq[c];

		/* swap nodes to keep the tree freq-ordered */
		if (k > LZH_freq[l = c + 1]) {
			while (k > LZH_freq[++l]);
			l--;
			LZH_freq[c] = LZH_freq[l];
			LZH_freq[l] = k;

			i = LZH_son[c];
			LZH_prnt[i] = l;
			if (i < T) LZH_prnt[i + 1] = l;

			j = LZH_son[l];
			LZH_son[l] = i;

			LZH_prnt[j] = c;
			if (j < T) LZH_prnt[j + 1] = c;
			LZH_son[c] = j;

			c = l;
		}
	} while ((c = LZH_prnt[c]) != 0);   /* do it until reaching the root */
}

unsigned short LZH_code, LZH_len;

/*NH*/
LzhEncodeChar(c)
unsigned   short  c;
{
	unsigned short i;
	short j, k;

	i = 0;
	j = 0;
	k = LZH_prnt[c + T];

	/* search connections from leaf node to the root */
	do {
		i >>= 1;

		/*
		if node's address is odd, output 1
		else output 0
		*/
		if (k & 1) i += 0x8000;

		j++;
	} while ((k = LZH_prnt[k]) != R);
	if(LzhPutCode(j, i)) return(-1);
	LZH_code = i;
	LZH_len = j;
	LzhUpdate(c);
	return(0);
}

/*NH*/
LzhEncodePosition(c)
unsigned   short  c;
{
	unsigned short i;

	/* output upper 6 bits with encoding */
	i = c >> 6;
	if(LzhPutCode(LZH_p_len[i], (unsigned short)LZH_p_code[i] << 8)) return(-1);

	/* output lower 6 bits directly */
	if(LzhPutCode(6, (c & 0x3f) << 10)) return(-1);

	return(0);
}

/*NH*/
LzhEncodeEnd()
{
    if(LZH_putlen) {
	if(LzhPutChar(LZH_putbuf >> 8)) return(-1);
	LZH_codesize++;
	}
    return(0);
}

/*NH*/
short LzhDecodeChar()
{
	unsigned short c;

	c = LZH_son[R];

	/*
	 * start searching tree from the root to leaves.
	 * choose node #(LZH_son[]) if input bit == 0
	 * else choose #(LZH_son[]+1) (input bit == 1)
	 */
	while (c < T) {
		c += LzhGetBit();
		c = LZH_son[c];
	}
	c -= T;
	LzhUpdate(c);
	return c;
}

/*NH*/
short LzhDecodePosition()
{
	unsigned short i, j, c;

	/* decode upper 6 bits from given table */
	i = LzhGetByte();
	c = (unsigned short )LZH_d_code[i] << 6;
	j = LZH_d_len[i];

	/* input lower 6 bits directly */
	j -= 2;
	while (j--) {
		i = (i << 1) + LzhGetBit();
	}
	return c | i & 0x3f;
}

/*NH*/
/* Compression */

LzhEncode()  /* Encoding/Compressing */
{
	short             i, c, len, r, s, LZH_last_match_length, cnt;
	unsigned long   totsize;
	char            buf[20];

	LZH_textsize = totsize = LzhGetSize();
	sprintf(buf, "%09ld", totsize);
	for(i = 0 ; i < 9; i++) if(LzhPutChar(buf[i])) return(-1);
	if(LZH_textsize == 0) return(0);
	LZH_textsize = 0;                   /* rewind and rescan */
	LzhStartHuff();
	LzhInitTree();
	s = 0;
	r = N - F;
	for (i = s; i < r; i++)
		LZH_text_buf[i] = ' ';
	for (len = 0; len < F && (c = LzhGetChar()) != EOF; len++)
		LZH_text_buf[r + len] = c;
	LZH_textsize = len;
	for (i = 1; i <= F; i++)
		LzhInsertNode(r - i);
	LzhInsertNode(r);
	do {
		if (LZH_match_length > len)
			LZH_match_length = len;
		if (LZH_match_length <= THRESHOLD) {
			LZH_match_length = 1;
			if(LzhEncodeChar((unsigned int)LZH_text_buf[r])) return(-1);
		} else {
			if(LzhEncodeChar((unsigned int)(255 - THRESHOLD + LZH_match_length))) return(-1);
			if(LzhEncodePosition((unsigned int)LZH_match_position)) return(-1);
		}
		LZH_last_match_length = LZH_match_length;
		for (i = 0; i < LZH_last_match_length &&
				(c = LzhGetChar()) != EOF; i++) {
			LzhDeleteNode(s);
			LZH_text_buf[s] = c;
			if (s < F - 1)
				LZH_text_buf[s + N] = c;
			s = (s + 1) & (N - 1);
			r = (r + 1) & (N - 1);
			LzhInsertNode(r);
		}
		LZH_textsize += i;
		if(LzhDisplayFn) {
		    cnt = 100.0 * (double)LZH_textsize / (double) totsize;
		    if(LZH_printcount < cnt) {
			LZH_printcount = cnt;
			LzhDisplayFn(cnt);
			}
		    }

		while (i++ < LZH_last_match_length) {
			LzhDeleteNode(s);
			s = (s + 1) & (N - 1);
			r = (r + 1) & (N - 1);
			if (--len) LzhInsertNode(r);
		}
	} while (len > 0);
	if(LzhEncodeEnd()) return(-1);
	/*printf("input: %ld bytes\n", LZH_textsize);
	printf("output: %ld bytes\n", LZH_codesize);
	printf("output/input: %.3f\n", (double)LZH_codesize / LZH_textsize);
	*/
	return(0);
}

/*NH*/
LzhDecode()  /* Decoding/Uncompressing */
{
	short  i, j, k, r, c, cnt;
	unsigned long int  count, totsize;
	unsigned char buf[20];
/*
#ifndef WATCOM
	extern long     atol();
#endif
/* repl by stdlib.h JMP 04-01-98 */

	for(i = 0 ; i < 9; i++) buf[i] = LzhGetChar();
	buf[9] = 0;
	LZH_textsize = atol(buf);
	totsize = LZH_textsize;
	if(LZH_textsize == 0) return(0);
	LzhStartHuff();
	for (i = 0; i < N - F; i++) LZH_text_buf[i] = ' ';
	r = N - F;
	for (count = 0; count < LZH_textsize; ) {
		c = LzhDecodeChar();
		if (c < 256) {
			if(LzhPutChar(c)) return(-1);
			LZH_text_buf[r++] = c;
			r &= (N - 1);
			count++;
		} else {
			i = (r - LzhDecodePosition() - 1) & (N - 1);
			j = c - 255 + THRESHOLD;
			for (k = 0; k < j; k++) {
				c = LZH_text_buf[(i + k) & (N - 1)];
				if(LzhPutChar(c)) return(-1);
				LZH_text_buf[r++] = c;
				r &= (N - 1);
				count++;
			}
		}
		if(LzhDisplayFn) {
		    cnt = 100.0 * (double)count / (double) totsize;
		    if(cnt > LZH_printcount) {
			LZH_printcount = cnt;
			LzhDisplayFn(cnt);
			}
		    }
	}
/*      printf("%12ld\n", count); */
	return(0);
}



/* ========== Fonctions générales d'interface ========== */

#ifndef SCRCPP
long    (*LZH_sizfn)();
int     (*LZH_putfn)(),
	(*LZH_getfn)();
#else
long    (*LZH_sizfn)();
int     (*LZH_putfn)(int),
	(*LZH_getfn)();
#endif

/*NH*/
LzhPutChar(ch)
int     ch;
{
    return((*LZH_putfn)(ch));
}

/*NH*/
int LzhGetChar()
{
    return((*LZH_getfn)());
}

/*NH*/
long LzhGetSize()
{
    return((*LZH_sizfn)());
}

/*NH*/
void LzhFreeGnl()
{
    if(LZH_text_buf) free(LZH_text_buf);
    if(LZH_lson)     free(LZH_lson);
    if(LZH_rson)     free(LZH_rson);
    if(LZH_dad)      free(LZH_dad);
}

/*NH*/
LzhAllocGnl()
{
    LZH_textsize = LZH_codesize = LZH_printcount = 0;
    LZH_getbuf = 0;
    LZH_getlen = 0;
    LZH_putbuf = 0;
    LZH_putlen = 0;
    LZH_text_buf = (unsigned char *) malloc(N + F - 1);
    LZH_lson     = (short *)           malloc(sizeof(short) * (N + 1));
    LZH_rson     = (short *)           malloc(sizeof(short) * (N + 257));
    LZH_dad      = (short *)           malloc(sizeof(short) * (N + 1));
    if(LZH_text_buf == 0 || LZH_lson == 0 ||
       LZH_rson == 0 || LZH_dad == 0) {
	    LzhFreeGnl();
	    return(-1);
	    }
    return(0);
}

/* ════════════════════════════════════════════════════════════════════
Fonction générale de compactage de données basée sur l'algorithme de
Lempel-Zev et sur la méthode de Huffmann.

Cette fonction prend 3 pointeurs vers des fonctions en argument :

&EN la première fonction (getfn) doit retourner un int qui est le
    caractère suivant de la chaîne à compacter. Elle retourne -1
    pour indiquer la fin de la chaîne.

&EN la deuxième stocke le caractère suivant dans la chaîne compactée
    (putfn)

&EN la troisième retourne la longueur totale de la chaîne à compacter

&RT Le code retour est nul sauf en cas d'erreur

Dans le cas du compactage d'un fichier, les fonctions peuvent être par
exemple définies comme suit :

&CO
    FILE    *LZH_infile, *LZH_outfile;

    LzhGetFile()
    {
	return(getc(LZH_infile));
    }

    LzhPutFile(c)
    int     c;
    {
	return(putc(c, LZH_outfile));
    }

    long LzhSizFile()
    {
	long    pos;

	fseek(LZH_infile, 0L, 2);
	pos = ftell(LZH_infile);
	fseek(LZH_infile, 0L, 0);
	return(pos);
    }

&TX
et l'appel à la fonction d'encodage :
&CO
    LzhEncodeFile(filein, fileout)
    char    *filein, *fileout;
    {
	int     rc;

	LZH_infile = fopen(filein, "rb");
	if(LZH_infile == 0) return(-1);
	LZH_outfile = fopen(fileout, "wb+");
	if(LZH_outfile == 0) {
	    fclose(LZH_infile);
	    return(-1);
	    }

	rc = LzhEncodeGnl(LzhGetFile, LzhPutFile, LzhSizFile);
	fclose(LZH_infile);
	fclose(LZH_outfile);
	return(rc);
    }
&TX
&NO Rien n'empêche donc de ne compacter qu'une partie de fichier ou
    de stocker dans le fichier résultat d'autres données comme le
    nom du fichier en clair. De plus, on peut facilement encrypter
    les données compactées en intervenant au niveau de la fonction
    putfn.

&SA LzhDecodeGnl(), LzhEncodeFile(), LzhEncodeStr()
─────────────────────────────────────────────────────────────────────── */

LzhEncodeGnl(getfn, putfn, sizfn)
int     (*getfn)(), (*putfn)();
long    (*sizfn)();
{
    int     rc;

    LZH_getfn = getfn;
    LZH_putfn = putfn;
    LZH_sizfn = sizfn;
    if(LzhAllocGnl()) return(-1);
    rc = LzhEncode();
    LzhFreeGnl();
    return(rc);
}

/* ════════════════════════════════════════════════════════════════════
Fonction générale de décompactage de données basée sur l'algorithme de
Lempel-Zev et sur la méthode de Huffmann. Les données traitées par cette
fonction sont celles compactées à l'aide de la fonction LzhEncodeGnl().

Cette fonction prend 2 pointeurs vers des fonctions en argument :

&EN la première fonction (getfn) doit retourner un int qui est le
    caractère suivant de la chaîne compactée. Elle retourne -1
    pour indiquer la fin de la chaîne.

&EN la deuxième stocke le caractère suivant dans la chaîne décompactée
    (putfn)

&RT Le code retour est nul sauf en cas d'erreur

Dans le cas du compactage d'un fichier, les fonctions peuvent être par
exemple définies comme pour LzhEncodeGnl(). La fonction de décodage de
fichier est par exemple :

&CO
    LzhDecodeFile(filein, fileout)
    char    *filein, *fileout;
    {
	int     rc;

	LZH_infile = fopen(filein, "rb");
	if(LZH_infile == 0) return(-1);
	LZH_outfile = fopen(fileout, "wb+");
	if(LZH_outfile == 0) {
	    fclose(LZH_infile);
	    return(-1);
	    }

	rc = LzhDecodeGnl(LzhGetFile, LzhPutFile);
	fclose(LZH_infile);
	fclose(LZH_outfile);
	return(rc);
    }
&TX

&SA LzhEncodeGnl(), LzhEncodeFile(), LzhEncodeStr()
─────────────────────────────────────────────────────────────────────── */

LzhDecodeGnl(getfn, putfn)
int     (*getfn)(), (*putfn)();
{
    int     rc;

    LZH_getfn = getfn;
    LZH_putfn = putfn;
    if(LzhAllocGnl()) return(-1);
    rc = LzhDecode();
    LzhFreeGnl();
    return(rc);
}

