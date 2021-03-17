#include "iode.h"

extern char    K_LABEL[];
extern char    K_LABELS[]; /* JMP 09-04-98 */
extern char    K_LABELD[]; /* JMP 09-04-98 */
extern char    K_LABELX[];

int     K_WARN_DUP = 0;
int     K_LZH = 0;

/*NH*/
void K_32to64(void *ptr1, void *ptr2, int lg)
{
	memcpy(ptr1, ptr2, lg);
}

/*NH*/
int K_strcmp(const void *p1, const void *p2)
{
    KOBJ    *ko1 = (KOBJ *)p1, *ko2 = (KOBJ *)p2;

    if(ko1 == 0) return(-1);
    if(ko2 == 0) return(1);
    return(strcmp(ko1->o_name, ko2->o_name));
}

int K_find_strcmp(const void *name, const void *kobjs)
{
    KOBJ    *ko2 = (KOBJ *)kobjs;
    return(strcmp((char *)name, ko2->o_name));
}

/*NH*/
void K_sort(kdb)
KDB     *kdb;
{
    qsort(KOBJS(kdb), (int) KNB(kdb), sizeof(KOBJ), K_strcmp);
}
/*
    Creates a DB with a given type and mode.
    Returns a pointer to that DB or NULL if anything goes wrong.
*/
KDB *K_create(type, mode)
int     type, mode;
{
    KDB   *kdb;

    kdb = (KDB *) SW_nalloc(sizeof(KDB));
    if(kdb == NULL) return(NULL);
    strcpy(KARCH(kdb), ARCH);
    KMODE(kdb) = mode;
    KTYPE(kdb) = type;
    KNAMEPTR(kdb) = SCR_stracpy("ws"); // JMP 29/9/2015
    return(kdb);
}

/*
    Frees the DB and its contents
	***TODO*** Utiliser K_clear
*/
K_free(kdb)
KDB   *kdb;
{
    int i;

    if(kdb == NULL) return(0);
    for(i = 0; i < KNB(kdb); i++)
	if(KOBJS(kdb)[i].o_val != 0) SW_free(KOBJS(kdb)[i].o_val);

    SCR_free(KNAMEPTR(kdb)); // JMP 3/6/2015
    KNAMEPTR(kdb) = 0;              // JMP 3/6/2015
    K_free_kdb(kdb);
    return(0);
}

/*
	Clears the contents of a DB. Keeps the structure
*/
K_clear(kdb)
KDB   *kdb;
{
    int i;

    for(i = 0; i < KNB(kdb); i++) SW_free(KOBJS(kdb)[i].o_val);
    SW_nfree(KOBJS(kdb));
    KOBJS(kdb) = NULL;
    KNB(kdb) = 0;
    SCR_free(KNAMEPTR(kdb)); // JMP 3/6/2015
    KNAMEPTR(kdb) = 0;              // JMP 3/6/2015
    memset(KSMPL(kdb), 0, sizeof(SAMPLE)); /* JMP 28-03-92 */
    KNAMEPTR(kdb) = SCR_stracpy("ws"); // JMP 29/9/2015
    return(0);
}

/*
    Frees the DB and leaves its contents untouched
*/
int K_free_kdb(kdb)
KDB   *kdb;
{
    SW_nfree(KOBJS(kdb));
    SW_nfree(kdb);
    return(0);
}

/*
	Duplicates object name1 of kdb1 and names it name2 kdb2
	It returns the pos of name2 in the DB if all goes well.
	It returns -1 if object name1 does not exist,
	and -2 if name2 cannot be added to the DB.(kdb2)
*/
K_dup(kdb1, name1, kdb2, name2)
KDB     *kdb1, *kdb2;
char    *name1, *name2;
{
    int     pos1, pos2, lg;
    char    *pack, *ptr;

    pos1 = K_find(kdb1, name1);
    if(pos1 < 0 || !strcmp(name1, name2)) return(-1);

    pos2 = K_find(kdb2, name2);
    if(pos2 >= 0) {
	if(KSOVAL(kdb2, pos2) != 0) SW_free(KSOVAL(kdb2, pos2));
    }
    else {
	pos2 = K_add_entry(kdb2, name2);
	pos1 = K_find(kdb1, name1);
    }

    if(pos2 < 0) return(-2);

    pack = KGOVAL(kdb1, pos1);
    lg = * (OSIZE *) pack;
    ptr = SW_nalloc(lg);
    memcpy(ptr, pack, lg);

    KSOVAL(kdb2, pos2) = SW_alloc(lg);
    memcpy(KGOVAL(kdb2, pos2), ptr, lg);

    SW_nfree(ptr);
    return(pos2);
}

/*
    Renames an object name1 and gives it the name2.

    Returns -1 if name1 does not exist, -2 if name2 already exists,
    -3 if name2 cannot be added or returns the position of name2
    in the DB.
*/

K_ren(kdb, name1, name2)
KDB     *kdb;
char    *name1, *name2;
{
    int     pos1, pos2, lg;
    char    *pack;

    if(kdb == NULL) return(-1);
    pos1 = K_find(kdb, name1);
    if(pos1 < 0) return(-1);

    pos2 = K_find(kdb, name2);
    if(pos2 >= 0) return(-2);

    pos2 = K_add_entry(kdb, name2);
    if(pos2 < 0) return(-3);
    pos1 = K_find(kdb, name1); /* object name1 may have changed after add name2 */

    KSOVAL(kdb, pos2) = KSOVAL(kdb, pos1);
    K_del_entry(kdb, pos1);
    return(pos2);
}

long add_entry_cnt = 0;
/*
    Adds an entry in the database with a given name
    returns -1 if anything goes wrong
    or the position in the DB if succeeded
    No data is added !!!!!!!!!!!  (data pointer is set to 0)
*/
K_add_entry(kdb, name)
KDB     *kdb;
char    *name;
{

    int     i, lg,
	    pos, maxpos, minpos, nbobjs;
    KOBJ    *ktmp;

    if(kdb == NULL) return(-1);
    if(K_key(name, KMODE(kdb)) < 0) return(-1);
    pos = K_find(kdb, name);
    if(pos >= 0) {
	if(K_WARN_DUP)
	    kerror(0, "%s defined more than once", name);
	return(pos);
    }

    if((KNB(kdb)) % K_CHUNCK == 0)
	//KOBJS(kdb) = (char *) SW_nrealloc(KOBJS(kdb),
	KOBJS(kdb) = SW_nrealloc(KOBJS(kdb),
					  (unsigned int)(sizeof(KOBJ) * KNB(kdb)),
					  (unsigned int)(sizeof(KOBJ) * (KNB(kdb) + K_CHUNCK)));
    if(KOBJS(kdb) == 0) return(-1);


    if(KNB(kdb) == 0) { /* JMP 09-06-00 */
	maxpos = 0;
	goto done;
    }

    if(K_find_strcmp(name, KOBJS(kdb) + KNB(kdb) - 1) > 0) {
	maxpos = KNB(kdb);
	goto done;
    }

    if(K_find_strcmp(name, KOBJS(kdb)) < 0) {
	/* add before */
	maxpos = 0;
	goto done;
    }

    /* insert */
    maxpos = KNB(kdb);
    minpos = 0;
    while(maxpos - minpos > 1) {
	pos = minpos + (maxpos - minpos)/2;
	if(K_find_strcmp(name, KOBJS(kdb) + pos) < 0) maxpos = pos;
	else minpos = pos;
    }

done :
    /*
    for(i = KNB(kdb); i > maxpos; i--) {
    memcpy(KOBJS(kdb) + i, KOBJS(kdb) + i - 1, sizeof(KOBJ));
    add_entry_cnt++;
    }
    */
    nbobjs =  KNB(kdb) - maxpos;
    if(nbobjs != 0) {
	ktmp = (KOBJ *) BUF_alloc(nbobjs * sizeof(KOBJ));
	memcpy((char *)ktmp, (char *)(KOBJS(kdb) + maxpos), nbobjs * sizeof(KOBJ));
	memcpy((char *)(KOBJS(kdb) + maxpos + 1), (char *)ktmp, nbobjs * sizeof(KOBJ));
	//SCR_free(ktmp);
    }

    lg = min(strlen(name), K_MAX_NAME);
    memcpy(KONAME(kdb, maxpos), name, lg + 1);
    KSOVAL(kdb, maxpos) = 0;

    KNB(kdb)++;
    /*
    K_sort(kdb);
    return(K_find(kdb, name));
    */

    return(maxpos);
}

/*
    Finds an object name in the DB.
    Returns -1 if DB is empty or name is unknown in the DB
    or the position in the DB.
*/
K_find(kdb, name)
KDB     *kdb;
char    *name;
{
    char    *res;
    ONAME   oname;

    if(kdb == NULL || KNB(kdb) == 0) return(-1);

    SCR_strlcpy(oname, name, K_MAX_NAME);  /* JMP 13-02-2013 */
    if(K_key(oname, KMODE(kdb)) < 0) return(-1);

    res = (char *) bsearch(oname, KOBJS(kdb), (int) KNB(kdb),
			   sizeof(KOBJ), K_find_strcmp);
    if(res != 0) return((res - (char *) KOBJS(kdb)) / sizeof(KOBJ));
    else return(-1);
}

/*
    Deletes an object (and its data) from the DB
    returns 0 in case of success, -1 if DB is empty or
    element is unknown.

PS:K_del(kdb, K_find(kdb, name)) deletes an object with a given name
*/
K_del(kdb, nb)
KDB     *kdb;
int     nb;
{
    if(kdb == NULL) return(-1);
    if(nb < 0 || nb >= KNB(kdb)) return(-1);
    SW_free(KSOVAL(kdb, nb));
    K_del_entry(kdb, nb);
    return(0);
}

/*NH*/
/* PRIVATE
    deletes an object (without its data) from the DB

PS  only there for internal use, USE K_del instead
*/
K_del_entry(kdb, nb)
KDB     *kdb;
int     nb;
{
    memcpy(KOBJS(kdb) + nb, KOBJS(kdb) + (nb + 1),
	   (int)(KNB(kdb) - nb - 1) * sizeof(KOBJ));
    KNB(kdb)--;
    if(KNB(kdb) > 0) {
	memset(KOBJS(kdb) + (int) KNB(kdb), 0, sizeof(KOBJ));
	if(KNB(kdb) % K_CHUNCK == 0)
	    KOBJS(kdb) = (KOBJ *) SW_nrealloc((char *)KOBJS(kdb),
					      (unsigned int)(sizeof(KOBJ) * (KNB(kdb) + K_CHUNCK)),
					      (unsigned int)(sizeof(KOBJ) * KNB(kdb)));
    }
    else {
	SW_nfree(KOBJS(kdb));
	KOBJS(kdb) = NULL;
    }
    return(0);
}

/*
    Deletes from kdb all the objects which are not present in names.
*/
K_select(kdb, nb, names)
KDB     *kdb;
int     nb;
char    *names[];
{
    int     i, j;

    if(kdb == NULL) return(-1);
    for(i = 0 ; i < KNB(kdb); i++) {
	for(j = 0; j < nb; j++)
	    if(strcmp(names[j], KONAME(kdb, i)) == 0) break;
	if(j == nb) {
	    K_del(kdb, i);
	    i--;
	}
    }

    for(j = 0; j < nb; j++)
	if(K_find(kdb, names[j]) < 0) {
	    B_seterrn(98, names[j]);
	    return(-1);
	}

    return(0);
}

/*
    Refers to the variables (names) from kdb in a returned DB.
    names is a null terminated list.

    The data stored in the objects is not copied only the pointer
    is set !!!! So a K_free of the returned kdb destroys the
    contense of the refernce kdb
*/
KDB *K_refer(kdb, nb, names)
KDB     *kdb;
int     nb;
char    *names[];
{
    int     i, pos1, pos2, err = 0;
    KDB     *tkdb;

    if(kdb == NULL) return(NULL);
    tkdb = K_create(KTYPE(kdb), KMODE(kdb));
    memcpy(KDATA(tkdb), KDATA(kdb), K_MAX_DESC);

    for(i = 0 ; i < nb && names[i]; i++) {
	pos2 = K_find(kdb, names[i]);
	if(pos2 < 0)  {
	    B_seterrn(98, names[i]);
	    err = 1;
	    continue;
	}

	pos1 = K_add_entry(tkdb, names[i]);
	if(pos1 < 0) {
	    B_seterrn(99, names[i]);
	    K_free_kdb(tkdb);
	    tkdb = NULL;
	    err = 1;
	    break;
	}
	KSOVAL(tkdb, pos1) = KSOVAL(kdb, pos2);
    }
    if(err) B_display_last_error();
    return(tkdb);
}

// Version plus rapide de K_refer
// JMP 16/3/2012

KDB *K_quick_refer(kdb, names)
KDB     *kdb;
char    *names[];
{
    int     i, pos, nb = SCR_tbl_size(names);
    KDB     *tkdb;

    if(kdb == NULL) return(NULL);

    // Crée la nouvelle kdb avec le nombre exact d'entrées
    tkdb = K_create(KTYPE(kdb), KMODE(kdb));
    memcpy(KDATA(tkdb), KDATA(kdb), K_MAX_DESC);
    KOBJS(tkdb) = (KOBJ *) SW_nalloc(sizeof(KOBJ) * K_CHUNCK * (1 + nb / K_CHUNCK));
    KNB(tkdb) = nb;

    // Copie les entrées dans tkdb
    for(i = 0 ; i < nb; i++) {
	pos = K_find(kdb, names[i]);
	if(pos < 0) {
	    K_free_kdb(tkdb);
	    return(NULL);

	}
	memcpy(KOBJS(tkdb) + i, KOBJS(kdb) + pos, sizeof(KOBJ));
    }

    // Sort tkdb
    K_sort(tkdb);

    return(tkdb);
}

/*
    Merges two databases : kdb1 <- kdb1 + kdb2
    If replace == 0, existing elements in kdb1 are not overwritten.
    If replace == 1, the values of existing elements are replaced.

    It leaves kdb2 untouched.
*/

K_merge(kdb1, kdb2, replace)
KDB     *kdb1, *kdb2;
int     replace;
{
    int     i, pos;
    char    *ptr;

    if(kdb1 == NULL || kdb2 == NULL) return(-1);
    for(i = 0; i < KNB(kdb2); i++) {
	pos = K_find(kdb1, KONAME(kdb2, i));
	if(pos < 0) pos = K_add_entry(kdb1, KONAME(kdb2, i));
	else {
	    if(!replace) continue;
	    SW_free(KSOVAL(kdb1, pos));
	}

	if(pos < 0) return(-1);
	ptr = KGOVAL(kdb2, i);
	KSOVAL(kdb1, pos) = SW_alloc(P_len(ptr));

	ptr = KGOVAL(kdb2, i); /* GB 26/01/98 */
	memcpy(KGOVAL(kdb1, pos), ptr, P_len(ptr));
    }
    return(0);
}

/*
    Merges two databases : kdb1 <- kdb1 + kdb2
    If replace == 0, existing elements in kdb1 are not overwritten.
    If replace == 1, the values of existing elements are replaced.

    kdb2 is deleted simultaneously.
*/

K_merge_del(kdb1, kdb2, replace)
KDB   *kdb1, *kdb2;
int   replace;
{

    if(kdb1 == NULL || kdb2 == NULL) return(-1);
    if(KNB(kdb2) == 0) return(-1);
    if(KNB(kdb1) == 0) {
	KNB(kdb1) = KNB(kdb2);
	KOBJS(kdb1) = KOBJS(kdb2);
	KNB(kdb2) = 0;
	KOBJS(kdb2) = 0;
	K_free(kdb2);
	return(0);
    }

    K_merge(kdb1, kdb2, replace);
    K_free(kdb2);
    return(0);
}

/* FILE I/O */

#define kread(buf, so, nbo, fd)  if(fread(buf, so, nbo, fd) != nbo) goto error;
#define kseek(fd, where, how)    if(fseek(fd, where, how) < 0) goto error;
#define kwrite(buf, so, nbo, fd)  if(fwrite(buf, so, nbo, fd) != nbo) goto error;

K_read_len(fd, vers, len)
FILE    *fd;
int     vers;
OSIZE   *len;
{
    OSIZE   os;
    U_sh    us;
    int     rc;

    if(vers == 0 || vers == 3)  { // Long si vers courante (0) ou == 3
	rc = fread(&os, sizeof(OSIZE), 1, fd);
	if(rc != 1) return(-1);
	K_xdrPLONG(&os);
	kseek(fd, -1L * sizeof(OSIZE), 1);
	*len = os;
	return(0);
    }
    else {
	rc = fread(&us, sizeof(U_sh), 1, fd);
	if(rc != 1) goto error;
	K_xdrPINT((char *)&us);
	kseek(fd, -1L * sizeof(U_sh), 1);
	*len = us;
	return(0);
    }
error:
    return(-1);
}

char *K_set_ext(res, fname, type)
char    *res, *fname;
int     type;
{
    strcpy(res, fname);
    K_strip(res); /* JMP 19-04-98 */
    return(K_add_ext(res, k_ext[type]));
}

/* =================================================================
Loads a DB file(fname).

&EN int ftype = file type (K_CMT -> K_VAR)
&EN FNAME fname = filename
&EN int no    = 0 for all objects, 1 for a list (in objs)
&EN char **objs = null or list of objects to load

&RT DB structure if all goes well,
    NULL in case of error
-------------------------------------------------------------------- */
KDB  *K_load(ftype, fname, no, objs)
int     ftype, no;
FNAME   fname;
char    **objs;
{
    int     i, j, lpos, pos, nf, vers, rc;
    char    *ptr, *cptr, *aptr, label[512], fullpath[1024];
    OSIZE   len, clen;
    KDB     *kdb = NULL, *tkdb = NULL;
    OKDB643 *okdb643;
    KOBJ    *kobj;
    FNAME   file;
    FILE    *fd;

    if(U_is_in('!', fname)) return(K_load_odbc(ftype, fname, no, objs));

    K_set_ext(file, fname, ftype);
    fd = fopen(file, "rb");

    if(fd == NULL) {
	kmsg("File %s does not exist", file);
	return(NULL);
    }
    /*    setvbuf(fd, NULL, 0, 8192); */

    /* CHECK OBJS LABEL */
    kread(label, strlen(K_LABEL), 1, fd);
    vers = K_calcvers(label);
    if(vers < 0 || vers > 2) { // JMP 3/6/2015
	kmsg("%s : invalid iode file", file);
	fclose(fd);
	return(NULL);
    }

    // Allocate empty kdb
    kdb = K_create(0, 0);
    if(kdb == NULL) return(NULL);

    // Read kdb struct from open file (fd) and transpose into kdb 64 if X64
    K_read_kdb(kdb, fd, vers);

    if(ftype != KTYPE(kdb) || strcmp(ARCH, KARCH(kdb)) != 0) {
	kmsg("%s : unvalid type(%s) or wrong architecture(%s)",
	     file, k_ext[KTYPE(kdb)], KARCH(kdb));
	goto error;
    }

    // Set filename in kdb->k_nameptr
    if(strncmp(file, "ws.", 3)) {
	ptr = SCR_fullpath(file, fullpath);
	if(ptr == 0) ptr = file;
	//strcpy(kdb->k_name, ptr);
	K_set_kdb_name(kdb, ptr);  // JMP 3/6/2015
    }

    // Load object table (32 bits == 64 bits)
    // TODO: faire une sous-fonction K_read_objtable
    i = (KNB(kdb) / K_CHUNCK + 1) * K_CHUNCK;
    KOBJS(kdb) = (KOBJ *) SW_nalloc(sizeof(KOBJ) * i);
    if(KOBJS(kdb) == NULL) goto error;

    // CHECK VERSION KOBJ and load object tables
    if(vers == 1 || vers == 2) { // short names
	OKOBJ   *okobj;

	okobj = (OKOBJ *) SW_nalloc(sizeof(OKOBJ) * KNB(kdb));
	kread((char *) okobj, sizeof(OKOBJ), KNB(kdb), fd);
	for(j = 0 ; j < KNB(kdb) ; j++)
	    strcpy(KONAME(kdb, j), okobj[j].o_name);
	SW_nfree(okobj);
    }
    else {
	if(kdb->k_compressed == 0) {
	    // Normal case : no compression / long names
	    kread((char *) KOBJS(kdb), sizeof(KOBJ), (int) KNB(kdb), fd);
	}
	else {
	    if(K_read_len(fd, vers, &len)) goto error;
	    clen = len = -len;
	    cptr = SW_nalloc(clen);
	    kread(&len, sizeof(OSIZE), 1, fd);
	    kread(cptr, clen, 1, fd);
	    //LzhDecodeStr(cptr, clen, &aptr, &len);
	    GzipDecodeStr(cptr, clen, &aptr, &len);
	    memcpy((char *)KOBJS(kdb), aptr, len);
	    SW_nfree(cptr);
	    SCR_free(aptr);
	}
    }

    // Si aucun nom n'est donné, on charge tout
    if(no == 0) {
	for(i = 0; i < KNB(kdb); i++) {
	    if(K_read_len(fd, vers, &len)) goto error;

	    if(len < 0) {
		// Si len < 0 => version zippée
		clen = len = -len;
		cptr = SW_nalloc(clen);
		kread(&len, sizeof(OSIZE), 1, fd);
		kread(cptr, clen, 1, fd);
		//LzhDecodeStr(cptr, clen, &aptr, &len);
		GzipDecodeStr(cptr, clen, &aptr, &len);
		KOBJS(kdb)[i].o_val = SW_alloc(len);
		ptr = SW_getptr(KOBJS(kdb)[i].o_val);
		memcpy(ptr, aptr, len);
		SW_nfree(cptr);
		SCR_free(aptr);
	    }
	    else {
		// Si len >= 0 => version non zippée (dft)
		KOBJS(kdb)[i].o_val = SW_alloc(len);
		ptr = SW_getptr(KOBJS(kdb)[i].o_val);
		if(ptr == 0) {
		    kerror(0, "Memory full ? %s[%d]", __FILE__, __LINE__); // JMP 15/7/2013
		    kerror(0, "o_val = %d, block : %d", KOBJS(kdb)[i].o_val, SW_get_blk(KOBJS(kdb)[i].o_val));

		}
		kread(ptr, len, 1, fd);
	    }
	    K_xdrobj[KTYPE(kdb)](ptr, NULL);
	    K_setvers(kdb, i, vers);
	}
    }
    // Sinon on charge la liste demandée
    else  {
	/* get data of specified objects */
	tkdb = K_create(KTYPE(kdb), KMODE(kdb));
	if(tkdb == NULL) goto error;

	memcpy(tkdb, kdb, sizeof(KDB));
	KNB(tkdb) = 0;
	KOBJS(tkdb) = NULL;

	nf = SCR_tbl_size(objs);
	for(i = 0; i < nf; i++) K_add_entry(tkdb, objs[i]);

	lpos = 0;

	if(KTYPE(kdb) == K_VAR || KTYPE(kdb) == K_SCL) {
	    if(K_read_len(fd, vers, &len)) goto error;
	}

	for(i = 0; i < KNB(tkdb); i) {
	    pos = K_find(kdb, KONAME(tkdb, i));
	    if(pos < 0) {
		K_del_entry(tkdb, i);
		continue;
	    }

	    /* skip this pos - lpos entries */
	    if(kdb->k_compressed == 0 && (KTYPE(kdb) == K_VAR || KTYPE(kdb) == K_SCL)) {
		if(pos - lpos > 0)
		    kseek(fd, (long) len * (pos -lpos), 1);
	    }
	    else {
		for(j = 0; j < pos - lpos; j++) {
		    if(K_read_len(fd, vers, &len)) goto error;
		    if(len < 0) len = -len + sizeof(OSIZE);
		    kseek(fd, (long)len, 1);
		}
	    }

	    if(K_read_len(fd, vers, &len)) goto error;

	    if(len < 0) {
		clen = len = -len;
		cptr = SW_nalloc(clen);
		kread(&len, sizeof(OSIZE), 1, fd);
		kread(cptr, clen, 1, fd);
		//LzhDecodeStr(cptr, clen, &aptr, &len);
		GzipDecodeStr(cptr, clen, &aptr, &len);
		KOBJS(tkdb)[i].o_val = SW_alloc(len);
		ptr = SW_getptr(KOBJS(tkdb)[i].o_val);
		memcpy(ptr, aptr, len);
		SW_nfree(cptr);
		SCR_free(aptr);
	    }
	    else {
		KOBJS(tkdb)[i].o_val = SW_alloc(len);
		ptr = SW_getptr(KOBJS(tkdb)[i].o_val);
		kread(ptr, len, 1, fd);
	    }

	    K_xdrobj[KTYPE(tkdb)](ptr, NULL);
	    K_setvers(tkdb, i, vers); /* JMP 10-04-98 */
	    lpos = pos + 1;
	    i++;
	}

	K_free_kdb(kdb); /* no data in kdb */
	kdb = tkdb;
    }

    fclose(fd);
    return(kdb);

error:
    fclose(fd);
    if(no == 0) {
	if(i == 0) K_free(kdb);
	else { /* JMP 31-01-00 */
	    KNB(kdb) = i;
	    return(kdb);
	}
    }
    else {
	K_free(tkdb);
	K_free_kdb(kdb);
    }
    return(NULL);
}

K_read_kdb(KDB *kdb, FILE *fd, int vers)
{
    OKDB643     *okdb643, kdb643;
    KDB32       kdb32;
    int         posnb, posname;

    okdb643 = &kdb643;
    memset((char *)&kdb643, 0, sizeof(kdb643)); // JMP 28/10/2016 correction plantage fichier 2000 de PV

    switch(vers) {
	case 0 :
	    // Current version
	    if(X64) {
		kread((char *) &kdb32, sizeof(KDB32), 1, fd);
		memset(kdb, 0, sizeof(KDB));
		posnb   = (char *)&kdb->k_nb      - (char *)kdb;
		posname = (char *)&kdb->k_nameptr - (char *)kdb;
		memcpy(&(kdb->k_nb), &(kdb32.k_nb), 1 + posname - posnb);
	    }
	    else
		kread((char *) kdb, sizeof(KDB), 1, fd);
	    break;
	case 1 :
	case 2 : // JMP 28/10/2016 correction plantage fichier 2000 (PV -> 504d) à vérifier
	    // TODO: gérer la lecture des vers !=  si X64
	    kread((char *) okdb643, sizeof(OKDB), 1, fd);
	    break;
	case 3 :
	    kread((char *) okdb643, sizeof(OKDB643), 1, fd);
	    break;
    }

    if(vers != 0) {
	kdb->k_nb      = okdb643->k_nb  ;
	kdb->k_type    = okdb643->k_type  ;
	kdb->k_mode    = okdb643->k_mode  ;
	memcpy(kdb->k_arch,  okdb643->k_arch, LMAGIC);
	memcpy(kdb->k_magic, okdb643->k_magic, LMAGIC);
	memcpy(kdb->k_oname,  okdb643->k_name, OK_MAX_FILE); /* JMP 03-06-2015 */
	memcpy(kdb->k_desc,  okdb643->k_desc, K_MAX_DESC);
	memcpy(kdb->k_data,  okdb643->k_data, K_MAX_DESC);

	if(vers == 2 || vers == 3) {
	    kdb->k_compressed = okdb643->k_compressed;
	    memset(kdb->k_reserved, 0, sizeof(kdb->k_reserved));
	}
    }

    memset(kdb->k_reserved, 0, sizeof(kdb->k_reserved));
    KNAMEPTR(kdb) = 0;
    K_xdrKDB(kdb, NULL);
    return(0);
error :
    return(-1);
}

K_calcvers(label)
char    *label;
{
    if(memcmp(label, K_LABEL, strlen(K_LABEL)) == 0) return(0);     // Current version
    if(memcmp(label, K_LABELS, strlen(K_LABELS)) == 0) return(1);
    if(memcmp(label, K_LABELD, strlen(K_LABELD)) == 0) return(2);
//      if(memcmp(label, K_LABELX, strlen(K_LABELX)) == 0) return(3);
    return(-1);
}

K_repack(kdb, objnb)
KDB     *kdb;
int     objnb;
{
    SWHDL   opos, pos;
    OSIZE   *ptr;
    U_sh    *optr;
    int     nlg, add, lendata, i;

    opos = KOBJS(kdb)[objnb].o_val;
    optr = (U_sh *) SW_getptr(opos);
    add = (optr[1] + 2 + optr[1] % 2) * (sizeof(OSIZE) - sizeof(U_sh));
    nlg = optr[0] + add;
    pos = SW_alloc(nlg);
    optr = (U_sh *) SW_getptr(opos);
    ptr = (OSIZE *) SW_getptr(pos);
    ptr[0] = nlg;
    ptr[1] = optr[1];
    for(i = 2 ; i < optr[1] + 2 ; i++)
	ptr[i] = optr[i] + add;

    lendata = nlg - (ptr[1] + 2 + optr[1] % 2) * sizeof(OSIZE);
    memcpy(((char *) ptr) + (ptr[1]  + 2 + ptr[1] % 2) * sizeof(OSIZE),
	   ((char *)optr) + (optr[1] + 2 + optr[1] % 2) * sizeof(U_sh),
	   lendata);
    SW_free(opos);
    KOBJS(kdb)[objnb].o_val = pos;
}

char *Pack16To32(opack)
char    *opack;
{
    SWHDL   opos, pos;
    OSIZE   *ptr;
    U_sh    *optr = (U_sh *)opack;
    int     nlg, add, lendata, i;

    add = (optr[1] + 2 + optr[1] % 2) * (sizeof(OSIZE) - sizeof(U_sh));
    nlg = optr[0] + add;
    ptr = (OSIZE *)SW_nalloc(nlg);
    ptr[0] = nlg;
    ptr[1] = optr[1];
    for(i = 2 ; i < optr[1] + 2 ; i++)
	ptr[i] = optr[i] + add;

    lendata = nlg - (ptr[1] + 2 + optr[1] % 2) * sizeof(OSIZE);
    memcpy(((char *) ptr) + (ptr[1]  + 2 + ptr[1] % 2) * sizeof(OSIZE),
	   ((char *)optr) + (optr[1] + 2 + optr[1] % 2) * sizeof(U_sh),
	   lendata);
    return((char *)ptr);
}

char *T_cell_repack(pack, cell)
char    *pack;
TCELL   *cell;
{
    char    *npack, *ipack;

    if(cell->tc_val == NULL) return(pack);
    if(cell->tc_type == KT_LEC) {
	npack = Pack16To32(cell->tc_val);
	ipack = 0;
	K_ipack(&ipack, P_get_ptr(npack, 0));
	pack  = P_add(pack, ipack, P_len(ipack));
	SW_nfree(npack);
    }
    else
	pack = P_add(pack, cell->tc_val, strlen(cell->tc_val) + 1);

    return(pack);
}


char *K_repack_tbl(TBL *tbl)
{
    TLINE   *line;
    TCELL   *cell;
    int     i, j;
    char    *pack;

    pack = P_create();
    if(tbl == NULL) return(NULL);

    /* tbl */
    pack = P_add(pack, (char *) tbl, sizeof(TBL));

    /* div */
    cell = (TCELL *) tbl->t_div.tl_val;
    pack= P_add(pack, (char *) cell, sizeof(TCELL) * (int) T_NC(tbl));
    for(j = 0; j < T_NC(tbl); j++)
	pack = T_cell_repack(pack, cell + j);

    /* lines */
    pack= P_add(pack, (char *) tbl->t_line, sizeof(TLINE) * (int) T_NL(tbl));
    for(i = 0; i < T_NL(tbl); i++) {
	switch(tbl->t_line[i].tl_type) {
	    case KT_CELL :
		cell = (TCELL *) tbl->t_line[i].tl_val;
		pack= P_add(pack, (char *) cell, sizeof(TCELL) * (int) T_NC(tbl));
		for(j = 0; j < T_NC(tbl); j++)
		    pack = T_cell_repack(pack, cell + j);
		break;
	    case KT_TITLE :
		cell = (TCELL *) tbl->t_line[i].tl_val;
		pack= P_add(pack, (char *) cell, sizeof(TCELL));
		pack= T_cell_repack(pack, cell);
		break;
	}
    }

    return(pack);
}

K_setvers(kdb, i, vers)
KDB     *kdb;
int     i, vers;
{
    float   *f;
    double  *d;
    int     j, nb, lg;
    char    *ptr, *optr, *pack;
    SWHDL   pos, opos;
    char    buf[512];
    unsigned char *dptr;
    EQ      *eq;
    TBL     *tbl;

    if(vers == 0 || vers == 3) return(0);
    K_repack(kdb, i);
    switch(KTYPE(kdb)) {
	case K_VAR :
	    if(vers == 2) return(0);
	    opos = KOBJS(kdb)[i].o_val;
	    nb = KSMPL(kdb)->s_nb;
	    pos = KV_alloc_var(nb);
	    ptr = SW_getptr(pos);
	    d = (double *)P_get_ptr(ptr, 0);
	    optr = SW_getptr(opos);
	    f = (float *)P_get_ptr(optr, 0);
	    for(j = 0 ; j < nb ; j++) {
		sprintf(buf, "%.8g", f[j]);
		d[j] = atof(buf);
		if(L_IS0(d[j])) d[j] = 0.0; /* JMP 30-10-98 */
		/*d[j] = (double)f[j]; */
	    }
	    SW_free(opos);
	    KOBJS(kdb)[i].o_val = pos;
	    break;

	case K_SCL :
	    if(vers == 2) return(0);
	    nb = 3;
	    /* Do New allocation & get dbl ptr*/
	    pos = KS_alloc_scl();
	    ptr = SW_getptr(pos);
	    d = (double *)P_get_ptr(ptr, 0);

	    /* Get old allocation and get float ptr */
	    opos = KOBJS(kdb)[i].o_val;
	    optr = SW_getptr(opos);
	    f = (float *)P_get_ptr(optr, 0);

	    /* Convert */
	    for(j = 0 ; j < 3 ; j++) {
		d[j] = (double)f[j];
		//Debug("%.8lg", d[j]);
		sprintf(buf, "%.8lg", d[j]);
		d[j] = atof(buf);
		if(L_IS0(d[j])) d[j] = 0.0; /* JMP 30-10-98 */
	    }

	    SW_free(opos);
	    KOBJS(kdb)[i].o_val = pos;
	    break;

	case K_IDT :
	    opos = KOBJS(kdb)[i].o_val;
	    optr = SW_getptr(opos);
	    K_ipack(&pack, P_get_ptr(optr, 0));
	    SW_free(opos);
	    lg = P_get_len(pack, -1);
	    pos = SW_alloc(lg);
	    memcpy(SW_getptr(pos), pack, lg);
	    SW_nfree(pack);
	    KOBJS(kdb)[i].o_val = pos;
	    break;

	case K_EQS :
	    opos = KOBJS(kdb)[i].o_val;
	    optr = SW_getptr(opos);
	    eq = K_eunpack(optr);
	    SW_free(opos);
	    K_epack(&pack, (char *)eq, KOBJS(kdb)[i].o_name);
	    E_free(eq);
	    lg = P_get_len(pack, -1);
	    pos = SW_alloc(lg);
	    memcpy(SW_getptr(pos), pack, lg);
	    SW_nfree(pack);
	    KOBJS(kdb)[i].o_val = pos;
	    break;

	case K_TBL :
	    opos = KOBJS(kdb)[i].o_val;
	    optr = SW_getptr(opos);
	    tbl = K_tunpack(optr);
	    SW_free(opos);
	    pack = K_repack_tbl(tbl);
	    T_free(tbl);
	    lg = P_get_len(pack, -1);
	    pos = SW_alloc(lg);
	    memcpy(SW_getptr(pos), pack, lg);
	    SW_nfree(pack);
	    KOBJS(kdb)[i].o_val = pos;
	    break;

	default :
	    break;
    }

    return(0);
}

/*
    Interprets the contents of a file into a  db
    If the file is an ascii file the fn_ascii function provided
    will interpret this file into a valid db

    It returns a pointer to the DB, if this is a NULL pointer
    the file does not exist or is incompatible with the given
    type
*/
KDB *K_interpret(type, filename)
int     type;
char    *filename;
{
    KDB     *kdb = NULL;
    int     ftype;

    kmsg("Loading %s", filename);
    ftype = K_findtype(filename, type);
//    printf("type=%d, ftype=%d\n", type, ftype);
    if(ftype == -1) kdb = (*K_load_asc[type])(filename);

    if(kdb == NULL && type == ftype) kdb = K_load(type, filename, 0, NULL); /* GB 23/01/98*/

    if(kdb == NULL) {
	kerror(0, "%s not found or incompatible type", filename);
	return(NULL);
    }

    if(kdb != NULL) kmsg("%d objects loaded", (int) KNB(kdb));

    return(kdb);
}


/*
    Copies from a list of files a nbr of specified objects.
    Returns the number of objects copied.

    Note : if a given object can be found in more than one file
    only the first value is taken
*/
K_copy(kdb, nf, files, no, objs, smpl)
KDB     *kdb;
int     nf, no;
char    **files, **objs;
SAMPLE  *smpl;
{
    int     i, j, nb, nb_found = 0, *found = NULL;

    if(kdb == NULL) return(-1);

    if(objs == NULL || no == 0) return(0);
    found = (int *)SW_nalloc(no * sizeof(int));
    if(files == NULL || nf == 0) goto fin;

    for(i = 0; i < nf && nb_found < no; i++) {
	nb = K_copy_1(kdb, files[i], no, objs, found, smpl);
	if(nb < 0) {
	    SW_nfree(found);
	    return(-1);
	}
	nb_found += nb;
    }

fin:
    if(nb_found < no) {
	for(i = 0, j = 0 ; i < no && j < 10; i++) {
	    if(found[i] == 0) B_seterrn(98, objs[i]);
	    j++;
	}
	if(j == 10) B_seterrn(91);
	return(-2);
    }

    SW_nfree(found);
    return(0);
}

/*
    Copies from a given file a nbr of specified objects.
    Returns the number of objects copied.
*/
int K_copy_1(to, file, no, objs, found, smpl)
KDB     *to;
FNAME   file;
int     no;
char    **objs;
int     *found;
SAMPLE  *smpl;
{
    int     i, pf, pt, rc = 0, nb_found = 0;
    KDB     *from;
    char    *ptr, *pack;
    SAMPLE  csmpl;

    from = K_load(KTYPE(to), file, no, objs);
    if(from == NULL) return(-1);

    if(KTYPE(to) == K_VAR) {
	if(smpl == NULL) memcpy(&csmpl, KSMPL(from), sizeof(SAMPLE));
	else rc = PER_common_smpl(smpl, KSMPL(from), &csmpl);

	if(rc < 0) {
	    B_seterror("File sample and copy sample do not overlap");
	    goto the_end;
	}
	/* delete already found variables */
	for(i = 0 ; i < no; i++) {
	    pf = K_find(from, objs[i]);
	    if(pf < 0) continue;

	    if(found[i]) K_del(from, pf);
	    found[i] = 1;
	}

	rc = KV_sample(from, &csmpl);
	if(rc < 0) goto the_end;
	nb_found = KNB(from);
	rc = KV_merge(to, from, 1);
    }
    else {
	for(i = 0 ; i < no; i++) {
	    if(found[i]) continue;
	    pf = K_find(from, objs[i]);
	    if(pf < 0) continue;

	    found[i] = 1;

	    ptr = KGOVAL(from, pf);
	    pack = SW_nalloc(P_len(ptr));
	    memcpy(pack, ptr, P_len(ptr));

	    pt = K_find(to, objs[i]);
	    if(pt >= 0) { /* series already there, replace */
		SW_free(KSOVAL(to, pt));
	    }
	    else pt = K_add_entry(to, objs[i]);

	    KSOVAL(to, pt) = SW_alloc(P_len(pack));
	    memcpy(KGOVAL(to, pt), pack, P_len(pack));
	    SW_nfree(pack);
	    nb_found++;
	}
    }

the_end:
    K_free(from);
    if(rc < 0) return(0);
    else return(nb_found);
}

K_backup(filename)              /* JMP 17-12-93 */
char    *filename;
{
    FNAME   backname;

    if(access(filename, 0)) return(0);
    strcpy(backname, filename);
    backname[strlen(backname) - 1] = '$';
    unlink(backname);
    return(SCR_rename(filename, backname));
}

K_cwrite(method, buf, len, nb, fd, minlen)
int     method;
FILE    *fd;
char    *buf;
OSIZE   len;
int     nb, minlen;
{
    char    *cptr;
    OSIZE   clen;

    len *= nb;

    if(method == 0 || len < minlen) {
	kwrite(buf, len, 1, fd);
	return(len);
    }

    GzipEncodeStr(buf, len, &cptr, &clen);
    if(minlen > 0 && len <= clen + sizeof(OSIZE)) {
	kwrite(buf, len, 1, fd);
	clen = -len;
    }
    else {
	clen = -clen;
	kwrite(&clen, sizeof(OSIZE), 1, fd);
	kwrite(cptr, -clen, 1, fd);
    }

    SCR_free(cptr);
    return(-clen);
error :
    return(-1);
}

/*
    Saves a DB-structure in a named.
    Returns -1 if the DB is empty or the file cannot be opened
    to write in.
*/
K_save_kdb(kdb, fname, mode)
KDB     *kdb;
FNAME   fname;
int     mode;
{
    int     i, len, clen, posnb, posname;
    char    *ptr, *cptr, *xdr_ptr = NULL;
    KOBJ    *kobj;
    KDB     *xdr_kdb = NULL;
    KDB32   kdb32;
    FNAME   file;
    FILE    *fd;

    if(kdb == NULL) return(-1);

    strcpy(file, fname);
    /*SCR_sqz(file); */
    K_strip(file); /* JMP 21-04-98 */
    K_add_ext(file, k_ext[KTYPE(kdb)]);
    K_backup(file);     /* JMP 17-12-93 */
    fd = fopen(file, "wb+");
    if(fd == NULL) return(-1);
    setvbuf(fd, NULL, 0, 8192);
    kmsg("Saving %s", file);

    if(mode) {
	K_set_kdb_name(kdb, file); // JMP 3/6/2015
	//strcpy(KNAME(kdb), file);
    }

    kwrite(K_LABEL, strlen(K_LABEL), 1, fd); /* JMP 23-05-00 */

    /* XDR  KDB */
    kdb->k_compressed = K_LZH; /* JMP 25-05-00 */
    K_xdrKDB(kdb, &xdr_kdb);

    // Dump KDB struct
    if(X64) {
	/* convert to x64 if needed */
	memset(&kdb32, 0, sizeof(KDB));
	posnb   = (char *)&xdr_kdb->k_nb      - (char *)xdr_kdb;
	posname = (char *)&xdr_kdb->k_nameptr - (char *)xdr_kdb;
	memcpy(&(kdb32.k_nb), &(xdr_kdb->k_nb), 1 + posname - posnb);
	kwrite((char *) &kdb32, sizeof(KDB32), 1, fd);
	}
    else  {
	kwrite(xdr_kdb, sizeof(KDB), 1, fd);
    }

    SW_nfree(xdr_kdb);

    // Dump KOBJ table
    if(K_cwrite(kdb->k_compressed, KOBJS(kdb), sizeof(KOBJ), KNB(kdb), fd, -1) < 0) goto error;

    // Dump objects
    for(i = 0; i < KNB(kdb); i++) {
	/* XDR  OBJ */
	ptr = SW_getptr(KOBJS(kdb)[i].o_val);
	len = P_len(ptr);
	K_xdrobj[KTYPE(kdb)](ptr, &xdr_ptr);
	if(K_cwrite(kdb->k_compressed, xdr_ptr, len, 1, fd, 20) < 0) goto error;
	SW_nfree(xdr_ptr);
    }

    kmsg("%d objects saved", (int) KNB(kdb));
    fclose(fd);
    return(0);

error:
    kmsg("Error saving, try again");
    fclose(fd);
    return(-1);

}

K_save(kdb, fname)
KDB     *kdb;
FNAME   fname;
{
    return(K_save_kdb(kdb, fname, 1));
}

K_save_ws(kdb)
KDB     *kdb;
{
    return(K_save_kdb(kdb, "ws", 0));
}


/*NH*/
char *K_add_ext(filename, ext)
char    *filename, *ext;
{
    return(SCR_change_ext(filename, filename, ext));

    /*     return(SCR_cat_ext(filename, filename, ext)); GB 23/01/98 */
}

/*NH*/
int K_SECRETSEP = '#'; // JMP 14/2/2013 pour les macros pushed A#n

int K_key(name, mode)
char    *name;
int     mode;
{
    int     i;


    SCR_sqz(name);
    if(!SCR_is_alpha(name[0]) && name[0] != '_') return(-1);
    if(strlen(name) > K_MAX_NAME) name[K_MAX_NAME] = 0;
    for(i = 1 ; name[i] ; i++)
	if(!SCR_is_anum(name[i]) && name[i] != '_' && name[i] != K_SECRETSEP) return(-1); // JMP 14/2/2013 pour les macros pushed A#n

    switch(mode) {
	case K_UPPER :
	    SCR_upper(name);
	    break;
	case K_LOWER :
	    SCR_lower(name);
	    break;
	case K_ASIS  :
	    break;
    }

    return(0);
}

/*
    Finds a file type. If filename does not exist, adds the extension
    defined in k_ext[type], and search again.

    If filename or filename.ext does not exist, it returns -2. If the file
    is of an unrecognized type, returns -1. Otherwise, returns type as
    defined in the file header.
*/

K_findtype(filename, type)
char    *filename;
int     type;
{
    int     ftype;
    char    buf[1024];

    ftype = K_filetype(filename, 0L, 0L, 0L);
    if(type == K_VAR && U_is_in('!', filename)) return(type); /* Give ODBC a try */

    if(ftype != -2) return(ftype);
    strcpy(buf, filename);
    K_add_ext(buf, k_ext[type]);
    return(K_filetype(buf, 0L, 0L, 0L));
}

K_setname(type, from, to)
int     type;
char     *from, *to;
{
    char    label[512];
    KDB     kdb;
    FILE    *fd;

    fd = fopen(from, "rb+");
    if(fd == NULL)  return(-1);

    kread(label, strlen(K_LABEL), 1, fd);
    /*    if(memcmp(label, K_LABEL, strlen(K_LABEL))) {     /* JMP 09-04-98 */
    if(K_calcvers(label) < 0) {                         /* JMP 09-04-98 */
	kmsg("%s : unvalid iode file", from);
	fclose(fd);
	return(-1);
    }

    kread((char *) &kdb, sizeof(KDB), 1, fd);
    // strcpy(kdb.k_name, to);
    K_set_kdb_name(&kdb, to); // JMP 3/6/2015
    kseek(fd, (long) -1L * sizeof(KDB), 1);
    kwrite((char *) &kdb, sizeof(KDB), 1, fd);

    fclose(fd);
    return(0);

error:
    return(-1);
}

/*
    compares object name from 2 different kdb's
    returns 0 : if name not in 1, not in 2
	    1 ; if name in 1, not in 2
	    2 ; if name not in 1, in 2
	    3 ; if name in 1, in 2 and 1 = 2
	    4 ; if name in 1, in 2 and 1 != 2
*/

K_cmp(name, kdb1, kdb2)
KDB     *kdb1, *kdb2;
char    *name;
{
    int     p1, p2, l1, l2;
    char    *pck1, *pck2;

    if(KTYPE(kdb1) != KTYPE(kdb2)) return(-1);

    p1 = K_find(kdb1, name);
    p2 = K_find(kdb2, name);

    if(p1 < 0) {
	if(p2 < 0) return(0);   /* not 1, not 2 */
	else return(2);         /* not 1, 2 */
    }

    if(p2 < 0) return(1);      /* 1, not 2 */

    /* 1, 2 */
    return(3 +
	   K_cmpobj[KTYPE(kdb1)](KGOVAL(kdb1, p1), KGOVAL(kdb2, p2), name));
}

void K_strip(filename)
char *filename;
{
    U_ljust_text(filename);
    SCR_strip(filename);
}

