#include "iode.h"


/* b_data.c */
B_DataDisplayGraph(arg)
char    *arg;
{
    return(B_DataEditGraph(1, arg));
}

B_DataPrintGraph(arg) /* GB 08-97 */
char    *arg;
{
    return(B_DataEditGraph(0, arg));
}

B_DataEditGraph(view, arg) /* GB 08-97 */
int     view;
char    *arg;
{
    int     rc = 0, nb_args, i, mode, type, xgrid, ygrid, axis;
    double  ymin, ymax;
#ifndef WATCOM
    double  atof();
#endif
    char    **args = NULL;
    SAMPLE  *smpl = 0;

    args = B_ainit_chk(arg, NULL, 0);
    nb_args = SCR_tbl_size(args);    /* JMP 16-12-93 */
    if(nb_args < 10) {
	B_seterrn(67);
	rc = -1;
	goto fin;
    }

    mode = L_pos("LDGdg", args[0][0]); /* GB 10/08/98 */
    mode = max(0, mode);

    type  = B_argpos("LSBM", args[1][0]);
    //xgrid = B_argpos("MNJ",  args[2][0]);
    //ygrid = B_argpos("MNJ",  args[3][0]);
    xgrid = B_argpos("JNM",  args[2][0]); // Corr VL 4/6/2018
    ygrid = B_argpos("JNM",  args[3][0]); // Id
 
    axis  = B_argpos("LGSP", args[4][0]);
    if(memcmp(args[5], "--", 2) == 0) ymin = IODE_NAN;
    else                              ymin = atof(args[5]);
    if(memcmp(args[6], "--", 2) == 0) ymax = IODE_NAN;
    else                              ymax = atof(args[6]);

    smpl = PER_atosmpl(args[7], args[8]);
    if(smpl == 0) {
	B_seterrn(56);
	rc = -1;
	goto fin;
    }

    rc = ODE_graph(view, mode, type, xgrid, ygrid, axis, ymin, ymax,
		    smpl, args + 9);

fin:
    A_free(args);
    SCR_free(smpl);
    return(rc);
}


/* k_sim.c */
IODE_term_vkey(ovtime)
{
    SCR_term_vkey(ovtime);
}

int IODE_hit_key()
{
    return(SCR_hit_key());
}

int IODE_get_key()
{
    return(SCR_get_key());
}

int IODE_confirme(char *str)
{
    return(SCR_confirme(str));
}


/* b_base.c */
IODE_MessageBox(str, v, buts)
unsigned char   *str, *v, *buts[];
{
    SCR_MessageBox(str, v, buts);
}

/* b_win.c */

#include "scr4w.h"
#include <process.h>

extern HWND hWndDOS;

int IodeMinimized = 0;

B_WindowMinimize()
{
    ShowWindow(hWndDOS, SW_MINIMIZE);
    IodeMinimized = 1;
    return(0);
}

B_WindowMaximize()
{
    ShowWindow(hWndDOS, SW_RESTORE);
    IodeMinimized = 0;
    return(0);
}

B_exec_system(char *arg)
{
    U_ch    **argv;
    int     rc;
    int		MiniMaximize = !IodeMinimized; // change windows state only if IODE is maximized

    argv = SCR_vtoms(arg, "\t ");
    kmsg("Executing %s",arg);
    if(MiniMaximize) B_WindowMinimize();
    rc = spawnvp(P_WAIT, argv[0], argv);
    if(MiniMaximize) B_WindowMaximize();
    return(rc);
}

B_shellexec(char *arg)
{
    SHELLEXECUTEINFO    sei;

    memset(&sei, 0, sizeof(sei));
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_DDEWAIT;
    sei.hwnd = hWndDOS;
    sei.nShow = SW_SHOW;
    sei.lpFile = arg;
//    sei.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(1));

    ShellExecuteEx(&sei);
    return(0);
}

