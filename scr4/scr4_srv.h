#ifndef _SCR4_SRV_H_
#define _SCR4_SRV_H_

// #define SCR_ISC_VERSION  xxx reporté dans scr4.h

///////////////////// SERVER ET CLIENT ////////////////////

#define     WSCR_NOTIFYICON     5000
#define     WSCR_INITCLIENT     5002
#define     WSCR_ENDCLIENT      5003
#define     WSCR_SHUTDOWN       5004
#define     WSCR_SETBYTESIN     5005
#define     WSCR_SETBYTESOUT    5006
#define     WSCR_SETCPU         5007
#define     WSCR_GETBYTESIN     5008
#define     WSCR_GETBYTESOUT    5009
#define     WSCR_GETCPU         5010
#define     WSCR_KILLPROCESS    5011
#define     WSCR_GETLASTCMD     5012
#define     WSCR_SETLASTCMD     5013
#define     WSCR_GETCLIENTPID   5014
#define     WSCR_SETCLIENTPID   5015

#ifdef DOS
typedef struct _issclient_ {
    int     sock;
    HWND    hWnd;
    long    starttime;
    long    startdate;
    long    bytesin;
    long    bytesout;
    long    cpu;
    long    lastcmd;
    DWORD   pid;
    char    ip[30];
    char    iptxt[50];
    int     defunct;
} ISSCLIENT;

extern HWND        hWndServer, hWndClient;
extern int         ISS_NB_CLIENTS, ISS_MAX_CLIENTS;
extern ISSCLIENT   *ISS_CLIENTS;
#endif


#define ISS_CRFLTS          1
#define ISS_CRISAM          2
#define ISS_INITDB          3
#define ISS_ENDDB           4
#define ISS_CRDB            5
#define ISS_DELDB           6
#define ISS_DELIDX          7
#define ISS_DELIDXI         8
#define ISS_SETDBSTRING     9
#define ISS_EXECSQLNOIS    10
#define ISS_SETNAMEDB      11
#define ISS_OPENISAM       12
#define ISS_CLOSEDB        13
#define ISS_SEARCHREC      14
#define ISS_NEXTREC        15
#define ISS_PREVREC        16
#define ISS_WRITEREC       17
#define ISS_REWRITEREC     18
#define ISS_DELETEREC      19
#define ISS_LOCKREC        20
#define ISS_UNLOCKREC      21
#define ISS_SEARCHLWHERE   22
#define ISS_CHECKTABLEEXIST 23
#define ISS_REBUILD        24
#define ISS_ERRORTEXT      25
#define ISS_REREADREC      26
#define ISS_CREATESTRUCT   27
#define ISS_FREESTRUCT     28
#define ISS_DEFPREV        29
#define ISS_DBNAME         30
#define ISS_GETCOUNTWNOIS  31
#define ISS_GETMAXNOIS     32
#define ISS_LISTTABLESNOIS 33
#define ISS_CHECKTABLENOIS 34
#define ISS_DELTABLENOIS   35
#define ISS_CRALLISAM      36
#define ISS_SETVARS        37
#define ISS_NEXTRECN       38
#define ISS_GETVERS        39
#define ISS_PREVRECN       40
#define ISS_SEARCHRECN     41
#define ISS_REPOS          42
#define ISS_FILEDN         43
#define ISS_FILEUP         44
#define ISS_SETUSER        45
#define ISS_SETPWD         46
#define ISS_SETVARSGNL     47
#define ISS_END            48
#define ISS_FOPEN          49
#define ISS_FCLOSE         50
#define ISS_FREAD          51
#define ISS_FWRITE         52
#define ISS_GETC           53
#define ISS_PUTC           54
#define ISS_FEOF           55
#define ISS_ACCESS         56
#define ISS_RENAME         57
#define ISS_STAT           58
#define ISS_UNLINK         59
#define ISS_SYSTEM         60
#define ISS_WINEXEC        61 // 2.04
#define ISS_DIR            62
#define ISS_WAIT           63
#define ISS_MKDIR          64 // 2.05
#define ISS_RMDIR          65
#define ISS_XCOPY          66 // 2.06
#define ISS_CLOSEDSN       67 // 2.06       /* BP_M 30-10-2001 10:15 */
#define ISS_FSEEK          68 // 2.09       /* JMP 03-08-02 */
#define ISS_FTELL          69 // 2.09       /* JMP 03-08-02 */
#define ISS_FLOCK          70 // 2.09       /* JMP 03-08-02 */
#define ISS_FUNLOCK        71 // 2.09       /* JMP 03-08-02 */
#define ISS_GETCRC32       73               /* JMP 23-10-02 */
#define ISS_GETTAR         74               /* JMP 23-10-02 */
#define ISS_GETTIME        75               /* JMP 23-10-02 */
#define ISS_GETDATE        76               /* JMP 23-10-02 */
#define ISS_GETACCESS      77               /* JMP 23-10-02 */
#define ISS_DIREX          78               /* JMP 25-10-02 */
#define ISS_STATEX         79               /* JMP 25-10-02 */
#define ISS_SETFILETIME    80               /* JMP 29-10-02 */
#define ISS_GETCRC32LG     81               /* JMP 29-10-02 */
#define ISS_LOCALCOPY      82               /* JMP 01-11-02 */
#define ISS_CHMOD          83               /* JMP 01-11-02 */
#define ISS_FULLPATH       84               /* JMP 10-11-02 */
#define ISS_GETCWD         85               /* JMP 10-11-02 */
#define ISS_FFLUSH         86               /* JMP 14-11-02 */
#define ISS_CHECKDBSTRING  87 // 3.01
#define ISS_CHECKPACK      88 // 3.01
#define ISS_CHDIR          89 // 4.01       /* JMP 03-03-2004 */
#define ISS_MSTATEX        90 // 4.03       /* JMP 13-02-05 */
#define ISS_GETCOUNTFREE   120              /* BP_M 02-12-2003 16:56 */
//#define ISS_CHECKISREL     121            /* BP_M 16-02-2004 11:51 */
#define ISS_SETNOREREAD    122              /* BP_M 28-04-2004 */
#define ISS_SETDEBUGNAME   123              /* BP_M 27-11-2007 */
#define ISS_SETREPLICA     124              /* BP_M 17-12-2007 */
#define ISS_GETLASTREQ     125              /* BP_M 15-01-2008 */
#define ISS_STARTREPLICA   126              /* BP_M 15-01-2008 */
#define ISS_STOPREPLICA    127              /* BP_M 15-01-2008 */
#define ISS_SETSQLSEARCHLIKE 128
#define ISS_SWITCHDSN      129              /* BP_M 04-02-2008 */
#define ISS_CHECKCONNECT   130              /* BP_M 04-02-2008 */
#define ISS_CONNEXIONOK    131              /* BP_M 18-02-2008 */
#define ISS_SETCURDSN      132              /* BP_M 18-02-2008 */
#define ISS_SETCWD         133              /* BP_M 25-03-2008 */
#define ISS_SETREPLICALISTACCEPT 134        /* BP_M 18-07-2008 */
#define ISS_CONSTRUCTISAM  135              /* BP_M 23-10-2009 17:07 */
#define ISS_SETLIMSQL       300             /* BP_M 24-05-2005 15:20 */
#define ISS_SENDINFO        301             /* BP_M 07-11-2009 13:59 */
#define ISS_LAST_CMD        302             /* BP_M 05-01-2010 15:37 */
#define ISS_SET_LIMIT       303             /* BP_M 21-01-2010 15:30 */
#define ISS_SETTIMER        304             /* BP_M 10-12-2010 09:59 */
#define ISS_STOPTIMER       305             /* BP_M 10-12-2010 09:59 */
#define ISS_SETJREQ         306             /* BP_M 07-12-2011 13:27 */
#define ISS_EXECSQLIS       307             /* BP_M 13-02-2012 15:22 */
#define ISS_FREEREPLICA     310             /* BP_M 19-06-2013 15:11 */



//#define ISS_SETS4SRVPORT    308             /* BP_M 06-05-2013 12:50 */

extern int ISC_CLIENT_VERS, ISC_SERVER_VERS;
extern int ISC_SOCKET;


#endif



















