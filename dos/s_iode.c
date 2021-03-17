/**
 * Fonctions annulées ou a remplacer en fonctoin de la cible  
 *
 *
 * void kwrmsg(char *msg)          : affiche un message dans la "status bar" (non bloquant)
 * int kclmsg(int key, int status) : vide le message de la status bar (key et status sont ignorés)
 * kerror(level, st1, st2, st3, st4, st5, st6, st7, st8, st9, st10) : affiche un msg d'erreur (bloquant)
 * kmsg(st1, st2, st3, st4, st5, st6, st7, st8, st9, st10) : construit et affichje un message dans la status bar
 */
 
#include <scr4.h>
#include "o_objs.h"

// k_error.c
int     K_SCR = 0, K_MSG = 0;

void kwrmsg(msg)        
char    *msg;
{
    SCR_pad(msg, 48);
    SCR_set_attr(SCR_REVERSE);
    SCR_write_text(SCR_PAGE_SIZE[0] - 1, 0, msg, 48); 
    SCR_wdispatch();  
}

int kclmsg(int key, int status)
{
	extern char	*ODE_keylogfile;
	char    buf[80], *oODE_keylogfile;
	static	FILE *fd = 0;
	int		alt, ctrl, shift, type, base;
	
    if(K_MSG && K_SCR)  {
		buf[0] = K_MSG = 0;
		kwrmsg(buf);
    }
	
	if(ODE_keylogfile == 0) return(0);
	if(fd == 0) {
		fd = fopen(ODE_keylogfile, "w+");
		if(fd == 0) {
			DebugForce("Cannot open file '%s'\n", ODE_keylogfile);
			ODE_keylogfile = 0; // To avoid trying continuously 
			return(0);
		}
	}
	
	if(key >= SCR_A_A  && key <= SCR_A_9) 			fprintf(fd, "&a%c",   'a' + key - SCR_A_A);
	else if(key >= SCR_C_A && key <= SCR_C_9)		fprintf(fd, "&c%c",   'a' + key - SCR_C_A);
	
	else if(key >= SCR_F1    && key <=  SCR_F9) 	fprintf(fd, "@F%d", 1 + key - SCR_F1);
	else if(key >= SCR_F10   && key <= SCR_F20) 	fprintf(fd, "@F%c", 'A' + key - SCR_F10);
	else if(key >= SCR_A_F1  && key <= SCR_A_F9) 	fprintf(fd, "&a@F%d", 1 + key - SCR_A_F1);
	else if(key >= SCR_A_F10 && key <= SCR_A_F20) 	fprintf(fd, "&a@F%c", 'A' + key - SCR_A_F10);
	else if(key >= SCR_C_F1  && key <= SCR_C_F9) 	fprintf(fd, "&c@F%d", 1 + key - SCR_C_F1);
	else if(key >= SCR_C_F10 && key <= SCR_C_F20) 	fprintf(fd, "&c@F%c", 'A' + key - SCR_C_F10);
	else if(key >= SCR_S_F1  && key <= SCR_S_F9) 	fprintf(fd, "&s@F%d", 1 + key - SCR_S_F1);
	else if(key > SCR_S_F10 && key <= SCR_S_F20) 	fprintf(fd, "&s@F%c", 'A' + key - SCR_S_F10); // !! S_F10 réservé pour les messages 
	

	else if(key >= SCR_ARROW && key < SCR_ARROW + 999) {
		if((key >= SCR_ARROW + 100 && key < SCR_ARROW + 200) ||
		   (key >= SCR_ARROW + 300 && key < SCR_ARROW + 400))
			fprintf(fd, "&s");
		if(key >= SCR_ARROW + 200 && key < SCR_ARROW + 400) 
			fprintf(fd, "&c");
		
		switch(key) {
			case SCR_CSR_LEFT    : case SCR_C_CSR_LEFT    : case SCR_S_CSR_LEFT    : case SCR_C_S_CSR_LEFT    : fprintf(fd, "@l"); break;
			case SCR_CSR_RIGHT   : case SCR_C_CSR_RIGHT   : case SCR_S_CSR_RIGHT   : case SCR_C_S_CSR_RIGHT   : fprintf(fd, "@r"); break;
			case SCR_CSR_UP      : case SCR_C_CSR_UP      : case SCR_S_CSR_UP      : case SCR_C_S_CSR_UP      : fprintf(fd, "@u"); break;
			case SCR_CSR_DOWN    : case SCR_C_CSR_DOWN    : case SCR_S_CSR_DOWN    : case SCR_C_S_CSR_DOWN    : fprintf(fd, "@d"); break;
			case SCR_CSR_PG_UP   : case SCR_C_CSR_PG_UP   : case SCR_S_CSR_PG_UP   : case SCR_C_S_CSR_PG_UP   : fprintf(fd, "@p"); break;
			case SCR_CSR_PG_DN   : case SCR_C_CSR_PG_DN   : case SCR_S_CSR_PG_DN   : case SCR_C_S_CSR_PG_DN   : fprintf(fd, "@n"); break;
			case SCR_CSR_HOME    : case SCR_C_CSR_HOME    : case SCR_S_CSR_HOME    : case SCR_C_S_CSR_HOME    : fprintf(fd, "@h"); break;
			case SCR_CSR_END     : case SCR_C_CSR_END     : case SCR_S_CSR_END     : case SCR_C_S_CSR_END     : fprintf(fd, "@e"); break;
		}	
	}
	else switch(key) {
			case SCR_INSERT      : fprintf(fd, "@I"); break;
			case SCR_DELETE	     : fprintf(fd, "@D"); break;
			case SCR_ENTER 	     : fprintf(fd, "@R"); break;
			case SCR_ESCAPE	     : fprintf(fd, "@E"); break;
			case SCR_BACKSPACE 	 : fprintf(fd, "@B"); break;
			case SCR_TAB         : fprintf(fd, "@T"); break;
			case SCR_S_TAB       : fprintf(fd, "&s@T"); break;
			case SCR_C_TAB       : fprintf(fd, "&c@T"); break;
			case SCR_C_S_TAB     : fprintf(fd, "&c&s@T"); break;
			case SCR_A_TAB 		 : fprintf(fd, "&a@T"); break;
			case SCR_A_S_TAB 	 : fprintf(fd, "&a&s@T"); break;
			
			case SCR_S_F10   	 : 
				oODE_keylogfile = ODE_keylogfile;
				ODE_keylogfile = 0;
				PG_edit_window(p_playback_cmt);
				ODE_keylogfile = oODE_keylogfile;
				fprintf(fd, "\n# %s\n", SCR_strip(p_playback_cmt_CMT)); 
				break;
			
			case 0				 : break;
			default 			 : 
				if(key < 256) fprintf(fd, "%c", key); 
				else 		  fprintf(fd, "!!!!%d!!!!", key); 
				break;
	}
	fflush(fd);
    return(0);
}

// k_errorv

#define _IODEVARG_

kerror(level, st1, st2, st3, st4, st5, st6, st7, st8, st9, st10)
int     level;
char    *st1, *st2, *st3, *st4, *st5, *st6, *st7, *st8, *st9, *st10;
{
    char    buf[10256];

    sprintf(buf, st1, st2, st3, st4, st5, st6, st7, st8, st9, st10);

    if (K_SCR == 0)  	fprintf(stderr, "%-79.79s\n", buf);
    else       			PG_display_error(buf);

    if(level == 1) {
		if(K_SCR != 0) SCR_end();
		exit(1);
    }
}

kmsg(st1, st2, st3, st4, st5, st6, st7, st8, st9, st10)
char    *st1, *st2, *st3, *st4, *st5, *st6, *st7, *st8, *st9, *st10;
{
    char    buf[10256];

    if(K_MSG == 1) kclmsg(0, 0);

    K_MSG = 1;
    sprintf(buf, st1, st2, st3, st4, st5, st6, st7, st8, st9, st10);

    if(K_SCR == 0)  fprintf(stderr, "%-79.79s\r", buf);
    else {
	kwrmsg(buf);
//      AddDebug(buf);
    }

}
