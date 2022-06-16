/** 
 * @header4iode
 *
 * Some IODE functions may differ according to the context. For example, in the context of a console app, error messages will be 
 * printed in the console. In the DOS-Win32 context, error messages are displayed in the status bar, in the Qt context in a different way...
 * 
 * For each of these functions, a standard implementation is provided in iodeapi.lib, generally using the stdio functions like printf.
 * At the same time, an optional "super function" pointer may point to a alternative function. If that pointer is not null, 
 * the alternative function will replace (superseed) the original implementation. In that way, each context  has possibly its own implementation
 * without interfering with the others.
 * 
 * 
 * List of functions that can be superseeded
 * -----------------------------------------
 *     void kerror(int level, char* fmt, ...)       Displays an error message and optionally exits the program.
 *     void kpause(char* msg)                       Displays a message and then waits for the user to press ENTER.
 *     void kwarning(char* fmt, ...)                Displays a message and optionally asks the user to press ENTER before continuing.
 *     void kmsg(char* fmt, ...)                    Displays a message.
 *     int kconfirm(char *fmt,...)                  Displays a message and optionally asks confirmation before continuing.
 *     int kmsgbox(unsigned char *str, unsigned char *v, unsigned char **buts) Displays a message box with optional buttons. 
 *     void krecordkey(int key)                     Records a key in the keyboard buffer. 
 *     int Wprintf(char* fmt, ...)                  Displays a message.
 *     int SCR_panic()                              Exits the program (normally on a "memory full" event).
 *     void ksettitle()                             Set the window title (GUI only).
 *     int ktermvkey(int vkey)                      Defines the interval to wait between two checks on the keyboard buffer length (GUI only).
 *     int khitkey()                                Checks whether the keyboard buffer is not empty (GUI only).
 *     int kgetkey()                                Reads the next character in the keyboard buffer (GUI only).
 *     void kbeep()                                 Plays a sound (GUI only).
 *     SAMPLE *kasksmpl()                           Asks the user to give a SAMPLE (GUI only).
 *     int kexecsystem()                            Calls the fonction system().
 *     int kshellexec()                             Call the Win32 function ShellExecuteEx().
 *
 * SCR4 superseeded functions + assign
 * -----------------------------------
 *     char *A_expand_super_API(char* name)         Default implementation of A_expand() in IODE API
 *     void IODE_assign_super_API()                 Assigns default values to "super" (virtual) functions. 
 * 
 * List of function pointers that can replace the standard implementation  
 * ----------------------------------------------------------------------
 *      void (*kerror_super)(int level, char*msg);
 *      void (*kwarning_super)(char* msg);
 *      void (*kmsg_super)(char* msg);
 *      int  (*kwprintf_super)(char* msg);
 *      void (*kpanic_super)(void);
 *      int  (*kconfirm_super)(char* msg);
 *      int  (*kmsgbox_super)(unsigned char *str, unsigned char *v, unsigned char **buts);
 *      int   kmsgbox_continue = 0;
 *      int   kpause_continue = 0;
 *      void (*krecordkey_super)(int ch);
 *      void (*ksettitle_super)(void);
 *      int  (*ktermvkey_super)(int vkey);
 *      int  (*khitkey_super)();
 *      int  (*kgetkey_super)();
 *      void (*kbeep_super)(void);
 *      SAMPLE *(*kasksmpl_super)(void);
 *      int kexecsystem_super(char*);
 *      int kshellexec_super(char*);    
 *  
 */ 


#include "iode.h"
#include <stdarg.h>

// Global variables
void (*kerror_super)(int level, char*msg);
void (*kwarning_super)(char* msg);
void (*kpause_super)();
void (*kmsg_super)(char* msg);
int  (*kwprintf_super)(char* msg);
void (*kpanic_super)(void);
int  (*kconfirm_super)(char* msg);
int  (*kmsgbox_super)(unsigned char *str, unsigned char *v, unsigned char **buts);
int   kmsgbox_continue = 0; 
int   kpause_continue = 0; 
void (*krecordkey_super)(int ch);
void (*ksettitle_super)(void);
int  (*ktermvkey_super)(int vkey);
int  (*khitkey_super)();
int  (*kgetkey_super)();
void (*kbeep_super)(void);
SAMPLE *(*kasksmpl_super)(void);
int  (*kexecsystem_super)();
int  (*kshellexec_super)();

/**
 *  Displays an error message and optionally exits the program.
 *  By default, the message is sent to the stderr.
 *  
 *  If the function pointer kerror_super is not null, it is called instead.
 *  
 *  @param [in] level int   if not null, the default function exits with a return code = level
 *  @param [in] fmt   char* format of the following parameters ("%s %d...")
 *  @param [in] ...         optional list of additional parameters    
 *  
 */
void kerror(int level, char* fmt, ...)
{
    char    buf[10240];
    va_list myargs;

    va_start(myargs, fmt);
#ifdef _MSC_VER   
    vsnprintf_s(buf, sizeof(buf) - 1, _TRUNCATE, fmt, myargs);
#else
    vsnprintf_s(buf, sizeof(buf) - 1, fmt, myargs);
#endif    
    va_end(myargs);

    if(kerror_super != 0) 
        (*kerror_super)(level, buf);
    else {
        //fprintf(stderr, "%s\n", buf);
        printf("%s\n", buf);
        if(level == 1)  
            exit(1);
    }    
}


/**
 *  Displays the message "Press ENTER to continue" and then waits for the user to press ENTER.
 *  Sub of kmsgbox() and kwarning().
 *  
 *  The default behaviour is :
 *      - if the global int kpause_continue is not null, does not wait the user to press ENTER after having printed the message.
 *      - if kpause_continue is null, waits for a user input.
 *    
 *  
 */
void kpause()
{
    int     ch = 0;

    if(kpause_continue) return;
    if(kpause_super != 0) 
        (*kpause_super)();
    else {
        printf("Press ENTER to continue\n"); 
        while(ch != '\n') ch = getchar();
    }    
}


/**
 *  Displays a message and optionally asks the user to press ENTER before continuing.
 *  
 *  The default behaviour is :
 *     - if the global int kpause_continue is not null, does not wait the user to press ENTER after having printed the message.
 *  
 *  If the function pointer kwarning_super is not null, it is called instead.
 *  
 *  @param [in] fmt   char*     format of the following parameters (ex.: "%s %d...")
 *  @param [in] ...             optional list of additional parameters    
 *  
 */
void kwarning(char* fmt, ...)
{
    char    buf[10240];
    va_list myargs;

    va_start(myargs, fmt);
#ifdef _MSC_VER   
    vsnprintf_s(buf, sizeof(buf) - 1, _TRUNCATE, fmt, myargs);
#else
    vsnprintf_s(buf, sizeof(buf) - 1, fmt, myargs);
#endif    

    va_end(myargs);

    if(kwarning_super != 0) 
        (*kwarning_super)(buf);
    else {
        printf("%s\n", buf);
        kpause();
    }    
}


/**
 *  Displays a message.
 *  
 *  The default behaviour is to print the message on stdout.
 *  
 *  If the function pointer kmsg_super is not null, it is called instead.
 *  
 *  @param [in] fmt   char*     format of the following parameters (ex.: "%s %d...")
 *  @param [in] ...             optional list of additional parameters    
 *  
 */
void kmsg(char* fmt, ...)
{
    char    buf[10240];
    va_list myargs;

    va_start(myargs, fmt);
#ifdef _MSC_VER   
    vsnprintf_s(buf, sizeof(buf) - 1, _TRUNCATE, fmt, myargs);
#else
    vsnprintf_s(buf, sizeof(buf) - 1, fmt, myargs);
#endif    
    va_end(myargs);
    if(kmsg_super != 0) 
        (*kmsg_super)(buf);
    else 
        printf("%s\n", buf);
}


/**
 *  Displays a message and optionally asks confirmation before continuing.
 *  
 *  The default behaviour is to print the message and to return(1).
 *  TODO: default behaviour: waits for a answer and returns the answer
 *  
 *  If the function pointer kconfirm_super is not null, it is called instead.
 *  
 *  @param [in] fmt   char*     format of the following parameters (ex.: "%s %d...")
 *  @param [in] ...             optional list of additional parameters    
 *  
 */
int kconfirm(char *fmt,...)
{
    char    buf[10240];
    va_list myargs;

    va_start(myargs, fmt);

#ifdef _MSC_VER   
    vsnprintf_s(buf, sizeof(buf) - 1, _TRUNCATE, fmt, myargs);
#else
    vsnprintf_s(buf, sizeof(buf) - 1, fmt, myargs);
#endif    

    va_end(myargs);
    
    if(kconfirm_super != 0) 
        return((*kconfirm_super)(buf));
    else {
        printf("%s\n", buf);
        return(1);              // TODO: gets() et traiter la réponse 
    }    
}


/**
 *  Displays a message box with optional buttons. 
 *  
 *  The default behaviour is :
 *      - if the global int kmsgbox_continue is not null, return(1) without waiting for the user to press ENTER 
 *      - if kmsgbox_continue is null, calls kpause() (i.e. waits for the user).
 *    
 *  If the function pointer kmsgbox_super is not null, it is called instead.
 *  
 *  @param [in] str  char*  title of the message box (GUI version only)
 *  @param [in] v    char*  text of the message
 *  @param [in] buts char** table of buttons (GUI version only)       
 *  @return          int    1 or returned value of kmsgbox_super()
 *  
 */
int kmsgbox(unsigned char *str, unsigned char *v, unsigned char **buts)
{
    if(kmsgbox_super != 0) 
        return((*kmsgbox_super)(str, v, buts));
    else {
        printf("%s\n", v);
        if(!kmsgbox_continue) 
            kpause();
        return(1);
    }    
}


/**
 *  Records a key in the keyboard buffer. 
 *  
 *  The default behaviour is: do nothing.
 *  
 *  If the function pointer krecordkey_super is not null, it is called instead.
 *  
 *  @param [in] key     int     key to record
 *  
 */
void krecordkey(int key)
{
    if(krecordkey_super != 0) 
        (*krecordkey_super)(key);
}


/**
 *  Records a text in the keyboard buffer in reverse order (LIFO).
 *  
 *  Calls krecordkey() for each character in text.
 *  
 *  @param [in] text    unsigned char*  text to record
 */
void krecordtext(unsigned char*  text)
{
    int     i;

    for(i = (int)strlen(text) - 1 ; i >= 0 ; i--)
        krecordkey(text[i]);
}


/**
 *  Displays a message.
 *  
 *  The default behaviour is to print the message on stdout.
 *  
 *  If the function pointer kwprintf_super is not null, it is called instead.
 *  
 *  @param [in] fmt     char*   format of the following parameters (ex.: "%s %d...")
 *  @param [in] ...             optional list of additional parameters    
 *  @return             int     0 (for compatibility)
 *  
 *  @note Important: Wprintf() is used by some functions present in scr4iode.lib but is
 *  not defined in scr4iode.lib. Therefore, the definition below is required to link test1.exe, iodecmd.exe...
 *  
 *  However, Wprintf() IS DEFINED in s32wo.lib (used only with iode.exe with "DOS" GUI interface). 
 *  And consequently, we have 2 definitions at link time: iodeapi.lib and s32wo.lib. 
 *  It works, but should be fixed. 
 *   
 *  
 */

int Wprintf(char* fmt, ...)
{
    char    buf[10240];
    va_list myargs;

    va_start(myargs, fmt);
#ifdef _MSC_VER   
    vsnprintf_s(buf, sizeof(buf) - 1, _TRUNCATE, fmt, myargs);
#else
    vsnprintf_s(buf, sizeof(buf) - 1, fmt, myargs);
#endif    
 
    va_end(myargs);
    if(kwprintf_super != 0) 
        (*kwprintf_super)(buf);
    else 
        printf("%s\n", buf);
    
    return(0);
}


/**
 *  Exits the program (normally on a "memory full" event).
 *  
 *  The default behaviour is to print a standard message and to exit with a return code of 2.
 *  
 *  If the function pointer kpanic_super is not null, it is called instead.
 *  
 */
int SCR_panic()
{
    if(kpanic_super) 
        (*kpanic_super)();
    else 
        fprintf(stderr, "**** Memory full. Program stopped.****\n");
    exit(2);
}


/**
 *  Set the window title (GUI only).
 *  
 *  The default behaviour is to do nothing.
 *  
 *  If the function pointer ksettitle_super is not null, it is called instead.
 */
void ksettitle()
{
    if(ksettitle_super) 
        (*ksettitle_super)();
}


/**
 *  Defines the interval to wait between two checks on the keyboard buffer length (GUI only).
 *  
 *  The default behaviour is to do nothing.
 *  
 *  If the function pointer ktermvkey_super is not null, it is called instead.
 */
int ktermvkey(int vkey)  
{
    if(ktermvkey_super) 
        return((*ktermvkey_super)(vkey));
    
    return(0);
}


/**
 *  Checks whether the keyboard buffer is not empty (GUI only).
 *  
 *  The default behaviour is to do nothing.
 *  
 *  If the function pointer khitkey_super is not null, it is called instead.
 */
int khitkey()  
{
    if(khitkey_super) 
        return((*khitkey_super)());
    
    return(0);
}


/**
 *  Reads the next character in the keyboard buffer (GUI only).
 *  
 *  The default behaviour is to do nothing.
 *  
 *  If the function pointer kgetkey_super is not null, it is called instead.
 */
int kgetkey()  
{
    if(kgetkey_super) 
        return((*kgetkey_super)());
    
    return(0);
}


/**
 *  Plays a sound (GUI only).
 *  
 *  The default behaviour is to do nothing.
 *  
 *  If the function pointer kbeep_super is not null, it is called.
 */
void kbeep()  
{
    if(kbeep_super) 
        (*kbeep_super)();
}

/**
 *  Asks the user to give a SAMPLE (GUI only).
 *  
 *  The default behaviour is to return the sample of KV_WS or NULL is KV_WS is NULL
 *  
 *  If the function pointer kasksmpl_super is not null, it is called instead.
 */

SAMPLE  *kasksmpl()
{
    SAMPLE  *smpl = KSMPL(K_WS[K_VAR]);

    if(kasksmpl_super) 
        return((*kasksmpl_super)());
    
    if(KV_WS != 0)
        return(KSMPL(KV_WS));
    else 
        return(NULL);
}


/**
 *  Calls the fonction system().
 *  Can be superseeded by assigning kexecsystem_super.
 *    
 *  @param [in] arg     
 *  @return     
 *  
 */
int kexecsystem(char *arg)
{
    int     rc;

    kmsg("Executing %s",arg);

    // Alternate implementation
    if(kexecsystem_super) 
        return((*kexecsystem_super)(arg));

    // Default implementation
	rc = system(arg); 
    return(rc);
}


/**
 *  Call the Win32 function ShellExecuteEx().
 *  If kshellexec_super is not null, it replaces the default behaviour.
 *  
 *  @param [in] arg 
 *  @return 
 *  
 */
int kshellexec(char *arg)
{
    SHELLEXECUTEINFO    sei;

    if(kshellexec_super) 
        return((*kshellexec_super)(arg));

    // Default implementation
    memset(&sei, 0, sizeof(sei));
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_DDEWAIT;
    //sei.hwnd = hWndDOS;
	sei.hwnd = 0;
    sei.nShow = SW_SHOW;
    sei.lpFile = arg;
//    sei.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(1));

    ShellExecuteEx(&sei);
    return(0);
}


/**
 *  Default implementation of A_expand() in IODE API.
 *  Replaces "$LISTNAME" by the list "LISTNAME" contents.
 *  
 *  @param [in] name    char*   string to be expanded
 *  @return             char*   expanded string  
 *  
 */
char    *A_expand_super_API(char* name)
{
    int     pos;

    pos = K_find(KL_WS, name);
    if (pos < 0) return(NULL);
    return((char *)KLVAL(KL_WS, pos));
}


/**
 *  Assigns default values to "super" (virtual) functions. 
 *  Specifically, implements scr4 functions that can be superseeded (2022 version only).
 *  
 *  In the past, some scr4 functions have been replaced in IODE but with some linker, that method
 *  was not always acceptable. An alternative fonction is thus assigned to a fn pointer
 *  that, if not null, will replace the original implementation.
 *   
 */
void IODE_assign_super_API()
{
    A_expand_super = A_expand_super_API;   // Ok for other implementations (DOS, IODECOM, PYTHON, Qt)
    // A_error_super  = A_error_super_API; //  To be implemented for DOS and Qt (IODECOM ?)
}