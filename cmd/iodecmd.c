#define _IODEVARG_
#include "iode.h"
#include "iodeapi.h"

/*
#pragma link "s_iode.obj"
#pragma link "k_errorv.obj"
#pragma link "sb_gui.obj"


#pragma link "iodeapi.lib"
#pragma link "scr4iode.lib"
#pragma link "odbc32.lib"
*/

/* Extern vars */
extern long  SCR_TOTAL_ALLOC;
extern int   SCR_ALLOC_DOC;
extern char  *SCR_ALLOC_DOC_LOGFILE;
extern int   IODE_FORCEYES;

/* Global vars */
int IODE_VERBOSE = 0;

IodeCmdSyntax()
{
    printf("IODECMD v. %d.%d (%s)\n", IODE_VERSION_MAJOR, IODE_VERSION_MINOR, __DATE__);
    printf("Syntax : iodecmd [-nbrun n] [-alloclog filename] [-v] [-y] [-h] reportfile [arg1 arg2 ...]\n");
}

main(argc, argv)
int     argc;
char    **argv;
{

    int     i, j, nbrun = 1, hdl1, hdl2, rc;
    char    **lst, *reportname = NULL, **reportargs = NULL;
    double  *y;

    SCR_ALLOC_DOC = 0;

    // Error file appended to iodecmd.exe file
    /*  Supprimé pour McAfee ATP
    strcpy(SCR_NAME_ERR, argv[0]);
    SCR_ERR_FILE_NB = 1;
    */
    
    /*  Remplacé pour McAfee ATP */
    SCR_change_ext(SCR_NAME_ERR, argv[0], "scr");
    SCR_ERR_FILE_NB = 1;
    //printf("SCR_NAME_ERR = '%s'", SCR_NAME_ERR);
    /* */
    
    for(i = 1 ; i < argc ; i++) {
	if(strcmp(argv[i], "-nbrun") == 0) {
	    nbrun = atol(argv[i + 1]);
	    i++;
	}
	else if(strcmp(argv[i], "-alloclog") == 0) {
	    SCR_ALLOC_DOC_LOGFILE = argv[i + 1];
	    SCR_ALLOC_DOC = 1;
	    i++;
	}
	else if(strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
	    IODE_VERBOSE = 1;
	}
	else if(strcmp(argv[i], "--forceyes") == 0 || strcmp(argv[i], "-y") == 0) {
	    IODE_FORCEYES = 1;
	}

	else if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
	    IodeCmdSyntax();
	    return(0);
	}
	else if(argv[i][0] == '-') {
	    IodeCmdSyntax();
	    return(1);
	}
	else {
	    reportname = argv[i];
	    reportargs = argv + i + 1;
	    break;
	}
    }


    if(nbrun < 1 || reportname == NULL) {
		printf("%s\n", IODE_VERSION); // JMP 17-11-2015
	printf("Iodecmd : nothing to do ...\n");
	exit(1);
    }

    if(IODE_VERBOSE) printf("%s\n", IODE_VERSION);
    IodeInit();

//    printf("Before 1st run : %d bytes allocated\n", SCR_TOTAL_ALLOC);
    for(i = 0 ; i < nbrun ; i++) {
	rc = IodeExecArgs(reportname, reportargs);
	if(IODE_VERBOSE) printf("\n*****Run %d/%d *****\nrc = %d -- total residual allocation : %d\n", i + 1, nbrun, rc, SCR_TOTAL_ALLOC);
    }

    IodeEnd();
    if(IODE_VERBOSE) printf("After last run : %d bytes allocated\n", SCR_TOTAL_ALLOC);
    AllocDocLoop();
    exit(rc);
}
