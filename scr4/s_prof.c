#include "scr.h"

char    PROF_magic[] = " - Profile 2.0\032";

/*NH*/
SCR_prof_filename(filename, res)
char    *filename, *res;
{
    strcpy(res, filename);
    SCR_strip(res);
    SCR_change_ext(res, res, "prf");
    return(0);
}

/* ======================================================================
Fonction de sauvetage de profil et d'un mot de passe éventuel. Le fichier
filename contiendra les valeurs des champs de toutes les pages et les
options courantes de tous les menus dont la définition inclus le mot-clé
PROFILE. Il contiendra en plus le mot de passe passwd si celui-ci n'est pas
nul.

Cette fonction peut être lancée à la fin du programme ou à n'importe quel
moment dans le cours d'une application.

Lors de la création de ce fichier, toutes les pages sont successivement
sauvées. Les options courantes des menus sont également sauvées. De plus, le
numéro de version de SCR/AL1 est indiqué dans le fichier.

Le fichier de profile a toujours l'extension .prf. Celle-ci est forcée, même
si une autre extension est donnée dans le paramètre filename.

&EX
    main()
    {
	SCR_init();
	SCR_prof_load_pwd("myprof", PASSWD);
	...

	SCR_prof_save_pwd("myprof", PASSWD);
	SCR_end();
    }
&TX
&RT -1 si le fichier ne peut être créé, 0 sinon
&SA SCR_prof_save(), SCR_prof_load_pwd(), SCR_prof_load()
------------------------------------------------------------------------- */

SCR_prof_save_pwd(filename, passwd) /* JMP38 13-09-92 */
char    *filename, *passwd;
{
    char        buf[256]; // JMP 16/06/2016
    FILE        *fd;
    FILE_TYPE   *flt;
    int         i;

    SCR_prof_filename(filename, buf);
#ifdef DOS
    fd = fopen(buf, "wb+");
#else
    fd = fopen(buf, "w+");
#endif
    if(fd == 0) return(-1);

    SCR_write_magic(fd, SCR_magic);
    SCR_write_magic(fd, PROF_magic);
    SCR_prof_wpwd(fd, passwd);
    for(i = 1 ; i < SCR_READ_FILES.tbl_nb_fl ; i++) {
	flt = SCR_READ_FILES.tbl_fl[i];
	switch(TYPE16(flt->fl_type)) {
	    case SCR_TYPE_PAGE :
		SCR_prof_wpage(fd, flt);
		break;
	    case SCR_TYPE_MENU:
		SCR_prof_wmenu(fd, flt);
		break;
	    default : break;
	    }
	}


    fclose(fd);
    return(0);
}

/* ======================================================================
Fonction de sauvetage de profil. Le fichier filename contiendra les valeurs
des champs de toutes les pages et les options courantes de tous les menus
dont la définition inclus le mot-clé PROFILE.

Cette fonction peut être lancée à la fin du programme ou à n'importe quel
moment dans le cours d'une application.

Lors de la création de ce fichier, toutes les pages sont successivement
sauvées. Les options courantes des menus sont également sauvées. De plus, le
numéro de version de SCR/AL1 est indiqué dans le fichier.

Le fichier de profile a toujours l'extension .prf. Celle-ci est forcée, même
si une autre extension est donnée dans le paramètre filename.

&EX
    main()
    {
	SCR_init();
	SCR_prof_load("myprof");
	...

	SCR_prof_save("myprof");
	SCR_end();
    }
&TX
&RT -1 si le fichier ne peut être créé, 0 sinon
&SA SCR_prof_load(), SCR_prof_load_pwd(), SCR_prof_save_pwd()
------------------------------------------------------------------------- */

SCR_prof_save(filename)
char    *filename;
{
    return(SCR_prof_save_pwd(filename, 0L));
}

/*NH*/
SCR_prof_wpage(fd, flt)
FILE        *fd;
FILE_TYPE   *flt;
{
    PAGE    *pg = (PAGE *) flt->fl_ptr;
    short   lg, i, type = SCR_TYPE_PAGE;
    char    **ptr;

    if(pg->pg_prof == 0) return(0);
    if(flt->fl_name == 0) return(0);

    /* 1. Write Name & TYPE */
    SCR_DumpString(fd, flt->fl_name);
    SCR_DumpShort(fd, SCR_TYPE_PAGE);

    /* 2. Calc total fields LG */
    lg = pg->pg_rec_len;
    PG_read_flds(pg);
    for(i = 0 ; i < pg->pg_nb_flds ; i++)
	if(pg->pg_flds[i].fld_type == SCR_EDITOR) {
	    ptr = (char **)PG_get_text(pg, i);
	    lg += sizeof(short);
	    if(ptr[0]) lg += strlen(ptr[0]);
	    }

    /* 3. Write nb_flds, rec_len, tot_lg & pg_rec */
    SCR_DumpShort(fd, pg->pg_nb_flds);
    SCR_DumpShort(fd, pg->pg_rec_len);
    SCR_DumpShort(fd, lg);
    fwrite(pg->pg_rec, pg->pg_rec_len, 1, fd);

    /* 4. Write editor fields */
    for(i = 0 ; i < pg->pg_nb_flds ; i++)
	if(pg->pg_flds[i].fld_type == SCR_EDITOR) {
	    ptr = (char **)PG_get_text(pg, i);
	    SCR_DumpString(fd, ptr[0]);
	    }

    PG_fld_free(pg);
    return(0);
}

/*NH*/
SCR_prof_wmenu(fd, flt)
FILE        *fd;
FILE_TYPE   *flt;
{
    MENU    *mn = (MENU *)flt->fl_ptr;

    if(mn->mn_prof == 0) return(0);
    if(flt->fl_name == 0) return(0);
    SCR_DumpString(fd, flt->fl_name);
    SCR_DumpShort(fd, SCR_TYPE_MENU);
    SCR_DumpShort(fd, mn->mn_choice);
    return(0);
}

/*NH*/
SCR_prof_wpwd(fd, passwd)
FILE    *fd;
char    *passwd;
{
    short   i;

    if(passwd == 0) {
	SCR_DumpShort(fd, 0);
	return(0);
	}
    SCR_DumpShort(fd, SCR_PASSWD);
    i = strlen(passwd);
    SCR_crypt(passwd, i, 2L);
    fwrite(&i, sizeof(short), 1, fd);
    fwrite(passwd, i, 1, fd);
    SCR_decrypt(passwd, i, 2L);
    return(0);
}

/* ======================================================================
Tente d'ouvrir le fichier de profile filename. L'extension .prf est toujours
forcée. Cérifie également le numéro de version SCR/AL1 et la magic number
d'un fichier .prf.

&EX
    CheckProf(filename)
    char    *filename;
    {
	FILE    *fd;

	fd = SCR_prof_open(filename);
	if(fd == 0) return(-1);
	fclose(fd);
	return(0);
    }
&TX
&RT un FILE * vers le fichier ouvert ou (FILE *)0 si le fichier ne
    peut être ouvert ou est d'un type incorrect.
&SA SCR_prof_load(), SCR_prof_save()
------------------------------------------------------------------------- */

#ifdef SCRCPP
FILE *SCR_prof_open(
char    *filename
)
#else
FILE *SCR_prof_open(filename)
char    *filename;
#endif
{
    char    buf[256]; // JMP 16/06/2016
    FILE    *fd;

    SCR_prof_filename(filename, buf);
#ifdef DOS
    fd = fopen(buf, "rb");
#else
    fd = fopen(buf, "r");
#endif
    if(fd == 0) return(fd);
    if(SCR_check_magic(fd, SCR_magic) || SCR_check_magic(fd, PROF_magic)) {
	fclose(fd);
	return((FILE *)0);
	}
    return(fd);
}

/* ======================================================================
Fonction de chargement de profil. Le fichier filename, construit à l'aide de
la fonction SCR_prof_filename() contient les valeurs des champs de toutes
les pages et les options courantes de tous les menus dont la définition
inclus le mot-clé PROFILE. Il contient en plus un mot de passe.

Normalement, le fichier est le résultat de la fonction SCR_prof_save_pwd().

Cette fonction peut être lancée après l'appel à SCR_init() dans le programme
principal d'une application. Elle peut également être utilisée à n'importe
quel moment dans le cours d'une application.

Lors de l'interprétation de ce fichier, toutes les pages sont successivement
lues et analysées. Si la longueur totale des champs ne correspond pas à la
longueur trouvée dans le fichier, la lecture du profile s'arrête et les
pages et les menus qui suivent ne sont pas chargés. De plus, le numéro de
version de SCR/AL1 doit correspondre à celui encrypté dans le programme.
Cette statégie est mise en place pour éviter les "plantages" dus aux
modifications d'un programme.

Le fichier de profile a toujours l'extension .prf. Celle-ci est forcée, même
si une autre extension est donnée dans le paramètre filename.

&EX
    main()
    {
	SCR_init();
	SCR_prof_load_pwd("myprof", PASSWD);
	...

	SCR_prof_save_pwd("myprof", PASSWD);
	SCR_end();
    }
&TX
&RT -1 si le fichier n'est pas trouvé ou que la version SCR/AL1 est
    incorrecte, 0 si le fichier peut être interprété. Si toutes les pages ne
    sont pas chargées, le code retour est 0.

&SA SCR_prof_save_pwd(), SCR_prof_save(), SCR_prof_load()
------------------------------------------------------------------------- */
SCR_prof_load_pwd(filename, passwd) /* JMP38 13-09-92 */
char    *filename, *passwd;
{
    FILE        *fd;
    short       i, type, nb_flds, rec_len, tot_lg, opt;
    FILE_TYPE   *flt;
    char        name[128];
    U_ch        **ptr;
    PAGE        *pg;
    MENU        *mn;

    fd = SCR_prof_open(filename);
    if(fd == 0) return(-1);

    SCR_prof_rpwd(fd, passwd);

    while(1) {
	if(SCR_LoadSString(fd, name) < 0) break;
	if(SCR_LoadShort(fd, &type) < 0) break;
	if(type == SCR_TYPE_PAGE) {
	    if(SCR_LoadShort(fd, &nb_flds) < 0) break;
	    if(SCR_LoadShort(fd, &rec_len) < 0) break;
	    if(SCR_LoadShort(fd, &tot_lg) < 0) break;
	    }
	else {
	    if(SCR_LoadShort(fd, &opt) < 0) break;
	    }

	flt = SCR_find_flt(name);
	if(type == SCR_TYPE_PAGE) {
	    if(flt == 0 || flt->fl_type != TYPE5(type)) {
		fseek(fd, (long)tot_lg, SEEK_CUR);
		continue;
		}

	    pg = (PAGE *)flt->fl_ptr;
	    if(nb_flds != pg->pg_nb_flds || rec_len != pg->pg_rec_len) {
		fseek(fd, (long)tot_lg, SEEK_CUR);
		continue;
		}

	    PG_read_flds(pg);
	    PG_free_editor(pg);
	    fread(pg->pg_rec, rec_len, 1, fd);
	    for(i = 0 ; i < pg->pg_nb_flds ; i++) {
		if(pg->pg_flds[i].fld_type == SCR_EDITOR) {
		    ptr = (U_ch **)PG_get_text(pg, i);
		    ptr[0] = ptr[1] = 0;
		    SCR_LoadString(fd, &(ptr[0]));
		    }
		}

	    PG_fld_free(pg);
	    }

	else {
	    if(flt == 0 || flt->fl_type != TYPE5(type)) continue;
	    mn = (MENU *)flt->fl_ptr;
	    mn->mn_choice = opt;
	    }
	}

fin:
    fclose(fd);
    return(0);
}

/* ======================================================================
Fonction de chargement de profil. Le fichier filename, construit à l'aide de
la fonction SCR_prof_save() contient les valeurs des champs de toutes les
pages et les options courantes de tous les menus dont la définition inclus
le mot-clé PROFILE.

Cette fonction peut être lancée après l'appel à SCR_init() dans le programme
principal d'une application. Elle peut également être utilisée à n'importe
quel moment dans le cours d'une application.

Lors de l'interprétation de ce fichier, toutes les pages sont successivement
lues et analysées. Si la longueur totale des champs ne correspond pas à la
longueur trouvée dans le fichier, la lecture du profile s'arrête et les
pages et les menus qui suivent ne sont pas chargés. De plus, le numéro de
version de SCR/AL1 doit correspondre à celui encrypté dans le programme.
Cette statégie est mise en place pour éviter les "plantages" dus aux
modifications d'un programme.

Le fichier de profile a toujours l'extension .prf. Celle-ci est forcée, même
si une autre extension est donnée dans le paramètre filename.

&EX
    main()
    {
	SCR_init();
	SCR_prof_load("myprof");
	...

	SCR_prof_save("myprof");
	SCR_end();
    }
&TX
&RT -1 si le fichier n'est pas trouvé ou que la version SCR/AL1 est
    incorrecte, 0 si le fichier peut être interprété. Si toutes les pages ne
    sont pas chargées, le code retour est 0.

&SA SCR_prof_save(), SCR_prof_load_pwd(), SCR_prof_save_pwd()
------------------------------------------------------------------------- */
SCR_prof_load(filename)
char    *filename;
{
    return(SCR_prof_load_pwd(filename, 0L));
}

/*NH*/
SCR_prof_rpwd(fd, passwd)
FILE    *fd;
char    *passwd;
{
    short   i;
    char    buf[256]; // JMP 16/06/2016

    SCR_LoadShort(fd, &i);
    if(i != SCR_PASSWD) return(0);
    if(passwd == 0) passwd = buf;
    passwd[0] = 0; /* JMP 15-11-07 */

    if(fread(&i, sizeof(short), 1, fd) != 1) return(-1);
    if(i < 0) return(0);
    if(fread(passwd, i, 1, fd) != 1) return(-1);
    SCR_decrypt(passwd, i, 2L);
    return(0);
}


/* ======================================================================
Remet les valeurs par défaut dans toutes les PAGES et les MENUS contenant le
mot-clé PROFILE.
&SA SCR_prof_load(), SCR_prof_save()
------------------------------------------------------------------------- */
SCR_prof_reset()
{
    int         i;
    FILE_TYPE   *flt;
    PAGE        *pg;
    MENU        *mn;

    for(i = 1 ; i < SCR_READ_FILES.tbl_nb_fl ; i++) {
	flt = SCR_READ_FILES.tbl_fl[i];
	switch(TYPE16(flt->fl_type)) {
	    case SCR_TYPE_PAGE :
		pg = (PAGE *)flt->fl_ptr;
		if(pg->pg_prof) {
		    PG_reset(pg);
		    PG_fld_free(pg);
		    }
		break;

	    case SCR_TYPE_MENU:
		mn = (MENU *)flt->fl_ptr;
		if(mn->mn_prof) mn->mn_choice = 0;
		break;

	    default : break;
	    }
	}
    return(0);
}


