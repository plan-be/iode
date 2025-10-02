/**
 * @header4iode
 *
 * IODE object file management
 * ---------------------------
 *      char *K_add_ext(char* filename, char* ext)                                      sets the extension of a filename.
 *      int K_get_ext(char* filename, char* ext, int max_ext_len)                       gets a filename extension.
 *      int K_has_ext(char* filename)                                                   indicates if a filename contains an extension.
 *      char *K_set_ext(char* res, char* fname, int type)                               deletes left and right spaces in a filename and changes its extension according to the given type.
 *      char *K_set_ext_asc(char* res, char* fname, int type)                           trims a filename then changes its extension to the ascii extension according to the given type).
 *      void K_strip(char* filename)                                                    deletes left and right spaces in a filename. Keeps the space inside the filename.
 *      KDB  *K_load(int ftype, FNAME fname, int no, char** objs)                       loads a IODE object file. 
 *      int K_filetype(char* filename, char* descr, int* nobjs, Sample* smpl)           retrieves infos on an IODE file: type, number of objects, Sample
 *      KDB *K_interpret(int type, char* filename): generalisation of K_load()          interprets the content of a file, ascii files included, and try to load ist content into a KDB.
 *      int K_copy(KDB* kdb, int nf, char** files, int no, char** objs, Sample* smpl)   reads a list of objects from a list of IODE object files and adds them to an existing KDB.
 *      int K_cat(KDB* ikdb, char* filename)                                            concatenates the content of a file to an existing kdb.
 *      int K_set_backup_on_save(int take_backup)                                       sets the backup choice before saving a kdb. 
 *      int K_get_backup_on_save()                                                      indicates if a backup must be taken before saving a kdb. 
 *      int K_backup(char* filename)                                                    takes a backup of a file by renaming the file: filename.xxx => filename.xx$.
 *      int K_save(KDB* kdb, FNAME fname)                                               saves a KDB in an IODE object file. The extension of fname is replaced by the standard one (.cmt, .eqs...).
 *      int K_save_ws(KDB* kdb)                                                         saves a KDB in an IODE object file called "ws.<ext>" where <ext> is one of (.cmt, .eqs...).
 *      int K_setname(char* from, char* to)                                             replaces KNAMEPTR(kdb) in an IODE object file.
 */
#ifndef UNIX
#include <io.h>
#endif

#include "scr4/s_prodir.h"

#include "api/constants.h"
#include "api/k_super.h"
#include "api/b_errors.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/ascii/ascii.h"
#include "api/objs/xdr.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/variables.h"
#include "api/objs/old_structs.h"
#include "api/objs/structs_32.h"


// UTILITIES FOR STANDARDISING/MODIFYING FILENAMES AND EXTENSIONS
// --------------------------------------------------------------

/**
 *  Forces the extension of a filename. Filename must be large enough to include the extension.
 *   
 *  @param [in, out]    filename    char*   source / target filename
 *  @param [in]         ext         char*   new extension
 *  @return                         char*   filename modified (same pointer)
 *  
 *  @example : 
 *      strcpy(filename, "example");
 *      K_add_ext(filename, "var");
 *      printf("'%s'\n", filename);  // 'example.var'
 */
char *K_add_ext(char* filename, char* ext)
{
    char    buf[512];
    
    if(filename == 0 ||strlen(filename) > 511) return(NULL);
    SCR_change_ext(buf, filename, ext); // Force the replacement extension of filename by ext (see http://xon.be/scr4/libs1/libs1172.htm)
    strcpy(filename, buf);              
    return(filename); 
}


/**
 *  Gets a filename extension.
 *   
 *  @param [in]    filename      char*   filename
 *  @param [out]   ext           char*   filename extension
 *  @param [in]    max_ext_len   int     ext max length
 *  @return                      int     ext length (0 if no extension, -1 if filename == NULL)
 */
int K_get_ext(char* filename, char* ext, int max_ext_len)
{
    int     i, len;

    if(filename == 0) return(0);

    len = (int)strlen(filename);
    for(i = len - 1 ; i >= 0 ; i--) {
        if(filename[i] == ':' || filename[i] == '!' ||        /* JMP 23-06-02 */
           filename[i] == '.' || filename[i] == '\\' ||
           filename[i] == '/') break;
    }

    if(ext) ext[0] = 0;

    if(i >= 0 && filename[i] == '.') {
        if(ext && max_ext_len >= len - i)    // var.ac => len = 5, i = 3
            strcpy(ext, filename + i + 1);
        return(len - i - 1);
    }
    else 
        return(0);
}


/**
 *  Indicates if a filename contains an extension.
 *   
 *  @param [in]    filename    char*   source / target filename
 *  @return                    int     1 if an extension is detected, 0 otherwise
 *  
 *  @example : 
 *      strcpy(filename, "example");
 *      K_add_ext(filename, "var");
 *      printf("'%s'\n", filename);  // 'example.var'
 */

int K_has_ext(char* filename)
{
    int     i;

    if(filename == 0) return(0);
    for(i = (int)strlen(filename) - 1 ; i >= 0 ; i--) {
        if(filename[i] == ':' || filename[i] == '!' ||        /* JMP 23-06-02 */
           filename[i] == '.' || filename[i] == '\\' ||
           filename[i] == '/') break;
    }
    if(i >= 0 && filename[i] == '.') return(1);
    else return(0);
}


/**
 *  Trims a filename then changes its extension according to the given type.
 *   
 *  @param [out]    res   char*   resulting filename
 *  @param [in]     fname char*   input filename
 *  @param [in]     type  int     file type (value defined in iode.h between FILE_COMMENTS and FILE_CSV) 
 *  @return               char*   res (same pointer)
 *  
 *  @example : 
 *      char buf[256];
 *  
 *      strcpy(filename, "example.xxx      ");
 *      K_set_ext(buf, filename, VARIABLES);
 *      printf("'%s'\n", buf);  // 'example.var'
 *      K_set_ext(buf, filename, FILE_CSV);
 *      printf("'%s'\n", buf);  // 'example.csv'
 */
char *K_set_ext(char* res, char* fname, int type)
{
    strcpy(res, fname);
    K_strip(res); /* JMP 19-04-98 */
    return(K_add_ext(res, k_ext[type]));
}

/**
 *  Trims a filename then changes its extension to the ascii extension (according to the given type).
 *   
 *  @param [out]    res   char*   resulting filename
 *  @param [in]     fname char*   input filename
 *  @param [in]     type  int     file type (value defined in iode.h between FILE_COMMENTS and FILE_CSV) 
 *  @return               char*   res (same pointer)
 *  
 *  @example : 
 *      char buf[256];
 *  
 *      strcpy(filename, "example.xxx      ");
 *      K_set_ext_asc(buf, filename, VARIABLES);
 *      printf("'%s'\n", buf);  // 'example.av'
 */
char *K_set_ext_asc(char* res, char* fname, int type)
{
    strcpy(res, fname);
    K_strip(res); /* JMP 19-04-98 */
    return(K_add_ext(res, k_ext[ASCII_COMMENTS + type]));
}


/**
 *  Left and right trims a filename. Keeps the space inside the filename.
 *  
 *  @param [in, out]    filename  char*   string containing a filename
 */
void K_strip(char* filename)
{
    U_ljust_text((unsigned char*) filename);
    SCR_strip((unsigned char*) filename);
}



// UTILITIES FOR READING / WRITING IODE FILES
// ------------------------------------------         

// TODO: review the next 3 lines
#define kread(buf, so, nbo, fd)   if(fread(buf, so, nbo, fd) != (nbo))  goto error;
#define kseek(fd, where, how)     if(fseek(fd, where, how) < 0)         goto error;
#define kwrite(buf, so, nbo, fd)  if(fwrite(buf, so, nbo, fd) != (nbo)) goto error;


/**
 *  Reads a "length" on the stream fd and then rewinds the stream. The number of bytes depends
 *  on the IODE object version.
 *  
 *  For IODE object version 0, reads sizeof(OSIZE) bytes, 
 *  For IODE object versions 1 and 2, reads sizeof(short) bytes.
 *   
 *  
 *  @param [in]  fd     FD*     stream to read from 
 *  @param [in]  vers   int     version of IODE objects
 *  @param [out] len    OSIZE*  length read frol the stream    
 *  @return             int     -1 if the bytes cannot be read from the stream
 *                              0 on success
 */
static int K_read_len(FILE* fd, int vers, OSIZE* len)
{
    OSIZE   os;
    U_sh    us;
    //int     rc;
    size_t  rc; 

    if(vers == 0 || vers == 3)  { // Long si vers courante (0) ou == 3
        rc = fread(&os, (int)sizeof(OSIZE), 1, fd);
        if(rc != 1) return(-1);
        K_xdrPLONG((unsigned char*)&os);
        kseek(fd, -1L * sizeof(OSIZE), 1);
        *len = os;
    }
    else {
        rc = fread(&us, (int)sizeof(U_sh), 1, fd);
        if(rc != 1) return(-1);
        K_xdrPINT((unsigned char*) &us);
        kseek(fd, -1L * sizeof(U_sh), 1);
        *len = us;
    }
    return(0);
    
error:
    return(-1);
}


// LOAD AND SAVE IODE FILES
// ------------------------

/**
 *  Reads a KDB struct from an IODE object file and modify the read KDB struct according to the current
 *  IODE object version (0, 1 or 2) and 32 or 64 bits current architecture. 
 *  
 *  @note The previous versions of the KDB struct are OKDB and OKDB643.
 *        The 32 bit version of KDB (when in 64 bits executable) is KDB32.
 *      
 *  Warning: IODE objects versions 1 and 2 are not implemented in 64 bits !
 *  TODO: implement version 1 and 2 in 64 bits.
 *  
 *  @param [out] kdb    KBD*    modified KDB struct 
 *  @param [in] fd      FILE*   IO stream
 *  @param [in] vers    int     version of IODE objects (0-2 ?)
 *  @return             int     0 success, -1 on I/O error
 *  
 */
static int K_read_kdb(KDB *kdb, FILE *fd, int vers)
{
    OKDB643     *okdb643, kdb643;
    KDB32       kdb32;

    okdb643 = &kdb643;
    memset((char *)&kdb643, 0, sizeof(kdb643)); // JMP 28/10/2016 correction plantage fichier 2000 de PV

    switch(vers) {
        case 0 :
            // Current version
            if(X64) 
            {
                kread((char *) &kdb32, sizeof(KDB32), 1, fd);
                memset(kdb, 0, sizeof(KDB));
                kdb->k_nb = kdb32.k_nb;
                kdb->k_type = kdb32.k_type;
                kdb->k_mode = kdb32.k_mode;
                kdb->k_compressed = kdb32.k_compressed;
                memcpy(kdb->k_arch, kdb32.k_arch, LMAGIC);
                memcpy(kdb->k_magic, kdb32.k_magic, LMAGIC);
                memcpy(kdb->k_oname, kdb32.k_oname, OK_MAX_FILE); // JMP 03-06-2015 
                memcpy(kdb->k_desc, kdb32.k_desc, K_MAX_DESC);
                memcpy(kdb->k_data, kdb32.k_data, K_MAX_DESC);
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

    if(vers != 0) 
    {
        kdb->k_nb      = okdb643->k_nb  ;
        kdb->k_type    = okdb643->k_type  ;
        kdb->k_mode    = okdb643->k_mode  ;
        memcpy(kdb->k_arch,  okdb643->k_arch, LMAGIC);
        memcpy(kdb->k_magic, okdb643->k_magic, LMAGIC);
        memcpy(kdb->k_oname,  okdb643->k_name, OK_MAX_FILE); // JMP 03-06-2015 
        memcpy(kdb->k_desc,  okdb643->k_desc, K_MAX_DESC);
        memcpy(kdb->k_data,  okdb643->k_data, K_MAX_DESC);

        if(vers == 2 || vers == 3) 
        {
            kdb->k_compressed = okdb643->k_compressed;
            kdb->k_db_type = DB_GLOBAL; 
        }
    }

    kdb->k_db_type = DB_GLOBAL; 
    KNAMEPTR(kdb) = 0;
    K_xdrKDB(kdb, NULL);
    return(0);
error :
    return(-1);
}


/**
 *  Loads an IODE binary object file. 
 *  
 *  @note Prefer K_interpret() to load ascii or binary files.
 *  
 *  If needed: 
 *      - uncompresses the file
 *      - transposes objects to 64 bits 
 *      - converts to the current IODE object version.
 *  
 * @param [in]   ftype      int     file type (FILE_COMMENTS -> FILE_VARIABLES)
 * @param [in]   fname      FNAME   filename
 * @param [in]   load_all   int     0 for loading all objects, 1 for loading the list objs 
 * @param [in]   objs       char**  null or list of objects to load
 * @return                  KDB*    KDB with the content of filename restricted to objs if objs is not null
 *                               NULL on error
 *  
 * @note Messages are sent to the user via calls to 2 functions which must be defined 
 *       according to the context (console app, window app, Qt app...) : 
 *          - kmsg() for notifications
 *          - kerror() for error messages (TODO: check the use of ksmg on errors)
 *                                   
 */
KDB  *K_load(int ftype, FNAME fname, int load_all, char** objs)
{
    int     i, j, lpos, pos, nf, vers;
    char    *ptr, *cptr, *aptr, label[512], fullpath[1024];
    OSIZE   len, clen;
    KDB     *kdb = NULL, *tkdb = NULL;
    //OKDB643 *okdb643;
    //KOBJ    *kobj;
    FNAME   file;
    FILE    *fd;

    // Next line is deleted because K_load_odbc() was implemented for a specific project
    // if(U_is_in('!', fname)) return(K_load_odbc(ftype, fname, load_all, objs));

    // avoid runtime error 'i is not set' in line 'if(i == 0) K_free(kdb);' below
    i = 0;

    K_set_ext(file, fname, ftype);
    fd = fopen(file, "rb");

    if(fd == NULL) {
        kmsg("File %s does not exist", file);
        return(NULL);
    }
    //    setvbuf(fd, NULL, 0, 8192); 

    // CHECK OBJS LABEL 
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
        K_set_kdb_name(kdb, (unsigned char*) ptr);  // JMP 3/6/2015
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
            GzipDecodeStr((unsigned char*) cptr, clen, (unsigned char**) &aptr, (unsigned long*) &len);
            memcpy((char *)KOBJS(kdb), aptr, len);
            SW_nfree(cptr);
            SCR_free(aptr);
        }
    }

    // Si aucun nom n'est donné, on charge tout
    if(load_all == 0) {
        for(i = 0; i < KNB(kdb); i++) {
            if(K_read_len(fd, vers, &len)) goto error;

            if(len < 0) {
                // Si len < 0 => version zippée
                clen = len = -len;
                cptr = SW_nalloc(clen);
                kread(&len, sizeof(OSIZE), 1, fd);
                kread(cptr, clen, 1, fd);
                //LzhDecodeStr(cptr, clen, &aptr, &len);
                GzipDecodeStr((unsigned char*) cptr, clen, (unsigned char**) &aptr, (unsigned long*) &len);
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
            K_xdrobj[KTYPE(kdb)]((unsigned char*) ptr, NULL);
            K_setvers(kdb, i, vers);
        }
    }
    // Sinon on charge la liste demandée
    else  {
        // get data of specified objects 
        tkdb = K_create(KTYPE(kdb), KMODE(kdb));
        if(tkdb == NULL) goto error;

        memcpy(tkdb, kdb, sizeof(KDB));
        KNB(tkdb) = 0;
        KOBJS(tkdb) = NULL;

        nf = SCR_tbl_size((unsigned char**) objs);
        for(i = 0; i < nf; i++) K_add_entry(tkdb, objs[i]);

        lpos = 0;

        if(KTYPE(kdb) == VARIABLES || KTYPE(kdb) == SCALARS) {
            if(K_read_len(fd, vers, &len)) goto error;
        }

        for(i = 0; i < KNB(tkdb); i) {
            pos = K_find(kdb, KONAME(tkdb, i));
            if(pos < 0) {
                K_del_entry(tkdb, i);
                continue;
            }

            // skip this pos - lpos entries 
            if(kdb->k_compressed == 0 && (KTYPE(kdb) == VARIABLES || KTYPE(kdb) == SCALARS)) {
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
                GzipDecodeStr((unsigned char*) cptr, clen, (unsigned char**) &aptr, (unsigned long*) &len);
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

            K_xdrobj[KTYPE(tkdb)]((unsigned char*) ptr, NULL);
            K_setvers(tkdb, i, vers); // JMP 10-04-98 
            lpos = pos + 1;
            i++;
        }

        K_free_kdb(kdb); // no data in kdb 
        kdb = tkdb;
    }

    fclose(fd);
    return(kdb);

error:
    fclose(fd);
    if(load_all == 0) {
        if(i == 0) K_free(kdb);
        else { // JMP 31-01-00 
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


/**
 *  Retrieves infos on an IODE file: type, number of objects and, if defined, 
 *  the file description (free info on the file contents) and Sample (for var files only).
 *  
 *  @note: opens the IODE file to retrieve these informations from the file header, but does not 
 *  read the objects themselves.
 *  
 *  @param [in]  filename   char*   file to analyze
 *  @param [out] descr      char*   NULL or pointer to copy the description of the file
 *  @param [out] nobjs      int*    NULL or pointer to the number of objs in the file
 *  @param [out] smpl       Sample* NULL or pointer to the sample of the file (for VARIABLES only)
 *  @return                 int     on success: file type (FILE_COMMENTS...)
 *                                  on error: 
 *                                      -1 if filename is empty 
 *                                      -1 if IODE object version is < 0 or > 3
 *                                      -2 if file cannot be opened
 *  
 *  descr, nobjs and smpl may be NULL. In this case, nothing is copied.
 */
int K_filetype(char* filename, char* descr, int* nobjs, Sample* smpl)
{
    FILE* fd;
    int     vers;
    KDB     kdb;
    FNAME   file;
    char    label[80];

    if (descr) descr[0] = 0;
    if (nobjs) *nobjs = 0;
    if (smpl)  memset(smpl, 0, sizeof(Sample));
    if (filename == 0 ||
        filename[0] == 0 || filename[0] == '-') return(-1);

    strcpy(file, filename);
    K_strip(file); // JMP 19-04-98 
#ifdef DOS
    fd = fopen(file, "rb");
#else
    fd = fopen(file, "rb");
#endif
    if (fd == 0) return(-2); // file cannot be opened

    fread(label, strlen(K_LABEL), 1, fd);
    vers = K_calcvers(label);
    if (vers < 0 || vers > 3) {
        fclose(fd);
        return(-1);
    }

    K_read_kdb(&kdb, fd, vers);
    fclose(fd);

    if (descr) strcpy(descr, kdb.k_desc);
    if (nobjs) *nobjs = KNB(&kdb);
    if (smpl && kdb.k_type == VARIABLES)
        memcpy(smpl, KSMPL(&kdb), sizeof(Sample));

    return(kdb.k_type);
}


/**
 *  Given a filename and an expected type, try to retrieve the file type. 
 *  If the file cannot be open, try to open the file in which the extension is replaced
 *  by the standard iode extension for the object type (filename.var). 
 *  
 *  UPDATE 6.64: if the filename contains an extension and the file cannot be opened, the standard iode extension
 *  is not added. 
 *  
 *  @param [in] filename    char*   file to "inspect"
 *  @param [in] type        int     type expected
 *  @return                 int     on success file type (FILE_COMMENTS...)
 *                                  on error : -2 if file and file.ext cannot be opened
 *                                             -1 if the file type or the object version is not recognized 
 *  
 *  TODO: review this function! 
 */
static int K_findtype(char* filename, int type)
{
    int     ftype;
    char    buf[1024];

    ftype = K_filetype(filename, 0L, 0L, 0L); // Open the file to retrieve its content type
    
    if(type == VARIABLES && U_is_in('!', filename)) return(type); /* Give ODBC a try */

    if(ftype != -2) return(ftype); // -2 => file cannot be opened
    
    // Try the filename + std extension (e.g.: .var) 
    if(0) { // old version before 6.64: replacement extension even if an extension is present
        strcpy(buf, filename);
        K_add_ext(buf, k_ext[type]);
        return(K_filetype(buf, 0L, 0L, 0L));
    }
    else { // Change JMP 30/11/2022: try to add extension **only** if there is no extension in filename
        if(K_has_ext(filename)) return(-2); // file with extension (and not found)
        // file w/o extension => try a standard IODE extension
        strcpy(buf, filename);
        K_add_ext(buf, k_ext[type]);
        return(K_filetype(buf, 0L, 0L, 0L));
    }
    
}


/**
 *  Generalisation of K_load() : interprets the content of a file, ascii files included, 
 *  and tries to load its content into a KDB.
 *
 *  If the file is an ascii file (.ac, .ae... or .csv), the corresponding function 
 *      (*K_load_asc[type])() is called.
 *
 *  @param [in]   type       int     file type (FILE_COMMENTS,...ASCII_COMMENTS,..., FILE_CSV)
 *  @param [in]   filename   char*   file to load
 *  @return                  KDB*    KDB with the content of filename on success
 *                                   NULL on error
 *   
 *  @note Messages are sent to the user via calls to 2 functions which can be redefined 
 *       depending on the context (console app, window app, Qt app...): 
 *          - kmsg() for notifications
 *          - kerror() for error messages (TODO: check the use of ksmg on errors)
 *                                   
 */
KDB *K_interpret(int type, char* filename)
{
    KDB     *kdb = NULL;
    int     ftype;
    char    ext[10];

    kmsg("Loading %s", filename);
    ftype = K_findtype(filename, type);
//    printf("type=%d, ftype=%d\n", type, ftype);
    if(ftype == -1) { // file exists but when opened, type not recognized => test ascii contents
        if(type >= FILE_COMMENTS && type <= FILE_VARIABLES && 
            K_get_ext(filename, ext, 3) > 0 && strcmp(ext, k_ext[type + ASCII_COMMENTS]) == 0) {
                kdb = ascii_handlers[type]->load_asc(filename);
        }
    }
    
    if(kdb == NULL && type == ftype) 
        kdb = K_load(type, filename, 0, NULL); /* GB 23/01/98*/

    if(kdb == NULL) {
        kerror(0, "%s not found or incompatible type", filename);
        return(NULL);
    }

    if(kdb != NULL) kmsg("%d objects loaded", (int) KNB(kdb));

    return(kdb);
}


/**
 *  Copies a list of objects from an IODE object file to a KDB in memory. 
 *  Sub function of K_copy() acting on a single file.
 *  
 *  @param [in, out] to      KDB*       KDB destination 
 *  @param [in]      file    FNAME      filename
 *  @param [in]      no      int        number of object names in objs
 *  @param [in]      objs    char**     list of object names to copy
 *  @param [in, out] found   int*       indicates which names have been read (0 for still to read, 1 for already read) 
 *  @param [in]      smpl    Sample*    larger sample to read. 
 *                                      If the sample in file is smaller, only the common sample will be read.
 *  @return                             -1 on error 
 *                                      number of read objects on success. 
 *                                      0 if the vector "found" contains only ones or
 *                                      if none of the objects in objs are found in file.
 *  
 * @note Error codes are accumulated via a call to IodeErrorManager::append_error().
 *
 * TODO: refactor to, from...
 */
static int K_copy_1(KDB* to, FNAME file, int no, char** objs, int* found, Sample* smpl)
{
    int     i, pf, pt, rc = 0, nb_found = 0;
    KDB     *from;
    char    *ptr, *pack;
    Sample  csmpl;

    from = K_load(KTYPE(to), file, no, objs);
    if(from == NULL) return(-1);

    if(KTYPE(to) == VARIABLES) {
        if(smpl == NULL) 
            csmpl = *KSMPL(from);
        else
            csmpl = smpl->intersection(*KSMPL(from)); 

        if(rc < 0) {
            error_manager.append_error("File sample and copy sample do not overlap");
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


/**
 *  Reads objects from a list of IODE files and adds them to an existing KDB.
 *  If more than one file contains an object, the priority is given to the first file in the list.
 *  
 *  For Variables, if the sample in a file is smaller than the parameter smpl, the "intersection" sample is read.
 *  
 *  @param [in, out] to      KDB*       KDB destination (cannot be NULL)
 *  @param [in]      nf      int        number of files to read objects from
 *  @param [in]      files   char**     list of files to be read
 *  @param [in]      no      int        number of object names in objs (may not be 0)
 *  @param [in]      objs    char**     list of object names to copy (may not be 0)
 *  @param [in]      smpl    Sample*    larger sample to read. 
 *
 *  @return                             -1 on error: kdb is null, no list is given, one of the files does not exist
 *                                      -2 if not all object could be found
 *                                      0 if all objects have been found
 * @note Error codes are accumulated via a call to IodeErrorManager::append_error().
 */
int K_copy(KDB* kdb, int nf, char** files, int no, char** objs, Sample* smpl)
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
            if(found[i] == 0)
                error_manager.append_error(std::string("Var ") + std::string(objs[i]) + 
                                           " not found");
            j++;
        }
        if(j == 10) 
            error_manager.append_error("... others skipped");
        return(-2);
    }

    SW_nfree(found);
    return(0);
}




/**
 *  Concatenates the content of a file to an existing kdb.
 *  If ikdb is empty, the current ikdb filename is replaced by filename.
 *  
 *  TODO: check object types between file and ikdb.
 *  
 *  @param [in, out]    ikdb        KDB*    existing KDB where to copy the content of filename
 *  @param [in]         filename    char*   file containing objects to copy to ikdb
 *  @return                         int     -1 on error, 0 on success
 *              
 */
 
int K_cat(KDB* ikdb, char* filename)
{
    KDB     *kdb;
    //int     ftype;

    kdb = K_interpret(KTYPE(ikdb), filename);
    if(kdb == NULL) return(-1);

    if(KNB(ikdb) == 0) {
        memcpy(KDESC(ikdb), KDESC(kdb), K_MAX_DESC);
        //strcpy(KNAME(ikdb), KNAME(kdb));
        K_set_kdb_name(ikdb, (unsigned char*) KNAMEPTR(kdb)); // JMP 3/6/2015
    }
    if(KTYPE(ikdb) == VARIABLES) KV_merge_del(ikdb, kdb, 1);
    else K_merge_del(ikdb, kdb, 1);

    return(0);
}


static int K_BACKUP_ON_SAVE = 1; // Save a backup file ".??$" before saving a WS

/**
 *  Sets the backup choice before saving a kdb. 
 *  The backup file has the form "*.??$", e.g. "test.va$" for "test.var".
 *   
 *  @param [in] take_backup     int     0 if a backup should NOT be taken, any other value indicates that a backup is required
 *  @return                     int     0 for no backup, -1 for backup
 */
int K_set_backup_on_save(int take_backup) 
{
    K_BACKUP_ON_SAVE = (take_backup != 0);
    return(K_BACKUP_ON_SAVE);
}


/**
 *  Indicates if a backup must be taken before saving a kdb. 
 *  The backup file has the form "*.??$", e.g. "test.va$" for "test.var".
 *   
 *  @return                     int     0 for no backup, -1 for backup
 */
int K_get_backup_on_save() 
{
    return(K_BACKUP_ON_SAVE);
}


/**
 *  Takes a backup of a file by renaming the file: filename.xxx => filename.xx$. Function called before saving 
 *  an IODE object file.
 *   
 *  @param [in] filename    char*   file to backup
 *  @return                 int     0 on success, -1 on error    
 *  
 */
int K_backup(char* filename) 
{
    FNAME   backname;

    if(_access(filename, 0)) return(0);     
    strcpy(backname, filename);
    backname[strlen(backname) - 1] = '$';
    _unlink(backname);                      // JMP 15/5/2022 => Posix
    return(SCR_rename(filename, backname));
}

/**
 *  Local function to write binary data to an open binary stream (FILE*). If the param method is not null 
 *  and len * nb > minlen, compressed the data before writing it. 
 *  
 *  The size of the buffer is written before the data itself. To indicate that the buffer is compressed, the written size = -size.
 *  |10  |1234567890| => not compressed
 *  |-10 |azeazea|    => compressed
 *  
 *  @param [in] method      int     if not null, compress the data before writing
 *  @param [in] buf         char*   data to write
 *  @param [in] len         OSIZE   length of one element pointed to by buf
 *  @param [in] nb          int     number of elements in buf
 *  @param [in] fd          FILE*   binary output stream
 *  @param [in] minlen      int     if minlen > len * nb, don't compress, even if method is set to compress
 *  @return                 int     number of bytes written
 *  
 */
static int K_cwrite(int method, char* buf, OSIZE len, int nb, FILE* fd, int minlen)
{
    char    *cptr;
    OSIZE   clen;

    len *= nb;

    if(method == 0 || len < minlen) {
        kwrite(buf, len, 1, fd);
        return(len);
    }

    GzipEncodeStr((unsigned char*) buf, len, (unsigned char**) &cptr, (unsigned long*) &clen);
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


/**
 *  Save a KDB in an IODE object file. The extension of filename is replaced by the standard one (.cmt, .eqs...).
 *  
 *  A backup (file.xx$) is taken before saving.
 *  
 *  If the current processor architecture is "big-endian" (powerpc, motorola, Sun Sparc...), the KDB struct and the objects 
 *  are transposed into little-endian (see k_xdr.c).
 *  
 *  If the running architecture is X64, the KDB is translated into 32 bits (KDB32) before writing.
 *  
 *  if K_XDR is not null, the table of objects (KOBJS(kdb)) and the objects are individually compressed before being saved.
 *    
 *  @param [in, out]    kdb     KDB*    KDB to save 
 *  @param [in]         fname   FNAME   filename. 
 *  @param [in]         mode    int     1 to replace KNAMEPTR(kdb) by filename before saving
 *  @return                     int     -1 on error (kdb null, cannot create file)
 *                                      0 on success
 *  
 *  @note Messages are sent to the user via calls to 2 functions which can be redefined 
 *        depending on the context (console app, window app, Qt app...): 
 *           - kmsg() for notifications
 *           - kerror() for error messages (TODO: check the use of ksmg on errors)
 */

static int K_save_kdb(KDB* kdb, FNAME fname, int mode)
{
    int     i, len;
    char    *ptr, *xdr_ptr = NULL;
    //KOBJ    *kobj;
    KDB     *xdr_kdb = NULL;
    KDB32   kdb32;
    FNAME   file;
    FILE    *fd;

    if(kdb == NULL) return(-1);

    strcpy(file, fname);
    K_strip(file); /* JMP 21-04-98 */
    K_add_ext(file, k_ext[KTYPE(kdb)]);
    if(K_get_backup_on_save()) K_backup(file);     /* JMP 02-12-2022 */
    fd = fopen(file, "wb+");
    if(fd == NULL) return(-1);
    setvbuf(fd, NULL, 0, 8192);
    //kmsg("Saving %s", file); // JMP 11/01/2023 (msg already in calling function B_WsDump)

    if(mode) {
        K_set_kdb_name(kdb, (unsigned char*) file); // JMP 3/6/2015
        //strcpy(KNAME(kdb), file);
    }

    kwrite(K_LABEL, strlen(K_LABEL), 1, fd); /* JMP 23-05-00 */

    /* XDR  KDB */
    kdb->k_compressed = K_LZH; /* JMP 25-05-00 */
    K_xdrKDB(kdb, &xdr_kdb);

    // Dump KDB struct
    if(X64) 
    {
        /* convert to x64 if needed */
        memset(&kdb32, 0, sizeof(KDB32)); // JMP 7/2/2022
        kdb32.k_nb = xdr_kdb->k_nb;
        kdb32.k_type = xdr_kdb->k_type;
        kdb32.k_mode = xdr_kdb->k_mode;
        kdb32.k_compressed = xdr_kdb->k_compressed;
        memcpy(kdb32.k_arch, xdr_kdb->k_arch, LMAGIC);
        memcpy(kdb32.k_magic, xdr_kdb->k_magic, LMAGIC);
        memcpy(kdb32.k_oname, xdr_kdb->k_oname, OK_MAX_FILE); // JMP 03-06-2015 
        memcpy(kdb32.k_desc, xdr_kdb->k_desc, K_MAX_DESC);
        memcpy(kdb32.k_data, xdr_kdb->k_data, K_MAX_DESC);

        kwrite((char *) &kdb32, sizeof(KDB32), 1, fd);
    }
    else
        kwrite(xdr_kdb, sizeof(KDB), 1, fd);

    SW_nfree(xdr_kdb);

    // Dump KOBJ table
    if(K_cwrite(kdb->k_compressed, (char*)KOBJS(kdb), sizeof(KOBJ), KNB(kdb), fd, -1) < 0) goto error;

    // Dump objects
    for(i = 0; i < KNB(kdb); i++) {
        /* XDR  OBJ */
        ptr = SW_getptr(KOBJS(kdb)[i].o_val);
        len = P_len(ptr);
        K_xdrobj[KTYPE(kdb)]((unsigned char*) ptr, (unsigned char**) &xdr_ptr);
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


/**
 *  Save a KDB in an IODE object file. The extension of fname is replaced by the standard one (.cmt, .eqs...).
 *  A backup (file.xx$) is taken before saving.
 *  
 *  @see K_save_kdb() for more details.
 *    
 *  @param [in, out]    kdb     KDB*    KDB to save 
 *  @param [in]         fname   FNAME   filename. 
 *  @return                     int     -1 on error (kdb null, cannot create file)
 *                                      0 on success
 *  
 */

int K_save(KDB* kdb, FNAME fname)
{
    return(K_save_kdb(kdb, fname, 1));
}


/**
 *  Save a KDB in a IODE object file called "ws.<ext>" where <ext> is one of (.cmt, .eqs...).
 *  
 *  @see K_save_kdb() for more details.
 *    
 *  @param [in, out]    kdb     KDB*    KDB to save 
 *  @return                     int     -1 on error (kdb null, cannot create file)
 *                                      0 on success
 *  
 */

int K_save_ws(KDB* kdb)
{
    return(K_save_kdb(kdb, I_DEFAULT_FILENAME, 0));
}


/**
 *  Replaces KNAMEPTR(kdb) in an IODE object file.
 *  
 *  Opens the object file "from", replaces KNAMEPTR(kdb) by "to" (i.e. the name of the file in the struct) and resaves the KDB struct.
 *  
 *  Only used when renaming or copying a file.
 *  
 *  @param [in] from    char*   iode file name 
 *  @param [in] to      char*   new filename to put in the file from
 *  @return             int     0 on success
 *                              -1 on error (file not found, invalid IODE file)
 *  
 *  TODO: check if not obsolete.
 */
 
int K_setname(char* from, char* to)
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
    K_set_kdb_name(&kdb, (unsigned char*) to); // JMP 3/6/2015
    kseek(fd, (long) -1L * sizeof(KDB), 1);
    kwrite((char *) &kdb, sizeof(KDB), 1, fd);

    fclose(fd);
    return(0);

error:
    return(-1);
}

