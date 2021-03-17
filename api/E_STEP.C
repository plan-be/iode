#include "iode.h"


/* IODE_REAL    C_evallec(char *lec,int t)
{
    CLEC    *clec;
    IODE_REAL    x = L_NAN;
    int k;

    SCR_strip(lec);
    if(lec[0]) {
	clec = L_cc(lec);
	    for(k=0;k < clec->nb_names;k++)
		if(K_find(K_WS[K_SCL],clec->lnames[k].name)== -1)
		    {kerror(0,"Coef %s not found",clec->lnames[k].name );
		     return(-1);}
	if(clec == NULL) {
	    B_seterror("Syntax error %.80s", L_error());
	    return(x);
			 }
	if(clec != 0 && !L_link(K_WS[K_VAR], K_WS[K_SCL], clec))
	    x = L_exec(K_WS[K_VAR], K_WS[K_SCL], clec, t);
	SW_nfree(clec);
		}

    return(x);
}
/* JMP 05-10-00 */

int E_GetScl(clec, scl)                                            /*charge le tableau scl des noms de scalaires de l'équation sous forme clec*/
CLEC    *clec;
char    ***scl;


{
    int     j, nbscl = 0;
    if(clec != 0) {
       for(j = 0 ; j < clec->nb_names ; j++){
	   if(L_ISCOEF(clec->lnames[j].name) && KSVAL(K_WS[K_SCL],K_find(K_WS[K_SCL],clec->lnames[j].name))->relax  !=0)
		   SCR_add_ptr(scl, &nbscl,clec->lnames[j].name);
					    }
		  }
    SCR_add_ptr(scl, &nbscl, NULL);
    return(nbscl - 1);
}

E_SetScl(relax, name)                                             /*Met le relax du scalaire name à relax*/
int     relax;
char    *name;

{
    char    buf[128];

    if(relax == 1)    sprintf(buf, "%s 0.9 1.0", name);
    else              sprintf(buf, "%s 0.0 0.0", name);

    B_DataUpdate(buf, K_SCL);
}
 IODE_REAL E_StepWise_1(i, nbscl, scl, smpl, eqs,test)                       /*Effectue l'estimation de l'équation eqs pour la combinaison i des nbscl scalaires en tenant compte des valeurs initiales des relax*/
int     i, nbscl;
char    **scl;
SAMPLE  *smpl;
char    **eqs;
char    *test;
{
    int     j, cscl, rc, nscl;
    char    buf[512];
    IODE_REAL    res[3];
    char    etest[20];

    cscl = 1;
    nscl = 0;
    buf[0] = 0;
    for(j = 0; j < nbscl; j++) {                                 /*Fixe les relax des scalaires*/

	if(i & cscl){
	    E_SetScl(1, scl[j]);
	    strcat(buf, scl[j]);
	    strcat(buf, " ");
	    nscl++;}
	else
	    E_SetScl(0, scl[j]);
	cscl = cscl * 2;
			       }

   if(nscl > 1) {                                              /*Effectue l'estimation si plus d'un relax est != 0 */
       B_EqsEstimateEqs(smpl,eqs);
       etest[0]=0;
       strcat(etest,"e0_");
       strcat(etest,test);
       E_SclToReal(etest, res);
       kmsg("%s: scalars : %s, fstat=%lf", eqs[0], buf, res[0]);
       L_debug("%s: scalars : %s, fstat=%lf\n", eqs[0], buf, res[0]);
		}
   else res[0] = 0.0;
   return(res[0]);
}
 IODE_REAL    E_StepWise(smpl, arg,cond,test)                       /*Effectue toutes les estimations pour toutes les combinaisons en créant des nouveaux coeficients pour toutes les combinaisons qui virifient la condition et en retenant la combinaison qui donne le meilleur fstat pour les coef de l'équation eqs*/
SAMPLE  *smpl;
char    *arg;
char    *cond;
char    *test;
{
    int     i,k,l=0,nbscl, nbcom;
    int     pos, lasti;
    IODE_REAL    r2, lnumtest,numtest;
    EQ      *eq;
    CLEC    *cl;
    char    **scl = NULL, **eqs = NULL, *string;
    double  res[3];



    eqs = B_ainit_chk(arg, NULL, 0);                          /*Cr‚e le tableau d'équations à partir de arg (il faut qu'une seule eqs!!)*/
    if(eqs == NULL) return(0.0);
    pos = K_find(K_WS[K_EQS], eqs[0]);
    if (pos < 0) return(0.0);

    eq = KEVAL(K_WS[K_EQS], pos);                             /*Construit le tableau de scalaire contenus dans l'équation eqs*/
    cl = eq->clec;
    nbscl = E_GetScl(cl, &scl);
    E_free(eq);

    nbcom = pow(2.0, nbscl);
    lnumtest = -999999999999.0;
    lasti = 0;
    for(i = 1; i < nbcom; i++) {                             /* Effectue les estimations pour toutes les combi*/
	numtest = E_StepWise_1(i, nbscl, scl, smpl, eqs,test);
	/*print_result(numtest,numtest,C_evallec(cond,0),scl,nbscl,i,cond);*/  /*Aide pour la programmation*/
	if(lnumtest < numtest && C_evallec(cond,0)!=0 && numtest!=0) {   /*Sauve la combi qui a le meilleur fstat*/
	    lasti = i;
	    lnumtest = numtest;
	    }
	}

    numtest = E_StepWise_1(lasti, nbscl, scl, smpl, eqs,test);            /*Effectue l'estimation pour la meilleur combi*/
    SCR_free_tbl(eqs);
    SCR_free_tbl(scl);
    return(numtest);
}

B_EqsStepWise(arg)                                                 /*Lit les arguments from, to, eqs, et cond et effectue les estimation (fonction principale)*/
char    *arg;
{
    int     lg1, lg2,lg3,lg4,lg5;
    char    from[16], to[16], eqs[30], cond[1024], test[20];
    SAMPLE  *smpl;

    lg1 = B_get_arg0(from, arg, 15);                              /*Lit les arguments*/
    lg2 = B_get_arg0(to, arg + lg1, 15);
    lg3 = B_get_arg0(eqs, arg + lg1 + lg2, 29);
    lg4 = B_get_arg0(cond, arg + lg1 + lg2 + lg3 , 1023);
    lg5 = B_get_arg0(test, arg + lg1 + lg2 + lg3 + lg4, 19);

    smpl = PER_atosmpl(from, to);                                /*Calcul le sample*/

    if(smpl == NULL) {                                           /*GŠre les erreurs de sample*/
	kerror(0,"Incorrect sample");
	return(1);
		     }
    if(K_find(K_WS[K_EQS],eqs)== -1) {                            /*GŠre les erreurs d'‚quation*/
	kerror(0,"Eqs %s not found",eqs);
	return(1);
	}

    if(C_evallec(cond,0)==-1)return(1);                          /*GŠre les erreurs de condition*/

    strcpy(test,SCR_lower(test));
    if(strcmp(test,"r2")!=0 && strcmp(test,"fstat")!=0)          /*GŠre les erreurs de test*/
	{
	 kerror(0,"Incorrect test name");
	 return(1);
	}
    if(check_scl_var(eqs) == -1)return(1);                      /*GŠre les erreurs de pr‚sence des scalaires et variables de l'‚quation*/
    E_StepWise(smpl,eqs,cond,test);                                   /*Effectue les estimations*/
    return(1);
}

print_result (double F,double R,double cond,char **tab_scl,int nb_scls,int combi,char *cond_string) /*Imprime le r‚sultat d'un estimation*/
{
    double res[3];
    char *tmp;
    FILE *output;
    int j;

    tmp=(char *)malloc(30);
    output=fopen("stepwise.txt","a");
    fprintf(output,"\nStep %d : (F_Stat = %f;R2 = %f;Condition(%s) = %f) ",combi,F,R,cond_string,cond);
    for(j=0;j<nb_scls;j++) {
	sprintf(tmp,"%s",tab_scl[j]);
	E_SclToReal(tmp,res);
	fprintf(output,"%s = %f ",tab_scl[j],res[0]);
	}
    SCR_free(tmp);
    fclose(output);
}


int check_scl_var(char *eqs)
{
    int pos,j;
    EQ *eq;
    CLEC *cl;
    char *buf[1024];

    pos = K_find(K_WS[K_EQS], eqs);
    eq = KEVAL(K_WS[K_EQS], pos);
    cl = eq->clec;

    for(j = 0 ; j < cl->nb_names ; j++){
	if(L_ISCOEF(cl->lnames[j].name))
	    {
	     if(K_find(K_WS[K_SCL],cl->lnames[j].name)== -1)
		 {sprintf(buf, "%s 0.9 1", cl->lnames[j].name);
		  B_DataUpdate(buf, K_SCL);}
	    }
	else
	    {
	    if(K_find(K_WS[K_VAR],cl->lnames[j].name)== -1)
		{kerror(0,"Var %s from %s not found",cl->lnames[j].name,eqs);
		 return(-1);}
	    }}
    return(1);
}


















