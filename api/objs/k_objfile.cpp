#ifndef UNIX
#include <io.h>
#endif
#include <filesystem>   // requires C++17 

#include "scr4/s_swap.h"        // SWHDL
#include "scr4/s_prodir.h"

#include "api/constants.h"
#include "api/k_super.h"
#include "api/b_errors.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/ascii/ascii.h"

#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"
#include "api/objs/old_structs.h"
#include "api/objs/structs_32.h"


bool KDB::backup_enabled = true;

// required to read old binary files
struct KOBJ 
{
    SWHDL o_val;    // Handle of the object in the scr4/swap memory -> to be passed to SW_getptr()
    ONAME o_name;   // name of the object
};


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
 *  Reads the header information of an IODE file and fills the KDBInfo attributes with 
 *  this information. The number of objects and the 64 bytes 'data' are returned as a pair. 
 *  
 *  @note The previous versions of the KDB struct are OKDB and OKDB643.
 *        The 32 bit version of KDB (when in 64 bits executable) is KDB32.
 *      
 *  Warning: IODE objects versions 1 and 2 are not implemented in 64 bits !
 *  TODO: implement version 1 and 2 in 64 bits.
 *  
 *  @param [in]  fd       FILE*   IO stream
 *  @param [in]  vers     int     version of IODE objects
 */
std::pair<int, char[64]> KDBInfo::_preload_(FILE *fd, const std::string& filepath, const int vers)
{
    int nb_objs = -1;
    char data[64] = {0};
    OKDB643 kdb643;
    KDB32   kdb32;
    bool    success;

    OKDB643* okdb643 = &kdb643;
    memset((char*) &kdb643, 0, sizeof(kdb643));

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
                    memcpy(data, kdb32.k_data, 64);
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
            memcpy(data, okdb643->k_data, 64);
        if(vers == 2 || vers == 3) 
            this->k_compressed = okdb643->k_compressed;
    }

    std::pair<int, char[64]> res;
    res.first = nb_objs;
    memcpy(res.second, data, 64);
    return res;
}

int KDBInfo::preload(FILE *fd, const std::string& filepath, const int vers)
{
    std::pair<int, char[64]> res = _preload_(fd, filepath, vers);
    return res.first;
}

int KDBVariables::preload(FILE *fd, const std::string& filepath, const int vers)
{
    std::pair<int, char[64]> res = _preload_(fd, filepath, vers);

    char* data = res.second;
    if(data[0] != 0)
    {
        Sample* smpl = (Sample*) data;
        this->sample = new Sample(*smpl);
    }

    return res.first;
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
 * @param [in]   filename   string   filename
 * @param [in]   objs       char**   null or list of objects to load
 *  
 * @note Messages are sent to the user via calls to 2 functions which must be defined 
 *       according to the context (console app, window app, Qt app...) : 
 *          - kmsg() for notifications
 *          - kerror() for error messages (TODO: check the use of ksmg on errors)                                 
 */
bool KDB::load_binary(const std::string& filename, const std::vector<std::string>& objs)
{
    int     pos, nf;
    char    *ptr, *cptr, *aptr, label[512], fullpath[1024];
    OSIZE   len, clen;
    KDB*    tkdb = nullptr;
    FNAME   file;
    FILE*   fd;
    int     nb_objs;
    int     k_objs_size;
    KOBJ*   k_objs = NULL;
    bool    success;
    std::string error_msg;
    std::string name;
    std::vector<std::string> v_names;

    // avoid runtime error 'i is not set' in line 'if(i == 0) delete kdb;' below
    int i = 0;

    std::string prefix_error_msg = "Error when loading binary file " + std::string(file) + ":\n";

    std::string filename_trim = trim(filename);
    std::string file_ext = set_file_extension(filename_trim, (int) this->k_type);
    strcpy(file, file_ext.c_str());

    fd = fopen(file, "rb");
    if(!fd) 
    {
        error_msg = prefix_error_msg + "File does not exist.";
        kwarning(error_msg.c_str());
        return false;
    }

    // CHECK OBJS LABEL 
    memset(label, 0, 512);
    success = kread(label, strlen(K_LABEL), 1, fd);
    if(!success)
    {
        fclose(fd);
        error_msg = prefix_error_msg + "Cannot read IODE file version";
        kwarning(error_msg.c_str());
        return false;
    }

    int version = get_version(label);
    if(version < 0 || version > 2) 
    {
        fclose(fd);
        error_msg = prefix_error_msg + "Unrecognized IODE file version.";
        kwarning(error_msg.c_str());
        return false;
    }

    // Read kdb struct from open file (fd) and transpose into kdb 64 if X64
    nb_objs = this->preload(fd, std::string(file), version);
    this->xdr(NULL);

    if(this->k_arch != ARCH) 
    {
        fclose(fd);
        error_msg = prefix_error_msg + "Wrong CPU architecture (" + this->k_arch + ")";
        kwarning(error_msg.c_str());
        return false;
    }

    // Set filename in this->filepath
    memset(fullpath, 0, 1024);
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
    if(version == 1 || version == 2) 
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
            // get the size (length) of the object in the file (compressed or not)
            if(K_read_len(fd, version, &len))
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

            // get the size (length) of the object in the file (compressed or not)
            if(K_read_len(fd, version, &len)) 
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
                ptr = SW_nalloc(len);
                memcpy(ptr, aptr, len);
                SW_nfree(cptr);
                SCR_free(aptr);
            }
            else 
            {
                // Si len >= 0 => version non zippée (dft)
                ptr = SW_nalloc(len);
                success = kread(ptr, len, 1, fd);
                if(!success) 
                    goto error;
            }
            K_xdrobj[this->k_type]((unsigned char*) ptr, NULL);
            ptr = convert_obj_version(name, ptr, version);
            this->binary_to_obj(name, ptr);
            SW_nfree(ptr);
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
            if(K_read_len(fd, version, &len))
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
                    if(K_read_len(fd, version, &len)) 
                        goto error;
                    // zipped file
                    if(len < 0) 
                        len = -len + sizeof(OSIZE);
                    success = kseek(fd, (long) len, 1);
                    if(!success)
                        goto error;
                }
            }

            // read next object

            // get the size (length) of the object in the file (compressed or not)
            if(K_read_len(fd, version, &len)) 
                goto error;
            
            // zipped file
            if(len < 0) 
                len = -len + sizeof(OSIZE);

            name = std::string(c_name);

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
                ptr = SW_nalloc(len);
                memcpy(ptr, aptr, len);
                SW_nfree(cptr);
                SCR_free(aptr);
            }
            else
            {
                ptr = SW_nalloc(len);
                success = kread(ptr, len, 1, fd);
                if(!success) 
                    goto error;
            }

            K_xdrobj[this->k_type]((unsigned char*) ptr, NULL);
            ptr = convert_obj_version(name, ptr, version);
            this->binary_to_obj(name, ptr);
            SW_nfree(ptr);

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
 *  Retrieves infos on an IODE file
 *  
 *  @param [in]  filename   const std::string&   file containing an IODE database
 *  @return                 int                  on success: file type (FILE_COMMENTS...)
 *                                               on error: 
 *                                                   -1 if filename is empty 
 *                                                   -1 if IODE object version is < 0 or > 3
 *                                                   -2 if file cannot be opened
 */
static int sub_file_type_from_file(const std::string& filepath)
{   
    FILE* fd = fopen(filepath.c_str(), "rb");
    if(!fd)
        return -2;

    char label[80];
    memset(label, 0, 80);
    fread(label, strlen(K_LABEL), 1, fd);
    int version = get_version(label);
    // NOTE: return -1 if the file is an ASCII file (not an IODE binary file) 
    if(version < 0 || version > 3) 
    {
        fclose(fd);
        return -1;
    }

    KDBInfo kdb_info(OBJECTS, false);
    kdb_info.preload(fd, filepath, version);
    fclose(fd);

    int iode_type = (int) kdb_info.k_type;
    return iode_type;
}


/**
 *  Given a filename and an expected type, try to retrieve the file type. 
 *  If the file cannot be open, try to open the file in which the extension is replaced
 *  by the standard iode extension for the object type (filename.var). 
 *  
 *  UPDATE 6.64: if the filename contains an extension and the file cannot be opened, the standard iode extension
 *  is not added. 
 *  
 *  @param [in] filename    const std::string&   file to "inspect"
 *  @param [in] iode_type   int                  type expected
 *  @return                 int                  on success file type (FILE_COMMENTS...)
 *                                               on error : -2 if file and file.ext cannot be opened
 *                                                          -1 if the file type or the object version is not recognized 
 */
int get_file_type_from_file(const std::string& filepath, int iode_type)
{
    if(filepath.empty())
    {
        kwarning("Empty filepath");
        return -2;
    }

    // file without extension => try a standard IODE extension
    std::string filepath_with_ext = filepath;
    if(!has_extension(filepath))
    {
        std::string ext = "." + std::string(k_ext[iode_type]);
        filepath_with_ext = add_file_extension(filepath, ext);
    }

    std::filesystem::path p_filepath(filepath_with_ext);
    if(!std::filesystem::exists(p_filepath))
        return -2;

    // Open the file to retrieve its content type
    // NOTE: if ASCII file -> sub_file_type_from_file() returns -1
    int file_type = sub_file_type_from_file(filepath_with_ext);
    return file_type;
}

/**
 *  Interprets the content of a file, ascii files included, 
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
    if(this->k_db_type == DB_SUBSET)
    {
        kwarning("Cannot load file into a shallow copy of a database");
        return false;
    }

    kmsg("Loading %s", filename.c_str());

    this->clear();

    bool success = false;
    int file_type = get_file_type_from_file(filename, (int) this->k_type);
    if(file_type == -1) 
    {   
        // test if correct ascii extension
        std::string expected_ext = "." + std::string(k_ext[this->k_type + ASCII_COMMENTS]);
        std::string file_ext = get_file_extension(filename);
        if(!file_ext.empty() && file_ext == expected_ext) 
            success = this->load_asc(filename);
        
        if(success) 
            this->filepath = filename;
    }
    
    // NOTE: this->filepath updated in load_binary() 
    if(this->k_type == file_type) 
        success = this->load_binary(filename);

    if(success)
    {
        // global_ref_xxx[0] = new <sub_class>(this, "*");      
        if(this->k_db_type == DB_GLOBAL)
            this->update_reference_db();
        kmsg("%d objects loaded", (int) this->size());
    }
    
    return success;
}


/**
 *  Returns the type of content of filename according to its extension:
 *    - 0 -> 6 = .cmt, ..., .var (binary format)
 *    - 8 -> 14 = ac, ... av
 *    - 16 = rep
 *    - 26 = csv
 *    - others formats (rep, a2m, ..., csv): return -1 (undefined)
 *  
 *  @global     char**  k_ext       recognized extensions
 *  @param [in] char*   filename    file to analyze
 *  @return     int                 file type (see above)
 */
int X_findtype(char* filename)
{
    int  lg = (int) strlen(filename);
    char buf[5];

    // check if 'filename' ends with binary files extensions .cmt => .var
    if(lg > 4) 
    {
        for(int i = 0 ; i < 7 ; i++) 
        {
            if(filename[lg - 4] == '.' &&
                SCR_cstrcmp((unsigned char*) k_ext[i], ((unsigned char*) filename) + lg - 3) == 0) 
                    return(i);
        }
    }

    // check if 'filename' ends with ascii extensions .ac => .av
    if(lg > 3) 
    {
        strcpy(buf, ".ac");
        for(int i = 0 ; i < 7 ; i++) 
        {
            buf[2] = k_ext[i][0];   // c, e, i, l, s, t, v (comments, ..., variables)
            if(SCR_cstrcmp(((unsigned char*) filename) + lg - 3, (unsigned char*) buf) == 0) 
                return(8 + i);
        }
    }

    // CSV file
    if(lg > 4 && SCR_cstrcmp(((unsigned char*) filename) + lg - 4, (unsigned char*) ".csv") == 0) 
        return (int) FILE_CSV;
    
    // IODE report file
    if(lg > 4 && SCR_cstrcmp(((unsigned char*) filename) + lg - 4, (unsigned char*) ".rep") == 0) 
        return (int) FILE_REP;
    
    // other formats (a2m, txt, ...)
    return -1;
}


/**
 *  Saves the content of KDB in a file: binary IODE format, ASCII IODE format or CSV. 
 *  The extension of the filename determines if the output file must be in binary, 
 *  ASCII or CSV format.
 *  
 *  @param [in] std::string   filename    filename with or without extension
 */
bool KDB::save(const std::string& filename, const bool compress)
{
    if(this->size() == 0)
    {
        std::string msg = "Database '" + v_iode_types[this->k_type] + "' is empty. ";
        msg += "Nothing to save.";
        kwarning(msg.c_str());
        return false;
    }

    std::filesystem::path p_filepath = check_file(filename, "save", false);

    IodeFileType file_type;
    // if 'filename' has no extension -> use binary format
    if(!p_filepath.has_extension())
        file_type = (IodeFileType) this->k_type;
    else
    {
        file_type = (IodeFileType) X_findtype((char*) filename.c_str());
        // file_type < 0 --> unmanaged format
        if(file_type < 0)
        {
            std::string error_msg = "Cannot save the '" + v_iode_types[this->k_type] + "' database.\n";   
            error_msg += "The filename '" + filename + "' has an invalid extension";
            kwarning(error_msg.c_str());
            return false;
        }
    }

    // filename has an extension -> check that the extension corresponds to the object type
    // filename has no extension -> add the binary file extension associated with the database type
    std::string _filepath_ = check_filepath(filename, file_type, "save", false);

    if(_filepath_.size() >= sizeof(FNAME))
    {
        std::string error_msg = "Cannot save the '" + v_iode_types[this->k_type] + "' database.\n";   
        error_msg += "The filepath '" + _filepath_ + "' is too long";
        throw std::invalid_argument(error_msg);
    }

    p_filepath = std::filesystem::path(_filepath_);
    std::string _filename_ = p_filepath.filename().string();

    kmsg("Saving %s", _filename_.c_str()); 

    // set compression mode
    int klzh = K_LZH;
    K_LZH = compress ? 2 : 0;

    std::string msg;
    bool success = false;
    if(file_type >= FILE_COMMENTS && file_type <= FILE_VARIABLES)
    {
        #ifdef DEBUG
        msg = "KDBTemplate::save() -> save binary file '" + filename + "'";
        logger.info(msg);
        #endif
        success = this->save_binary(_filepath_);
    }
    else if(file_type >= ASCII_COMMENTS && file_type <= ASCII_VARIABLES)
    {
        #ifdef DEBUG
        msg = "KDBTemplate::save() -> save ASCII file '" + filename + "'";
        logger.info(msg);
        #endif
        success = this->save_asc(_filepath_);
    }
    else if(file_type == FILE_CSV)
    {
        #ifdef DEBUG
        msg = "KDBTemplate::save() -> save CSV file '" + filename + "'";
        logger.info(msg);
        #endif
        success = this->save_csv(_filepath_);
    }

    // restore compression mode
    K_LZH = klzh;

    if(!success)
    {
        std::string error_msg = "Could not save the '" + v_iode_types[this->k_type] + "' database";
        kwarning(error_msg.c_str());
    }

    return success;
}

std::string set_file_extension(const std::string& filepath, const int file_type)
{
    if(filepath.empty())
    {
        kwarning("Empty filepath");
        return "";
    }

    std::string ext = "." + std::string(k_ext[file_type]);
    std::string new_filepath = add_file_extension(filepath, ext);
    return new_filepath;
}

/**
 *  Takes a backup of a file by renaming the file: filename.xxx => filename.xx$. 
 *  Function called before saving an IODE object file.
 *   
 *  @param [in] filename    const std::string&   file to backup
 *  @return                 bool    
 */
bool KDB::take_backup(const std::string& filepath) 
{
    if(filepath.empty())
    {
        kwarning("Cannot take backup: filename is empty");
        return false;
    }

    std::filesystem::path p_filepath = check_file(filepath, "take_backup", false);
    
    // NOTE: no need to create a copy backup if the original file does not exist
    if(!std::filesystem::exists(p_filepath))
        return false;
    
    // filename.xxx -> filename.xx$
    std::string backup_ext = p_filepath.extension().string();
    backup_ext[backup_ext.size() - 1] = '$';


    // create a copy of p_filepath with the backup extension
    try
    {
        std::filesystem::path p_filepath_backup = p_filepath;
        p_filepath_backup.replace_extension(backup_ext);
        std::filesystem::copy_file(p_filepath, p_filepath_backup, std::filesystem::copy_options::overwrite_existing);
        
        std::string msg = "A backup of the file '" + p_filepath.filename().string() + "' has been created: '";
        msg += p_filepath_backup.filename().string() + "'";
        kmsg(msg.c_str());
        return true;
    }
    catch(const std::filesystem::filesystem_error& e)
    {
        std::string filename = p_filepath.filename().string();
        std::string error_msg = "Failed to take backup of file '" + filename + "':\n";
        error_msg += e.what();
        kwarning(error_msg.c_str());
        return false;
    }
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


bool KDBInfo::predump(FILE* fd, const std::string& filepath, const int nb_objs)
{
    bool success;
    KDB32 kdb32;

    /* XDR KDB */
    KDBInfo* xdr_kdb = nullptr;
    this->xdr(&xdr_kdb);

    std::string error_msg = "Failed to save database '" + v_iode_types[this->k_type] + "' ";
    error_msg += "to the file '" + filepath + "'";

    // Dump KDB struct
    if(X64) 
    {
        /* convert to x64 if needed */
        memset(&kdb32, 0, sizeof(KDB32));
        kdb32.k_nb = nb_objs;
        kdb32.k_type = xdr_kdb->k_type;
        kdb32.k_mode = xdr_kdb->k_mode;
        kdb32.k_compressed = xdr_kdb->k_compressed;
        memset(kdb32.k_arch, 0, LMAGIC);
        std::strncpy(kdb32.k_arch, xdr_kdb->k_arch.c_str(), xdr_kdb->k_arch.size());
        memset(kdb32.k_desc, 0, K_MAX_DESC);
        std::strncpy(kdb32.k_desc, (char*) xdr_kdb->description.c_str(), xdr_kdb->description.size());
        
        // NOTE: KDBInfo does not have a sample member, but KDBVariables does. 
        //       If the current KDB is a KDBVariables, copy the sample data into 
        //       kdb32.k_data to be able to save it in the binary file.
        char* data = NULL;
        size_t size_of_data = _predump_(&data);
        if(data != NULL)
            memcpy(kdb32.k_data, data, size_of_data);

        success = kwrite((char *) &kdb32, sizeof(KDB32), 1, fd);
        if(!success)
        {
            fclose(fd);
            error_msg += ":\nCannot write IODE file header";
            kwarning(error_msg.c_str());
        }
    }
    else
    {
        success = kwrite(xdr_kdb, sizeof(KDB), 1, fd);
        if(!success)
        {
            fclose(fd);
            error_msg += ":\nCannot write IODE file header";
            kwarning(error_msg.c_str());
        }
    }

    delete xdr_kdb;
    xdr_kdb = nullptr;
    return success;
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
    KOBJ*   k_objs;
    bool    success;

    std::string ext = "." + std::string(k_ext[this->k_type]);
    std::string tmp = trim(filename);
    std::string file_str = add_file_extension(tmp, ext);

    std::string error_msg = "Failed to save database '" + v_iode_types[this->k_type] + "' ";
    error_msg += "to the file '" + file_str + "'";

    if(KDB::backup_enabled) 
        take_backup(file_str);
    
    FNAME file;
    strcpy(file, file_str.c_str());
    FILE* fd = fopen(file, "wb+");
    if(!fd)
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

    this->k_compressed = K_LZH;
    
    success = this->predump(fd, file_str, this->size());
    if(!success) 
    {
        fclose(fd);
        return false;
    }

    // prepare KOBJ table
    i = 0;
    int nb_objs = (int) this->size();
    k_objs = new KOBJ[nb_objs];
    for(const std::string& name : this->get_names())
    {
        strcpy(k_objs[i].o_name, name.c_str());
        k_objs[i].o_val = 0;
        i++;
    }

    // dump KOBJ table -> dump pairs of (name, handle)
    res = K_cwrite(this->k_compressed, (char*) k_objs, sizeof(KOBJ), nb_objs, fd, -1);
    if(res < 0) 
    {
        error_msg += ":\nCannot write object data";
        kwarning(error_msg.c_str());
        fclose(fd);
        return false;
    }

    delete[] k_objs;

    // dump object values as packed objects
    for(const std::string& name : this->get_names())
    {
        success = this->obj_to_binary(&ptr, name);
        if(!success) 
        {
            error_msg += ":\nCannot write object '" + name + "'";
            kwarning(error_msg.c_str());
            fclose(fd);
            return false;
        }

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
 *  @param [in] from    const std::string&   iode file name 
 *  @param [in] to      const std::string&   new filename to put in the file from
 *  @return             int                  0 on success
 *                                           -1 on error (file not found, invalid IODE file)
 *  
 *  TODO: check if not obsolete.
 */
bool update_filename(const std::string& from, const std::string& to)
{
    bool success = false;
    std::string error_msg = "Failed to update the 'filename' in IODE file " + from + ":\n";

    if(from.empty())
    {
        error_msg += "'from' filename is empty";
        kwarning(error_msg.c_str());
        return false;
    }

    if(to.empty())
    {
        error_msg += "'to' filename is empty";
        kwarning(error_msg.c_str());
        return false;
    }

    std::filesystem::path p_filepath(from);
    if(!std::filesystem::exists(p_filepath))
    {
        error_msg += "'from' file '" + from + "' not found";
        kwarning(error_msg.c_str());
        return false;
    }
    
    FILE* fd = fopen(from.c_str(), "rb+");
    if(!fd)
    {
        error_msg += "Cannot open 'from' file '" + from + "' for reading/writing";
        kwarning(error_msg.c_str());
        return false;
    }  

    char label[512];
    success = kread(label, strlen(K_LABEL), 1, fd);
    if(!success) 
    {
        error_msg += "cannot read the IODE file version";
        kwarning(error_msg.c_str());
        fclose(fd);
        return false;
    }

    if(get_version(label) < 0) 
    {
        error_msg += "cannot read the IODE file version";
        kwarning(error_msg.c_str());
        fclose(fd);
        return false;
    }

    KDB* kdb = nullptr;
    success = kread((char *) kdb, sizeof(KDB), 1, fd);
    if(!success) 
    {
        error_msg += "cannot read the IODE file header";
        kwarning(error_msg.c_str());
        fclose(fd);
        return false;
    }

    kdb->filepath = to;

    success = kseek(fd, (long) (-1 * sizeof(KDB)), 1);
    if(!success)
    {
        error_msg += "cannot update the IODE file header";
        kwarning(error_msg.c_str());
        return false;
    }
    
    success = kwrite((char *) kdb, sizeof(KDB), 1, fd);
    if(!success)
    {
        error_msg += "cannot update the IODE file header";
        kwarning(error_msg.c_str());
        fclose(fd);
        return false;
    }

    fclose(fd);
    return true;
}
