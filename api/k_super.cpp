/** 
 * @header4iode
 *
 * Some IODE functions may differ according to the context. For example, in the context of a console app, error messages will be 
 * printed in the console. In the DOS-Win32 context, error messages are displayed in the status bar, in the Qt context in a different way...
 * 
 * For each of these functions, a standard implementation is provided in iode_c_api.lib, generally using the stdio functions like printf.
 * At the same time, an optional "super function" pointer may point to a alternative function. If that pointer is not null, 
 * the alternative function will replace (superseed) the original implementation. In that way, each context  has possibly its own implementation
 * without interfering with the others.
 * 
 * 
 * List of functions that can be superseeded
 * -----------------------------------------
 *     void kerror(const int level, const char* fmt, ...)       Displays an error message and optionally exits the program.
 *     void kpause(const char* msg)                             Displays a message and then waits for the user to press ENTER.
 *     void kwarning(const char* fmt, ...)                      Displays a message and optionally asks the user to press ENTER before continuing.
 *     void kmsg(const char* fmt, ...)                          Displays a message.
 *     int kconfirm(char *fmt,...)                              Displays a message and optionally asks confirmation before continuing.
 *     int kmsgbox(const unsigned char *str,                    Displays a message box with optional buttons.
 *                 const unsigned char *v, 
 *                 const unsigned char **buts)  
 *     void krecordkey(const int key)                           Records a key in the keyboard buffer. 
 *     void krecordtext(const unsigned char*  text)             Records a text in the keyboard buffer in reverse order (LIFO).
 *     int Wprintf(const char* fmt, ...)                        Displays a message.
 *     int SCR_panic()                                          Exits the program (normally on a "memory full" event).
 *     void ksettitle()                                         Set the window title (GUI only).
 *     int ktermvkey(const int vkey)                            Defines the interval to wait between two checks on the keyboard buffer length (GUI only).
 *     int khitkey()                                            Checks whether the keyboard buffer is not empty (GUI only).
 *     int kgetkey()                                            Reads the next character in the keyboard buffer (GUI only).
 *     void kbeep()                                             Plays a sound (GUI only).
 *     Sample *kas)                                       Asks the user to give a Sample (GUI only->sample.
 *     int kexecsystem()                                        Calls the fonction system().
 *     int kshellexec()                                         Call the Win32 function ShellExecuteEx().
 *
 *     int ODE_end()                                            Ends an IODE session.
 *
 * SCR4 superseeded functions + assign
 * -----------------------------------
 *     char *A_expand_super_API(char* name)                     Default implementation of A_expand() in IODE API
 *     void IODE_assign_super_API()                             Assigns default values to "super" (virtual) functions. 
 * 
 * List of function pointers that can replace the standard implementation  
 * ----------------------------------------------------------------------
 *      int  (*kerror_super)(const int level, const char* msg);
 *      void (*kwarning_super)(const char* msg);
 *      void (*kmsg_super)(const char* msg);
 *      int  (*kwprintf_super)(const char* msg);
 *      void (*kpanic_super)(void);
 *      int  (*kconfirm_super)(const char* msg);
 *      int  (*kmsgbox_super)(const unsigned char* str, const unsigned char* v, const unsigned char** buts);
 *      int   kmsgbox_continue = 0;
 *      int   kpause_continue = 0;
 *      void (*krecordkey_super)(const int ch);
 *      void (*krecordtext_super)(const unsigned char* text);
 *      void (*ksettitle_super)(void);
 *      int  (*ktermvkey_super)(const int vkey);
 *      int  (*khitkey_super)();
 *      int  (*kgetkey_super)();
 *      void (*kbeep_super)(void);
 *      Sample *(*kasksmpl_super)(void);
 *      int kexecsystem_super(const char*);
 *      int kshellexec_super(const char*);    
 *      int ODE_end_super(const int);    
 *  
 */ 
#include <stdarg.h>
#include <stdlib.h>
#ifdef _MSC_VER
    #include <Windows.h>
#endif

#include "scr4/s_args.h"

#include "api/constants.h"
#include "api/k_super.h"
#include "api/objs/objs.h"
#include "api/objs/lists.h"
#include "api/objs/variables.h"
#include "api/report/undoc/undoc.h"


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
int kerror(const int level, const char* fmt, ...)
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
        return (*kerror_super)(level, buf);
    else {
        //fprintf(stderr, "%s\n", buf);
        printf("%s\n", buf);
        if(level > 0)  
            exit(level);
        else
            return -1;
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
void kwarning(const char* fmt, ...)
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
void kmsg(const char* fmt, ...)
{
    char    buf[10240];
    va_list myargs;

    if(MSG_DISABLED) 
        return;

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
 *  Function replacing ksmg() to suppress messages.
 *  
 *  @param [in] msg char*   message text 
 */
void kmsg_null(const char* msg) 
{
}

/**
 *  Suppress or restore default kmsg() behaviour.
 *  
 *  @param [in] IsOn int    0: suppress the messages, 1: restore the default function
  */
void kmsg_toggle(const int value)
{
    MSG_DISABLED = (value == 0) ? 1 : 0;
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
int kconfirm(const char *fmt,...)
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
#ifdef __GNUC__
        fgets(buf, sizeof(buf) - 1, stdin);
#else
        gets_s(buf, sizeof(buf) - 1);
#endif
        SCR_sqz((unsigned char*) buf);
        return(!U_is_in(buf[0], "OoYyJj1"));
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
int kmsgbox(const unsigned char* str, const unsigned char* v, const unsigned char** buts)
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
void krecordkey(const int key)
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
void krecordtext(const unsigned char* text)
{
    int     i;

    for(i = (int)strlen((char*) text) - 1 ; i >= 0 ; i--)
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
 *  @note Important: Wprintf() is used by some functions present in iode_scr4.lib but is
 *  not defined in iode_scr4.lib. Therefore, the definition below is required to link test1.exe, iodecmd.exe...
 *  
 *  However, Wprintf() IS DEFINED in s32wo.lib (used only with iode.exe with "DOS" GUI interface). 
 *  And consequently, we have 2 definitions at link time: iode_c_api.lib and s32wo.lib. 
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
int ktermvkey(const int vkey)  
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
 *  Asks the user to give a Sample (GUI only).
 *  
 *  The default behaviour is to return the sample of KV_WS or NULL is KV_WS is NULL
 *  
 *  If the function pointer kasksmpl_super is not null, it is called instead.
 */

Sample* kasksmpl()
{
    Sample  *smpl = K_WS[VARIABLES]->sample;

    if(kasksmpl_super) 
        return((*kasksmpl_super)());
    
    if(KV_WS != 0)
        return(KV_WS)->sample;
    else 
        return(NULL);
}


/**
 *  Calls the fonction system().
 *  Can be superseeded by assigning kexecsystem_super.
 *    
 *  @param [in] char* arg     command to execute
 *  @return     int           return code of system or the replacement function        
 *  
 */
int kexecsystem(const char *arg)
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
 *  @param [in] char* arg     command to execute
 *  @return     int           0 or return code of the "super" replacement function
 *  
 */
int kshellexec(const char *arg)
{
    int res = 0;

#ifdef _MSC_VER
    SHELLEXECUTEINFOW    sei;
    wchar_t warg[1024];

    if(kshellexec_super) 
        return((*kshellexec_super)(arg));

    // Default implementation
    memset(&sei, 0, sizeof(sei));
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_DDEWAIT;
    //sei.hwnd = hWndDOS;
	sei.hwnd = 0;
    sei.nShow = SW_SHOW;
    mbstowcs(warg, arg, 1024);
    sei.lpFile = warg;
//    sei.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(1));

    ShellExecuteExW(&sei);
    return(res);
#else
    if(kshellexec_super) 
        return((*kshellexec_super)(arg));

    res = system(arg);
    return res;
#endif
}


/**
 *  Default implementation of A_expand() in IODE API.
 *  Replaces "$LISTNAME" by the list "LISTNAME" contents.
 *  
 *  @param [in] name    char*   string to be expanded
 *  @return             char*   expanded string  
 *  
 */
char* A_expand_super_API(char* name)
{
    int     pos;

    pos = KL_WS->find(const_cast<char*>(name));
    if (pos < 0) return(NULL);
    return((char *)KLVAL(KL_WS, pos));
}


/**
 *  Ends an IODE session. Cleans up the allocated variables.
 *  If ODE_end_super is not null, it replaces the default behaviour.
 *  
 *  @param [in]  int    st  
 *  @return 
 *  
 */
int ODE_end(const int st)
{
    if(ODE_end_super) 
        return((*ODE_end_super)(st));

    // Default implementation
    return(IodeEnd());
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
