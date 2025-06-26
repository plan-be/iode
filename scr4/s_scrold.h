
/*** Moved from s_scroll.h due to compilation problems 8/12/2011 ***/
#define DATA        (SCRL->sc_pcl)

#define PL          (SCRL->sc_ipl)          // position on the screen
#define PC          (SCRL->sc_ipc)

#define NLC         (SCRL->sc_inlc)        // nbr of lines & cols
#define NCC         (SCRL->sc_incc)
#define ATTR        (SCRL->sc_iattr)
#define RATTR       (SCRL->sc_irattr)
#define LC0         (SCRL->sc_ilc0)
#define RLEN        (SCRL->sc_irlen)
#define IR          (SCRL->sc_ir)
#define IM          (SCRL->sc_im)
#define INS         (SCRL->sc_ins)
#define IAS         (SCRL->sc_ias)
#define NOROT       (SCRL->sc_norot)
#define NOHSBAR     (SCRL->sc_nohsb)
#define NOTITL      (SCRL->sc_noltit)
#define NOTITC      (SCRL->sc_noctit)

#define L           (SCRL->sc_icl)
#define C           (SCRL->sc_icc)
#define L0          (SCRL->sc_icl0)
#define C0          (SCRL->sc_icc0)
#define NL          (SCRL->sc_inl)
#define NC          (SCRL->sc_inc)
#define ML          (SCRL->sc_iml)
#define MC          (SCRL->sc_imc)
#define VBAR        (SCRL->sc_vbar)
#define HBAR        (SCRL->sc_hbar)

#define FTITLE       ((*(SCRL->sc_ftitle))(SCRL))
#define FCTITLE(i)   ((*(SCRL->sc_fctitle))(SCRL, i))
#define FLTITLE(i)   ((*(SCRL->sc_fltitle))(SCRL, i))
#define FML          ((*(SCRL->sc_fnl))(SCRL))
#define FMC          ((*(SCRL->sc_fnc))(SCRL))
#define FLTEXT(i,j)  ((*(SCRL->sc_ftext))(SCRL, i, j))
#define FLEN(i)      ((*(SCRL->sc_flen))(SCRL, i))
#define FEDIT(i, j)  ((*(SCRL->sc_fedit))(SCRL, i, j))
#define FDISP(i, j)  ((*(SCRL->sc_fdisp))(SCRL, i, j))
#define FDEL(i, j)   ((*(SCRL->sc_fdel))(SCRL, i, j))
#define FINS(i, j)   ((*(SCRL->sc_fins))(SCRL, i, j))
#define FFN(key, i, j)    ((*(SCRL->sc_ffn))(SCRL, key, i, j))
#define FBEG              (*(SCRL->sc_fbeg))
#define FEND              (*(SCRL->sc_fend))

/*** FIN Déplacement de s_scroll.h pour les problçmes de compilation le 8/12/2011 ***/

