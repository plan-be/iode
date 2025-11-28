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
 *      int K_filetype(char* filename, char* descr, int* nobjs, Sample* smpl)           retrieves infos on an IODE file: type, number of objects, Sample
 *      int X_findtype(char* filename)                                                  returns the type of content of filename according to its extension
 *      int K_copy(KDB* kdb, int nf, char** files, int no, char** objs, Sample* smpl)   reads a list of objects from a list of IODE object files and adds them to an existing KDB.
 *      int K_cat(KDB* ikdb, char* filename)                                            concatenates the content of a file to an existing kdb.
 *      int K_set_backup_on_save(int take_backup)                                       sets the backup choice before saving a kdb. 
 *      int K_get_backup_on_save()                                                      indicates if a backup must be taken before saving a kdb. 
 *      int K_backup(char* filename)                                                    takes a backup of a file by renaming the file: filename.xxx => filename.xx$.
 *      int K_setname(char* from, char* to)                                             replaces kdb->filepath in an IODE object file.
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


// required to read old binary files
struct KOBJ 
{
    SWHDL       o_val;          // Handle of the object in the scr4/swap memory -> to be passed to SW_getptr()
    ONAME       o_name;         // name of the object
};


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

static bool kseek(FILE* fd, long where, int how)
{
    int res = fseek(fd, where, how); 
    return res == 0;
}

static bool kwrite(void* buf, size_t elements_size, size_t elements_count, FILE* fd)
{
    size_t nb_elements_written = fwrite(buf, elements_size, elements_count, fd);
    return nb_elements_written == elements_count;
}

static bool kread(void* buf, size_t elements_size, size_t elements_count, FILE* fd)
{
    size_t nb_elements_read = fread(buf, elements_size, elements_count, fd);
    return nb_elements_read == elements_count;
}


/**
 *  Reads a "length" on the stream fd and then rewinds the stream. 
 *  The number of bytes depends on the IODE object version.
 *  
 *  For IODE object version 0, reads sizeof(OSIZE) bytes, 
 *  For IODE object versions 1 and 2, reads sizeof(short) bytes.
 *   
 *  
 *  @param [in]  fd     FD*     stream to read from 
 *  @param [in]  vers   int     version of IODE objects
 *  @param [out] len    OSIZE*  length read from the stream    
 *  @return             int     -1 if the bytes cannot be read from the stream
 *                              0 on success
 */
static int K_read_len(FILE* fd, int vers, OSIZE* len)
{
    OSIZE   os;
    U_sh    us;
    bool    success; 

    if(vers == 0 || vers == 3)  
    {
        success = fread(&os, (int)sizeof(OSIZE), 1, fd);
        if(!success) 
            return -1;
        
        K_xdrPLONG((unsigned char*) &os);
        success = kseek(fd, (long) (-1 * sizeof(OSIZE)), 1);
        if(!success) 
            return -1;
        
        *len = os;
    }
    else 
    {
        success = fread(&us, (int)sizeof(U_sh), 1, fd);
        if(!success) 
            return -1;
        
        K_xdrPINT((unsigned char*) &us);
        success = kseek(fd, (long) (-1 * sizeof(U_sh)), 1);
        if(!success) 
            return -1;
        
        *len = us;
    }

    return 0;
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
 *  @param [in]  fd       FILE*   IO stream
 *  @param [in]  vers     int     version of IODE objects (0-2 ?)
 *  @return               int     number of IODE objects to load
 */
int KDB::preload(FILE *fd, const std::string& filepath, const int vers)
{
    int     nb_objs = -1;
    OKDB643 *okdb643, kdb643;
    KDB32   kdb32;
    Sample* smpl;
    bool    success;

    okdb643 = &kdb643;
    memset((char*) &kdb643, 0, sizeof(kdb643)); // JMP 28/10/2016 correction plantage fichier 2000 de PV

    std::string error_msg = "preload(): Could not read header from input file '" + filepath + "'. ";
    switch(vers) 
    {
        case 0 :
            // Current version
            if(X64) 
            {
                success = kread((char *) &kdb32, sizeof(KDB32), 1, fd);
                if(!success)
                {
                    fclose(fd);
                    this->clear();
                    throw std::runtime_error(error_msg.c_str());
                }
                if(this->k_type == OBJECTS)
                    this->k_type = kdb32.k_type;
                if(this->k_type != kdb32.k_type)
                {
                    fclose(fd);
                    error_msg += "file type " + std::to_string(kdb32.k_type) + " does not match ";
                    error_msg += "the expected type " + std::to_string(this->k_type) + ".";
                    this->clear();
                    throw std::runtime_error(error_msg.c_str());
                }
                nb_objs = kdb32.k_nb;
                this->k_mode = kdb32.k_mode;
                this->k_compressed = kdb32.k_compressed;
                this->k_arch = std::string(kdb32.k_arch);
                this->description = std::string(kdb32.k_desc);
                if(kdb32.k_data != NULL && kdb32.k_data[0] != 0)
                {
                    smpl = (Sample*) kdb32.k_data;
                    this->sample = new Sample(*smpl);
                }
                else
                    this->sample = nullptr;
            }
            else
                success = kread((char *) this, sizeof(KDB), 1, fd);
                if(!success)
                {
                    fclose(fd);
                    this->clear();
                    throw std::runtime_error(error_msg.c_str());
                }
            break;
        case 1 :
        case 2 : // JMP 28/10/2016 correction plantage fichier 2000 (PV -> 504d) à vérifier
            // TODO: gérer la lecture des vers !=  si X64
            success = kread((char *) okdb643, sizeof(OKDB), 1, fd);
            if(!success)
            {
                fclose(fd);
                this->clear();
                throw std::runtime_error(error_msg.c_str());
            }
            break;
        case 3 :
            success = kread((char *) okdb643, sizeof(OKDB643), 1, fd);
            if(!success)
            {
                fclose(fd);
                this->clear();
                throw std::runtime_error(error_msg.c_str());
            }
            break;
    }

    if(vers != 0) 
    {
        if(this->k_type == OBJECTS)
            this->k_type = okdb643->k_type;
        if(this->k_type != okdb643->k_type)
        {
            fclose(fd);
            error_msg += "file type " + std::to_string(okdb643->k_type) + " does not match ";
            error_msg += "the expected type " + std::to_string(this->k_type) + ".";
            this->clear();
            throw std::runtime_error(error_msg.c_str());
        }
        nb_objs = okdb643->k_nb;
        this->k_mode = okdb643->k_mode;
        this->k_arch = std::string(okdb643->k_arch);
        this->description = std::string(okdb643->k_desc);
        if(okdb643->k_data != NULL && okdb643->k_data[0] != 0)
        {
            smpl = (Sample*) okdb643->k_data;
            this->sample = new Sample(*smpl);
        }
        else
            this->sample = nullptr;

        if(vers == 2 || vers == 3) 
            this->k_compressed = okdb643->k_compressed;
    }

    K_xdrKDB(this, NULL);
    return nb_objs;
}


static int index_of_name(const std::vector<std::string>& vec, const std::string& name)
{
    auto it = std::lower_bound(vec.begin(), vec.end(), name);
    if(it != vec.end())
        return (int) std::distance(vec.begin(), it);
    else
        return -1;
}


/**
 *  Loads an IODE binary object file. 
 *  
 *  @note Prefer load() to load ascii or binary files.
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
 * @param [in]   db_global  int     1 for DB_GLOBAL, 0 for DB_STANDALONE
 * @return                  KDB*    KDB with the content of filename restricted to objs if objs is not null
 *                                  NULL on error
 *  
 * @note Messages are sent to the user via calls to 2 functions which must be defined 
 *       according to the context (console app, window app, Qt app...) : 
 *          - kmsg() for notifications
 *          - kerror() for error messages (TODO: check the use of ksmg on errors)                                 
 */
bool KDB::load_binary(const int file_type, const std::string& filename, 
                      const std::vector<std::string>& objs)
{
    int     pos, nf, vers;
    char    *ptr, *cptr, *aptr, label[512], fullpath[1024];
    OSIZE   len, clen;
    KDB*    tkdb = nullptr;
    FNAME   file;
    FILE*   fd;
    SWHDL   handle;
    int     nb_objs;
    int     k_objs_size;
    KOBJ*   k_objs = NULL;
    bool    success;
    std::string error_msg;
    std::string name;
    std::vector<std::string> v_names;

    // avoid runtime error 'i is not set' in line 'if(i == 0) delete kdb;' below
    int i = 0;

    std::string filename_trim = trim(filename);
    char* fname = const_cast<char*>(filename_trim.c_str());
    K_set_ext(file, fname, file_type);

    std::string prefix_error_msg = "Error when loading binary file " + std::string(file) + ":\n";

    if(!this)
    {
        error_msg = prefix_error_msg + "Constructor of '" + v_iode_types[file_type]; 
        error_msg += "' must be called first";
        kwarning(error_msg.c_str());
        return false;
    }

    fd = fopen(file, "rb");

    if(fd == NULL) 
    {
        error_msg = prefix_error_msg + "File does not exist.";
        kwarning(error_msg.c_str());
        return false;
    }

    // CHECK OBJS LABEL 
    success = kread(label, strlen(K_LABEL), 1, fd);
    if(!success)
    {
        fclose(fd);
        error_msg = prefix_error_msg + "Cannot read IODE file version";
        kwarning(error_msg.c_str());
        return false;
    }

    vers = K_calcvers(label);
    if(vers < 0 || vers > 2) 
    {
        fclose(fd);
        error_msg = prefix_error_msg + "Unrecognized IODE file version.";
        kwarning(error_msg.c_str());
        return false;
    }

    // Read kdb struct from open file (fd) and transpose into kdb 64 if X64
    nb_objs = this->preload(fd, std::string(file), vers);

    if(file_type != this->k_type || this->k_arch != ARCH) 
    {
        fclose(fd);
        error_msg = prefix_error_msg + "Unvalid type (" + std::string(k_ext[this->k_type]) + ") or wrong ";
        error_msg += "CPU architecture (" + this->k_arch + ")";
        kwarning(error_msg.c_str());
        return false;
    }

    // Set filename in this->filepath
    if(strncmp(file, "ws.", 3)) 
    {
        ptr = SCR_fullpath(file, fullpath);
        if(ptr == 0) 
            ptr = file;
        this->filepath = std::string(ptr);
    }

    // ======== Load objects ========

    // -------- load names --------
    // check binary file version (1 and 2 -> old versions with short names)
    error_msg = prefix_error_msg + "Could not read object names";
    if(vers == 1 || vers == 2) 
    {
        // OKOBJ -> short names
        OKOBJ* okobj = (OKOBJ *) SW_nalloc(sizeof(OKOBJ) * nb_objs);
        success = kread((char *) okobj, sizeof(OKOBJ), nb_objs, fd);
        if(!success)
        {
            fclose(fd);
            SW_nfree(okobj);
            kwarning(error_msg.c_str());
            return false;
        }
        v_names.reserve(nb_objs);
        for(int j = 0; j < nb_objs; j++)
            v_names.push_back(std::string(okobj[j].o_name));
        SW_nfree(okobj);
    }
    else 
    {
        k_objs_size = (nb_objs / K_CHUNCK + 1) * K_CHUNCK;
        k_objs = (KOBJ *) SW_nalloc(sizeof(KOBJ) * k_objs_size);
        if(k_objs == NULL)
        {
            fclose(fd);
            error_msg += ".\nFailed allocating string array to store object names";
            kwarning(error_msg.c_str());
            return false;
        }

        // no compression
        if(this->k_compressed == 0) 
        {
            // load names
            success = kread((char *) k_objs, sizeof(KOBJ), (int) nb_objs, fd);
            if(!success)
            {
                fclose(fd);
                SW_nfree(k_objs);
                kwarning(error_msg.c_str());
                return false;
            }
        }
        // compression
        else 
        {
            if(K_read_len(fd, vers, &len))
            {
                fclose(fd);
                SW_nfree(k_objs);
                error_msg += "Could not read object names";
                kwarning(error_msg.c_str());
                return false;
            }
            clen = len = -len;
            cptr = SW_nalloc(clen);
            success = kread(&len, sizeof(OSIZE), 1, fd);
            if(!success) 
            {
                fclose(fd);
                SW_nfree(k_objs);
                kwarning(error_msg.c_str());
                return false;
            }
            success = kread(cptr, clen, 1, fd);
            if(!success) 
            {
                fclose(fd);
                SW_nfree(k_objs);
                kwarning(error_msg.c_str());
                return false;
            }
            //LzhDecodeStr(cptr, clen, &aptr, &len);
            GzipDecodeStr((unsigned char*) cptr, clen, (unsigned char**) &aptr, (unsigned long*) &len);
            memcpy((char *) k_objs, aptr, len);
            SW_nfree(cptr);
            SCR_free(aptr);
        }

        v_names.reserve(nb_objs);
        for(i = 0; i < nb_objs; i++) 
            v_names.push_back(std::string(k_objs[i].o_name));

        SW_nfree(k_objs);
    }

    // -------- load values --------
    // load all objects
    if(objs.size() == 0)
    {
        for(i = 0; i < nb_objs; i++) 
        {
            name = v_names[i];
            error_msg = prefix_error_msg + "Could not read object '" + name + "'";

            if(K_read_len(fd, vers, &len)) 
                goto error;

            if(len < 0) 
            {
                // Si len < 0 => version zippée
                clen = len = -len;
                cptr = SW_nalloc(clen);
                success = kread(&len, sizeof(OSIZE), 1, fd);
                if(!success) 
                    goto error;
                success = kread(cptr, clen, 1, fd);
                if(!success) 
                    goto error;
                //LzhDecodeStr(cptr, clen, &aptr, &len);
                GzipDecodeStr((unsigned char*) cptr, clen, (unsigned char**) &aptr, (unsigned long*) &len);
                handle = SW_alloc(len);
                this->k_objs[name] = handle;
                ptr = SW_getptr(handle);
                memcpy(ptr, aptr, len);
                SW_nfree(cptr);
                SCR_free(aptr);
            }
            else 
            {
                // Si len >= 0 => version non zippée (dft)
                handle = SW_alloc(len);
                this->k_objs[name] = handle;
                ptr = SW_getptr(handle);
                if(ptr == 0) 
                {
                    error_msg += ".\nMemory full when allocating object '" + name + "':\n";
                    error_msg += "o_val = " + std::to_string(handle);
                    error_msg += ", block: " + std::to_string(SW_get_blk(handle));
                    goto error;
                }

                success = kread(ptr, len, 1, fd);
                if(!success) 
                    goto error;
            }
            K_xdrobj[this->k_type]((unsigned char*) ptr, NULL);
            K_setvers(this, i, vers);
        }
    }
    // load only the objects in the list objs
    else  
    {
        nf = (int) objs.size();

        // check names in the objs list exist in the binary file
        std::vector<int> v_pos(nf, -1);
        for(i = 0; i < nf; i++) 
        {
            pos = index_of_name(v_names, objs[i]);
            if(pos >= 0) 
                v_pos[i] = pos;
        }
        
        // ???
        if(this->k_type == VARIABLES || this->k_type == SCALARS)
        {
            if(K_read_len(fd, vers, &len))
            {
                error_msg = prefix_error_msg + "Could not read object values";
                goto error;
            }
        }
        
        // read the objects one by one
        int j;
        int lpos = 0;
        char* c_name;
        for(int n = 0; n < nf; n++) 
        {
            c_name = (char*) objs[n].c_str();
            pos = v_pos[n];
            if(pos < 0) 
                continue;

            error_msg = prefix_error_msg + "Could not read object '" + objs[n] + "'";

            // skip the pos - lpos entries in the binary file 
            if(this->k_compressed == 0 && (this->k_type == VARIABLES || this->k_type == SCALARS)) 
            {
                if(pos - lpos > 0)
                {
                    success = kseek(fd, (long) len * (pos - lpos), 1);
                    if(!success)
                        goto error;
                }
            }
            else 
            {
                for(j = 0; j < pos - lpos; j++) 
                {
                    if(K_read_len(fd, vers, &len)) 
                        goto error;
                    if(len < 0) 
                        len = -len + sizeof(OSIZE);
                    success = kseek(fd, (long) len, 1);
                    if(!success)
                        goto error;
                }
            }

            // read next object
            if(K_read_len(fd, vers, &len)) 
                goto error;
            
            name = std::string(c_name);
            handle = SW_alloc(len);
            this->k_objs[name] = handle;
            ptr = SW_getptr(handle);
            if(ptr == 0) 
            {
                error_msg += ".\nMemory full when allocating object '" + name + "':\n";
                error_msg += "o_val = " + std::to_string(handle);
                error_msg += ", block: " + std::to_string(SW_get_blk(handle));
                goto error;
            }

            if(len < 0) 
            {
                clen = len = -len;
                cptr = SW_nalloc(clen);
                success = kread(&len, sizeof(OSIZE), 1, fd);
                if(!success) 
                    goto error;
                success = kread(cptr, clen, 1, fd);
                if(!success) 
                    goto error;
                //LzhDecodeStr(cptr, clen, &aptr, &len);
                GzipDecodeStr((unsigned char*) cptr, clen, (unsigned char**) &aptr, (unsigned long*) &len);
                memcpy(ptr, aptr, len);
                SW_nfree(cptr);
                SCR_free(aptr);
            }
            else
            {
                success = kread(ptr, len, 1, fd);
                if(!success) 
                    goto error;
            }

            K_xdrobj[this->k_type]((unsigned char*) ptr, NULL);
            K_setvers(this, i, vers);
            lpos = pos + 1;
        }
    }

    fclose(fd);
    return true;

error:
    fclose(fd);
    this->clear();
    if(objs.size() == 0 && tkdb != nullptr)
    {
        delete tkdb;
        tkdb = nullptr;
    }

    error_msg += ".\nReset database '" + v_iode_types[this->k_type] + "'.";
    kwarning(error_msg.c_str());
    return false;
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
    FILE*   fd;
    int     vers;
    FNAME   file;
    char    label[80];
    int     nb_objs;

    if (descr) descr[0] = 0;
    if (nobjs) *nobjs = 0;
    if (smpl) memset(smpl, 0, sizeof(Sample));
    if (filename == 0 || filename[0] == 0 || filename[0] == '-') 
        return(-1);

    strcpy(file, filename);
    K_strip(file); // JMP 19-04-98 
    fd = fopen(file, "rb");
    if (fd == 0) 
        return(-2); // file cannot be opened

    fread(label, strlen(K_LABEL), 1, fd);
    vers = K_calcvers(label);
    if (vers < 0 || vers > 3) 
    {
        fclose(fd);
        return(-1);
    }

    KDB* kdb = new KDB(OBJECTS, DB_STANDALONE);
    nb_objs = kdb->preload(fd, std::string(file), vers);
    fclose(fd);

    if(descr) 
        strcpy(descr, (char*) kdb->description.c_str());
    if(nobjs) 
        *nobjs = nb_objs;
    if(smpl != NULL && kdb->k_type == VARIABLES)
    {
        if(kdb->sample)
            smpl = new Sample(*kdb->sample);
        else
        {
            delete smpl;
            smpl = nullptr;
        }
    }

    int type = kdb->k_type;
    delete kdb;
    kdb = nullptr;
    return type;
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
int K_findtype(char* filename, int type)
{
    int     ftype;
    char    buf[1024];

    ftype = K_filetype(filename, 0L, 0L, 0L); // Open the file to retrieve its content type
    
    if(type == VARIABLES && U_is_in('!', filename)) 
        return(type); /* Give ODBC a try */

    if(ftype != -2) 
        return(ftype); // -2 => file cannot be opened
    
    // Try the filename + std extension (e.g.: .var) 
    
    // old version before 6.64: replacement extension even if an extension is present
    // strcpy(buf, filename);
    // K_add_ext(buf, k_ext[type]);
    // return(K_filetype(buf, 0L, 0L, 0L));
    
    // Change JMP 30/11/2022: try to add extension **only** if there is no extension in filename
    if(K_has_ext(filename)) 
        return(-2); // file with extension (and not found)
    // file w/o extension => try a standard IODE extension
    strcpy(buf, filename);
    K_add_ext(buf, k_ext[type]);
    return(K_filetype(buf, 0L, 0L, 0L));
}


/**
 *  Generalization of load() : interprets the content of a file, ascii files included, 
 *  and tries to load its content into a KDB.
 *
 *  If the file is an ascii file (.ac, .ae... or .csv), the corresponding function 
 *      (*K_load_asc[type])() is called.
 *
 *  @param [in]   type       int     file type (FILE_COMMENTS,...ASCII_COMMENTS,..., FILE_CSV)
 *  @param [in]   filename   char*   file to load
 *  @param [in]   db_global  int     1 for DB_GLOBAL, 0 for DB_STANDALONE
 *  @return                  KDB*    KDB with the content of filename on success
 *                                   NULL on error
 *   
 *  @note Messages are sent to the user via calls to 2 functions which can be redefined 
 *       depending on the context (console app, window app, Qt app...): 
 *          - kmsg() for notifications
 *          - kerror() for error messages (TODO: check the use of ksmg on errors)                              
 */
bool KDB::load(const std::string& filename)
{
    int   ftype;
    char  ext[10];
    KDB*  kdb = nullptr;
    bool  success = false;

    kmsg("Loading %s", filename.c_str());

    ftype = K_findtype((char*) filename.c_str(), this->k_type);
    if(ftype == -1) 
    {   
        // test if correct ascii extension
        int ext_length = K_get_ext((char*) filename.c_str(), ext, 3);
        if(ext_length > 0 && strcmp(ext, k_ext[this->k_type + ASCII_COMMENTS]) == 0) 
            success = this->load_asc(filename);
        
        if(success) 
            this->filepath = filename;
    }
    
    // NOTE: this->filepath updated in load_binary() 
    if(this->k_type == ftype) 
        success = this->load_binary(this->k_type, filename);

    if(success)
        kmsg("%d objects loaded", (int) this->size());
    
    return success;
}


/**
 *  Returns the type of content of filename according to its extension:
 *    - 0 -> 6 = .cmt, ..., .var (binary format)
 *    - 10 -> 16 = ac, ... av
 *    - 26 = csv
 *    - 22 = rep (TODO: check this)
 *    - autres formats (rep, a2m, ..., csv)
 *    - -1 -> undefined
 *  
 *  @global     char**  k_ext       recognized extensions
 *  @param [in] char*   filename    file to analyse
 *  @return     int                 file type (see above)
 */
int X_findtype(char* filename)
{
    int         i, lg = (int)strlen(filename);
    char        buf[5];

    // Check std extensions .cmt => .var
    if(lg > 4) 
    {
        for(i = 0 ; i < 7 ; i++) 
        {
            if(filename[lg - 4] == '.' &&
                    SCR_cstrcmp((unsigned char*) k_ext[i], ((unsigned char*) filename) + lg - 3) == 0) return(i);
        }
    }

    // Check ascii extensions .ac => .av
    if(lg > 3) 
    {
        strcpy(buf, ".ac");
        for(i = 0 ; i < 7 ; i++) 
        {
            buf[2] = k_ext[i][0];
            if(SCR_cstrcmp(((unsigned char*) filename) + lg - 3, (unsigned char*) buf) == 0) return(10 + i);
        }
    }

    // Other extensions
    if(lg > 4 && SCR_cstrcmp(((unsigned char*) filename) + lg - 4, (unsigned char*) ".csv") == 0) return(FILE_CSV); // Correction JMP 25/3/2019
    if(lg > 4 && SCR_cstrcmp(((unsigned char*) filename) + lg - 4, (unsigned char*) ".rep") == 0) return(22); // ??? pas trés cohérent...

    // Sais plus a quoi ca peut servir... => a supprimer
    for(i = 16 ; strcmp(k_ext[i], "xxx") !=0 ; i++) 
    {
        if(lg > 4 && SCR_cstrcmp(((unsigned char*) filename) + lg - 4, (unsigned char*) k_ext[i]) == 0) return(i); // Correction JMP 16/1/2019 : lg - 4 au lieu de -3
    }

    return(-1);
}


/**
 *  Saves the content of KDB in a file: internal IODE format, ASCII IODE format or CSV. 
 *  The extension of the filename determines if the output file must be in internal, 
 *  ASCII or CSV format.
 *  
 *  @param [in] std::string   filename    filename with extension
 */
bool KDB::save(const std::string& filename)
{
    int ftype;
    bool success = false;
    kmsg("Saving %s", filename.c_str()); 

    ftype = X_findtype((char*) filename.c_str());
    if(ftype >= 10 && ftype <= 17)
        success = this->save_asc(filename);
    else if(ftype <= 6)
        success = this->save_binary(filename);
    else if(ftype == FILE_CSV)
        success = this->save_vars_csv(filename);

    return success;
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
    bool    obj_found;
    int     i, rc = 0, nb_found = 0;
    bool    success;
    char    *ptr, *pack;
    SWHDL   handle_to;
    Sample  csmpl;
    std::string name;

    KDB* from = new KDB((IodeType) to->k_type, DB_STANDALONE);
    if(!from) 
        return(-1);
    
    std::vector<std::string> v_objs;
    for(i = 0; i < no; i++) 
        v_objs.push_back(std::string(objs[i]));

    from->load_binary(to->k_type, file, v_objs);

    if(to->k_type == VARIABLES) 
    {
        if(smpl == NULL) 
            csmpl = *from->sample;
        else
            csmpl = smpl->intersection(*from->sample);

        if(rc < 0) 
        {
            error_manager.append_error("File sample and copy sample do not overlap");
            goto the_end;
        }

        /* delete already found variables */
        for(i = 0 ; i < no; i++) 
        {
            name = std::string(objs[i]);
            obj_found = from->contains(name);
            if(!obj_found) 
                continue;
            if(found[i]) 
                from->remove(name);
            found[i] = 1;
        }

        rc = KV_sample(from, &csmpl);
        if(rc < 0) 
            goto the_end;
        nb_found = from->size();
        rc = KV_merge(to, from, 1);
    }
    else 
    {
        for(i = 0 ; i < no; i++) 
        {
            name = std::string(objs[i]);
            if(found[i]) 
                continue;
            obj_found = from->contains(name);
            if(!obj_found) 
                continue;

            found[i] = 1;

            // copy packed object from "from" to char* pack
            ptr = from->get_ptr_obj(name);
            pack = SW_nalloc(P_len(ptr));
            memcpy(pack, ptr, P_len(ptr));

            // check if the object already exists in "to"
            handle_to = to->get_handle(name);
            // if yes -> delete the corresponding object
            if(handle_to > 0)
                SW_free(handle_to);
            // if not -> create a new entry
            else 
                success = to->add_entry(objs[i]);
                if(!success) 
                {
                    SW_nfree(pack);
                    std::string msg = "Cannot add entry '" + name + "' to KDB";
                    error_manager.append_error(msg);
                    goto the_end;
                }

            // allocate memory in "to" and copy the packed object
            handle_to = SW_alloc(P_len(pack));
            to->k_objs[name] = handle_to;
            memcpy(SW_getptr(handle_to), pack, P_len(pack));

            SW_nfree(pack);
            nb_found++;
        }
    }

the_end:
    delete from;
    from = nullptr;
    
    if(rc < 0) 
        return(0);
    else 
        return nb_found;
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

    if(!kdb) 
        return(-1);
    if(objs == NULL || no == 0) 
        return(0);

    found = (int*) SW_nalloc(no * sizeof(int));
    if(files == NULL || nf == 0) 
        goto fin;

    for(i = 0; i < nf && nb_found < no; i++) 
    {
        nb = K_copy_1(kdb, files[i], no, objs, found, smpl);
        if(nb < 0) 
        {
            SW_nfree(found);
            return(-1);
        }
        nb_found += nb;
    }

fin:
    if(nb_found < no) 
    {
        for(i = 0, j = 0 ; i < no && j < 10; i++) 
        {
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
    KDB* kdb = new KDB((IodeType) ikdb->k_type, DB_STANDALONE);
    bool success = kdb->load(std::string(filename));
    if(!success)
    {
        delete kdb;
        return -1;
    }

    if(ikdb->size() == 0) 
    {
        ikdb->description = kdb->description;
        ikdb->filepath = kdb->filepath;
    }

    if(ikdb->k_type == VARIABLES) 
        KV_merge_del(ikdb, kdb, 1);
    else 
        K_merge_del(ikdb, kdb, 1);

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
    char* cptr;
    OSIZE clen;
    bool  success;

    len *= nb;

    if(method == 0 || len < minlen) 
    {
        success = kwrite(buf, len, 1, fd);
        if(!success)
            return -1;
        
        return len;
    }

    GzipEncodeStr((unsigned char*) buf, len, (unsigned char**) &cptr, (unsigned long*) &clen);
    if(minlen > 0 && len <= clen + sizeof(OSIZE)) 
    {
        success = kwrite(buf, len, 1, fd);
        SCR_free(cptr);
        if(!success)
            return -1;
        
        clen = -len;
    }
    else 
    {
        clen = -clen;
        success = kwrite(&clen, sizeof(OSIZE), 1, fd);
        if(!success)
            return -1;
        success = kwrite(cptr, -clen, 1, fd);
        SCR_free(cptr);
        if(!success)
            return -1;
    }

    return -clen;
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
 *  if K_XDR is not null, the table of objects (kdb->k_objs) and the objects are individually compressed before being saved.
 *  
 *  @param [in]         fname               FNAME   filename. 
 *  @param [in]         override_filepath   bool    true to replace kdb->filepath by filename before saving
 *  
 *  @note Messages are sent to the user via calls to 2 functions which can be redefined 
 *        depending on the context (console app, window app, Qt app...): 
 *           - kmsg() for notifications
 *           - kerror() for error messages (TODO: check the use of ksmg on errors)
 */

bool KDB::save_binary(const std::string& filename, const bool override_filepath)
{
    int     i, len, res;
    char    *ptr, *xdr_ptr = NULL;
    KDB*    xdr_kdb = nullptr;
    KDB32   kdb32;
    FNAME   file;
    FILE*   fd;
    KOBJ*   k_objs;
    bool    success;

    strcpy(file, filename.c_str());
    K_strip(file);
    K_add_ext(file, k_ext[this->k_type]);

    std::string error_msg = "Failed to save database '" + v_iode_types[this->k_type] + "' ";
    error_msg += "to the file '" + std::string(file) + "'";

    if(K_get_backup_on_save()) 
        K_backup(file);
    
    fd = fopen(file, "wb+");
    if(fd == NULL)
    {
        error_msg += ":\nCannot open file for writing";
        kwarning(error_msg.c_str());
        return false;
    } 
    
    setvbuf(fd, NULL, 0, 8192);

    if(override_filepath) 
        this->filepath = std::string(file);

    success = kwrite(K_LABEL, strlen(K_LABEL), 1, fd);
    if(!success)
    {
        fclose(fd);
        error_msg += ":\nCannot write IODE file version";
        kwarning(error_msg.c_str());
        return false;
    }

    /* XDR  KDB */
    this->k_compressed = K_LZH;
    K_xdrKDB(this, &xdr_kdb);

    // Dump KDB struct
    if(X64) 
    {
        /* convert to x64 if needed */
        memset(&kdb32, 0, sizeof(KDB32));
        kdb32.k_nb = xdr_kdb->size();
        kdb32.k_type = xdr_kdb->k_type;
        kdb32.k_mode = xdr_kdb->k_mode;
        kdb32.k_compressed = xdr_kdb->k_compressed;
        memset(kdb32.k_arch, 0, LMAGIC);
        std::strncpy(kdb32.k_arch, xdr_kdb->k_arch.c_str(), xdr_kdb->k_arch.size());
        memset(kdb32.k_desc, 0, K_MAX_DESC);
        std::strncpy(kdb32.k_desc, (char*) xdr_kdb->description.c_str(), xdr_kdb->description.size());
        if(xdr_kdb->sample)
        {
            char* c_smpl = (char*) xdr_kdb->sample; 
            memcpy(kdb32.k_data, c_smpl, sizeof(Sample));
        }

        success = kwrite((char *) &kdb32, sizeof(KDB32), 1, fd);
        if(!success)
        {
            fclose(fd);
            error_msg += ":\nCannot write IODE file header";
            kwarning(error_msg.c_str());
            return false;
        }
    }
    else
        success = kwrite(xdr_kdb, sizeof(KDB), 1, fd);
        if(!success)
        {
            fclose(fd);
            error_msg += ":\nCannot write IODE file header";
            kwarning(error_msg.c_str());
            return false;
        }

    delete xdr_kdb;
    xdr_kdb = nullptr;

    // prepare KOBJ table
    i = 0;
    k_objs = new KOBJ[this->size()];
    for(auto& [name, handle] : this->k_objs) 
    {
        strcpy(k_objs[i].o_name, name.c_str());
        k_objs[i].o_val = handle;
        i++;
    }

    // dump KOBJ table -> dump pairs of (name, handle)
    res = K_cwrite(this->k_compressed, (char*) k_objs, sizeof(KOBJ), this->size(), fd, -1);
    if(res < 0) 
    {
        error_msg += ":\nCannot write object data";
        kwarning(error_msg.c_str());
        fclose(fd);
        return false;
    }

    delete[] k_objs;

    // dump object values as packed objects
    for(auto& [name, handle] : this->k_objs) 
    {
        ptr = SW_getptr(handle);
        len = P_len(ptr);
        K_xdrobj[this->k_type]((unsigned char*) ptr, (unsigned char**) &xdr_ptr);
        res = K_cwrite(this->k_compressed, xdr_ptr, len, 1, fd, 20);
        SW_nfree(xdr_ptr);
        if(res < 0) 
        {
            error_msg += ":\nCannot write object '" + name + "'";
            kwarning(error_msg.c_str());
            fclose(fd);
            return false;
        }
    }

    kmsg("%d objects saved", (int) this->size());
    fclose(fd);
    return true;
}


/**
 *  Replaces kdb->filepath in an IODE object file.
 *  
 *  Opens the object file "from", replaces kdb->filepath by "to" (i.e. the name of the file in the struct) and resaves the KDB struct.
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
    char  label[512];
    FILE* fd;
    KDB*  kdb = nullptr;
    bool  success;

    fd = fopen(from, "rb+");
    if(fd == NULL)  
        return -1;

    std::string error_msg = "Failed to update the 'filename' in IODE file " + std::string(from);

    success = kread(label, strlen(K_LABEL), 1, fd);
    if(!success) 
    {
        error_msg += ": cannot read IODE file version";
        kwarning(error_msg.c_str());
        fclose(fd);
        return -1;
    }

    if(K_calcvers(label) < 0) 
    {
        error_msg += ": cannot read IODE file version";
        kwarning(error_msg.c_str());
        fclose(fd);
        return -1;
    }

    error_msg = "Failed to update the 'filename' in IODE file " + std::string(from);

    success = kread((char *) kdb, sizeof(KDB), 1, fd);
    if(!success) 
    {
        kwarning(error_msg.c_str());
        fclose(fd);
        return -1;
    }

    kdb->filepath = std::string(to);

    success = kseek(fd, (long) (-1 * sizeof(KDB)), 1);
    if(!success)
        return -1;
    
    success = kwrite((char *) kdb, sizeof(KDB), 1, fd);
    if(!success)
    {
        kwarning(error_msg.c_str());
        fclose(fd);
        return -1;
    }

    fclose(fd);
    return 0;
}
