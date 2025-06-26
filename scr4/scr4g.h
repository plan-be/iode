#ifndef _SCR4G_H_
#define _SCR4G_H_

#include "scr4.h"
#include <gnome.h>
#define SCRGNOME


typedef struct _GSCRACT_ {
    short   type;
    int     obj_nb;
    int     fld_nb;
    int     act_nb;
} GSCRACT;

typedef struct _GWDG_ {
    GtkWidget   *scrolled;
    GtkWidget   *widget;
    int         x, y, w, h;
    int         cursel;
    int         nl, nlv;
    int         nc, ncv;
    int         (*fn)();
    char        *data;
} GWDG;

typedef struct _GDDLGFN_ {
    int     key;
    int     (*fn)();
} GDDLGFN;

typedef struct _GDDLG_ {
    GtkWidget   *window;
    GtkWidget   *fixed;
    GWDG        **swdgs;
    int         nbswdgs;
    int         curwdg;
    GDDLGFN     *fns;
    int         nbfns;
    int         x, y, w, h;

} GDDLG;

typedef struct _GCOLOR_ {
    char        *name;
    GdkColor    color;
} GCOLOR;

#define GXPM char **

/******** VARIABLES SPECIFIQUES *******************/

extern GCOLOR       *GSCR_COLORS;
extern int          GSCR_NB_COLORS;
//extern char       **GSCR_COLORS_NAMES;

extern GXPM         *GSCR_PIXMAPS;
extern int          GSCR_NB_PIXMAPS;

/****************** Prototypes spécifiques à Gtk/Gnome ***************/
GtkWidget *GscrCreateApp();
GnomeUIInfo *GapCreateMenu(MENU *mn);
GnomeUIInfo *GapCreateApplMenu(APPL *ap);
GnomeUIInfo *GapCreateApplToolbar(int tb_nb);
gint GscrEventDeleteMainApp(GtkWidget *widget, GdkEvent *event,gpointer data);
gint GscrEventDestroyMainApp(GtkWidget *widget,GdkEvent *event, gpointer data);
int GapEditAppl(APPL *ap);
GSCRACT *GscrAddAct(int type, int obj_nb, int fld_nb, int act_nb);

GDDLG *GDDlgCreate(int x, int y, int w, int h, U_ch *ctitle);
GWDG *GLVCreate(int w, int h, int nl, U_ch **titles);

GtkWidget *GpgCreateEx(PAGE *pg, int lvline, int lvcol, int lvnl, int lvnc, U_ch **coltitles);
GtkWidget *GpgCreate(PAGE *pg);
int GscrMenuLV(MENU *mn, char sep, int istitle, PAGE *pg, int fld_nb);

extern GtkWidget    *GSCR_MAIN_APP;

#endif  // _SCR4G_H_












