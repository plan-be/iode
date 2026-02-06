/**
 *  @header4iode
 *  
 *  Extraction of equations, scalars and identities from E-Views data.
 *  
 *  Format of E-Views data
 *  
 *  --------------------------------------------------------------------------------------------------------
 *  Forecasting Equation:
 *  =========================
 *  D(B_PQVC) = C(1)*D(PQVC) + C(2)*(D2001+D2002) + C(3)*D(B_PQVC(-3))
 *  
 *  Substituted Coefficients:
 *  =========================
 *  D(B_PQVC) = 0.894661573413*D(PQVC) - 0.0284533462569*(D2001+D2002) + 0.241546373731*D(B_PQVC(-3))
 *  
 *  @IDENTITY gro_w_produz_def  = w_produz_def  / w_produz_def(-1)  - 1
 *  
 *  XYZ = xyz(1) + xyz(2) * EXP(ABC)
 *  
 *  ' Comments
 *  --------------------------------------------------------------------------------------------------------
 *  
 *  The file is interpreted as described below :
 *  
 *  - The characters after a quote (') are ignored.
 *  - The 2 equations following the "Forecasting Equation" and "Substituted Coefficients" titles are
 *  extracted.
 *  - The first equation is translated into IODE format :
 *  - D(...) is replaced by d(...)
 *  - EXP(...) is replaced by exp(...)
 *  - LOG(...) is replaced by ln(...)
 *  - DLOG(...) is replaced by d ln(...)
 *  - @TREND is replaced by t
 *  - @PCH is replaced by 0.01 * grt
 *  - NAME(<n>) is replaced by the name_<n>
 *  - Expr(+<n>) is replaced bu Expr[+<n>]
 *  - Expr(-<n>) is replaced bu Expr[-<n>]
 *  - the first variable is considered as the endogenous variable
 *  - the first = sign is considered as the separator between left and right members of the equation
 *      and therefore replaced by :=. This is only true if the = sign is not between ().
 *  
 *  - The coefficients values are extracted from the second equation.
 *  - The lines beginning with "@IDENTITY" are extracted and translated into IODE equations.
 *  - The other lines (not included in syntax above) are extracted and translated into IODE equations
 *  
 *  Each equation is added in the current Equation WS.
 *  Each coefficient (if any) is saved in the Scalar WS.
 *  
 *  Report function  :
 *  
 *      $WsImportEviews filename
 *  
 *  List of functions
 *  -----------------
 *      int EV_cc_eq(char *eveq, char *endo, char *lec)           | Transform an E-views equation lasteq into LEC format and returns the first encountered var name as endogenous.  
 *      EV_cc_coefs(char *lasteq, char *lastsubeq, double *coefs) | Look for the coefficients numbers in lasteq and their respective values in lastsubeq.
 *      EV_cc_idt(char *idt, char *endo, char *lec)               | Transform E-views identity idt into LEC format and returns the first encountered var name as endogenous.
 *      int EV_read_line(char *buf)                               | Read a line in EV_FD and delete the optional comment as from the first ' char.
 *      int EV_cc_file(char *filename)                            | Interpret an E-Views file and store all read objects into the current equation and scalar WS. 
 *      int B_WsImportEviews(char* arg, int unused)                           | $WsImportEviews filename
 */
 
// 2012-05-04 : version 1
// 2015-11-17 : version 2 : added : EXP, LOG, DLOG, NAME(n), comments, line without @IDENTITY
// 2016-06-21 : version 3 : lines without @IDENTITY are only interpreted after :
//    - either  : a line containing the text "Identities:"
//    - or : a first @IDENTITY line
// 2017-02-05 : added directives "KEEP COEFS" AND "DROP COEFS":
//    - KEEP COEFS (line containing only that text) => NAME(1) => name_1
//    - DROP COEFS (line containing only that text) => NAME(1) => name
//      DROP is the default value
//    - LINES "Forecasting equation:" are ignored
//    - LINES "Estimation equation:" replace "Forecasting equations:"
//    - correction for negative coefs when there is a difference (in # of spaces) bw ESTIMATION and SUBSTITUTED eqs

//================================================================================================================= */

#include "api/constants.h"
#include "api/k_super.h"
#include "api/objs/objs.h"

#include "api/report/commands/commands.h"
#include "api/report/undoc/undoc.h"


#define EV_MAXBUF 40960

// Strings found in the E-Views data files
// ---------------------------------------
char EV_ESTCOMMAND[] = "ESTIMATION COMMAND:";
char EV_ESTEQ[] = "ESTIMATION EQUATION:";
char EV_ESTFOREQ[] = "FORECASTING EQUATION:";
char EV_ESTSUBCOEFS[] = "SUBSTITUTED COEFFICIENTS:";
char EV_IDENTITY[] = "@IDENTITY";
char EV_INNOV[] = "@INNOV ";
char EV_IDENTITIESSTART[] = "IDENTITIES:";
char EV_KEEPCOEFS[] = "KEEP COEFS:";
char EV_DROPCOEFS[] = "DROP COEFS:";
FILE *EV_FD;
int  EV_LINENB = 0,
     EV_INIDENTITIES = 0,
     EV_ISKEEPCOEFS = 0;


// Functions
// ---------

/**
 *  Transform an E-views equation lasteq into LEC format and returns the first encountered var name as endogenous.
 *
 *  @param [in]  eveq   char *      E-Views format (coefs C(1), ...)
 *  @param [out] endo   char *      first var in lasteq
 *  @param [out] lec    char *      lec form
 *  @return             int         0 if OK, -1 if syntax error in lasteq
 */
int EV_cc_eq(char *eveq, char *endo, char *lec)
{
    YYFILE	*yy;
    char	scl[80], word[256];
    int		keyw, ch, var = 0, lag, firsteq = 1, isscl = 0, parlevel = 0;

    lec[0] = endo[0] = 0;
    strcpy(scl, "c");
    yy = YY_open(eveq, 0L, 0, 1);
    if(yy == 0) return -1;

    // Skip spaces
    while(1) {
        //YY_skip_spaces(yy);
        keyw = YY_read(yy);
        switch(keyw) {
            case YY_EOF:
                goto fin;
            case YY_WORD:
                strcpy(word, (char*) yy->yy_text);
                SCR_upper((unsigned char*) word);

                // D(...) -> d(...)
                if(strcmp(word, "D") == 0) {
                    strcat(lec, "d");
                    var = 0;
                }

                else if(strcmp(word, "LOG") == 0) {
                    strcat(lec, "ln");
                    var = 0;
                }
                else if(strcmp(word, "EXP") == 0) {
                    strcat(lec, "exp");
                    var = 0;
                }

                else if(strcmp(word, "DLOG") == 0) {
                    strcat(lec, "d ln");
                    var = 0;
                }

                // C(123) -> c_123
                else if(strcmp((char*) yy->yy_text, "C") == 0) {
                    YY_skip_spaces(yy);
                    if(YY_read(yy) != YY_SPECIAL || yy->yy_text[0] != '(') return -1; // Erreur de syntaxe
                    YY_skip_spaces(yy);
                    if(YY_read(yy) != YY_LONG) return -1; // Erreur de syntaxe
                    YY_skip_spaces(yy);
                    sprintf(lec + strlen(lec), "%s_%d", scl, yy->yy_long);
                    if(YY_read(yy) != YY_SPECIAL || yy->yy_text[0] != ')') return -1; // Erreur de syntaxe
                    var = 0;
                }

                // VARNAME -> Endo si 1er
                // VARNAME(1) => varname_1
                else {
                    YY_skip_spaces(yy);
                    isscl = 0;
                    if(YY_read(yy) == YY_SPECIAL && yy->yy_text[0] == '(') {
                        // NAME(
                        YY_skip_spaces(yy);
                        if(YY_read(yy) == YY_LONG) {
                            // NAME(n => param
                            isscl = 1;
                            if(EV_ISKEEPCOEFS)                                                        // JMP 2017-02-05
                                sprintf(lec + strlen(lec), "%s_%d", SCR_lower((unsigned char*) word), yy->yy_long);  // JMP 2017-02-05
                            else                                                                    // JMP 2017-02-05
                                sprintf(lec + strlen(lec), "%s", SCR_lower((unsigned char*) word));                  // JMP 2017-02-05

                            YY_skip_spaces(yy);
                            if(YY_read(yy) != YY_SPECIAL || yy->yy_text[0] != ')') return -1; // Syntax
                        }
                        else if(U_is_in(yy->yy_text[0], "-+")) {
                            // NAME(+ ou NAME[-
                            ch = yy->yy_text[0];
                            YY_skip_spaces(yy);
                            if(YY_read(yy) != YY_LONG) return -1; // Syntax
                            lag = yy->yy_long;
                            sprintf(lec + strlen(lec), "%s[%c%d]", word, ch, lag);
                            YY_skip_spaces(yy);
                            if(YY_read(yy) != YY_SPECIAL || yy->yy_text[0] != ')') return -1;
                        }
                        else return -1; // syntax
                    }
                    else {
                        YY_unread(yy);
                        sprintf(lec + strlen(lec), "%s", word);
                    }

                    if(endo[0] == 0 && !isscl) {
                        strcpy(endo, word);
                        strcpy(scl, endo);
                        SCR_lower((unsigned char*) scl);
                    }
                    var = 1; // Last token = varname
                }
                break;

            case YY_SPECIAL :
                ch = yy->yy_text[0];
                if(ch == '(') {
                    if(var == 1) {
                        // (-n)  => [-n]
                        YY_skip_spaces(yy);
                        if(YY_read(yy) != YY_SPECIAL) return -1;
                        ch = yy->yy_text[0];
                        if(U_is_in(ch, "-+")) {
                            YY_skip_spaces(yy);
                            if(YY_read(yy) != YY_LONG) return -1;
                            lag = yy->yy_long;
                            YY_skip_spaces(yy);
                            if(YY_read(yy) != YY_SPECIAL || yy->yy_text[0] != ')') return -1;
                            sprintf(lec + strlen(lec), "[%c%d]", ch, lag);
                        }
                        var = 1;
                    }
                    else {
                        // Début expression ()
                        parlevel++;
                        sprintf(lec + strlen(lec), "(");
                    }
                }

                // Fin expression ()
                else if(ch == ')') {
                    parlevel--;
                    sprintf(lec + strlen(lec), ")");
                }

                // @PCH -> grt @TREND -> t
                else if(ch == '@') {
                    if(YY_read(yy) != YY_WORD) return -1;
                    if(strcmp((char*) yy->yy_text, "PCH") == 0) {
                        sprintf(lec + strlen(lec), " 0.01 * grt"); // JMP 19/2/2013
                        var = 0;
                    }
                    else if(strcmp((char*) yy->yy_text, "TREND") == 0) {
                        sprintf(lec + strlen(lec), " t");
                        var = 1;
                    }
                    else {
                        return -1;
                    }
                }

                // single quote (user mistake or comment in EViews ?) => ignored
                else if(ch == '\'') {
                    continue;
                }

                // =
                else if(ch == '=' && firsteq && parlevel == 0) {
                    sprintf(lec + strlen(lec), " := ");
                    firsteq = 0;
                    var = 0;
                }

                // =+*-/) ->
                /* JMP 19/11/2015
                else if(U_is_in(ch, "=+*-/")) {
                	sprintf(lec + strlen(lec), " %c ", ch);
                	var = 0;
                }
                */
                // Autres cas (+-= ...)
                else {
                    sprintf(lec + strlen(lec), "%c", ch);
                    var = 0;
                }

                break;

            case YY_LONG:
            case YY_DOUBLE:
            default :
                strcat(lec, (char*) yy->yy_text);
                var = 0;
                break;
        }
    }

fin:
    YY_close(yy);
    return 0;
}


/**
 *  Look for the coefficients numbers in lasteq and their respective values in lastsubeq.
 *  
 *  @param [in]  lasteq    char*    E-Views equation text
 *  @param [in]  lastsubeq char*    E-Views equation text with subsituted coefficients
 *  @param [out] coefs     double*  values of the coefs
 *  @return                int      -1 on error, nb of coefs if ok
 */
int EV_cc_coefs(char *lasteq, char *lastsubeq, double *coefs)
{
    int		idx = 0, pos1 = 0, i, csign;

    while(1) {
        // find next coef (ie "C(") and move pointer in lasteq and lastsubeq
        pos1 = U_index((unsigned char*) lasteq, (unsigned char*) "C(");
        if(pos1 < 0) break;

        // if ABC(... -> not a coef, thus skip
        if(SCR_is_anum(lasteq[pos1 - 1])) {
            lasteq += pos1 + 2;
            lastsubeq += pos1 + 2;
            continue;
        }

        // Calc index  in C vector
        idx = atoi(lasteq + pos1 + 2);
        if(idx > 100) return -1;

        // Check if op before C(...) == '-' => csign = -1
        csign = 1;
        for(i = pos1 - 1; i >= 0 ; i--) { // JMP 8/2/2018
            if(lasteq[i] == '-') {
                csign = -1;
                break;
            }
            else if(lasteq[i] != ' ') {
                break;
            }
        }

        // Calc value in lastsubeq
        coefs[idx] = csign * atof(lastsubeq + pos1);

        // If last op == '-', coefs[idx] = -coefs[idx]
        for(i = pos1 - 1; i >= 0 ; i--) { // JMP 8/2/2018
            if(lastsubeq[i] == '-') {
                coefs[idx] = -coefs[idx];
                break;
            }
            else if(lastsubeq[i] != ' ') {
                break;
            }
        }
        //if(lastsubeq[pos1 - 2] == '-') coefs[idx] = -coefs[idx]; // JMP 8/2/2018

        // Move pointers
        // lastsubeq -> after coef value
        //lastsubeq += pos1 + 1;
        lastsubeq += pos1;
        while(U_is_in(lastsubeq[0], "-0123456789.eE"))  // JMP 8/2/2018 ('-' added for 1.23e-4)
            lastsubeq++;

        // lasteq -> after C(...)
        lasteq += pos1 + 2;
        pos1 = U_pos(')', (unsigned char*) lasteq);
        if(pos1 < 0) return -1;
        lasteq += pos1 + 1; // après C(...)
    }

    return(idx);
}


/**
 *  Transform E-views identity idt into LEC format and returns the first encountered var name as endogenous.
 *
 *  @param [in]  eveq   char *      E-Views format 
 *  @param [out] endo   char *      first var in lasteq
 *  @param [out] lec    char *      lec form
 *  @return             int         0 if OK, -1 if syntax error in lasteq
 */
int EV_cc_idt(char *idt, char *endo, char *lec)
{
    SCR_upper((unsigned char*) idt);
    SCR_replace((unsigned char*) idt, (unsigned char*) "@IDENTITY", (unsigned char*) "");
    return(EV_cc_eq(idt, endo, lec));
}


/**
 *  Read a line in EV_FD and delete the optional comment as from the first ' char.
 *  
 *  @param [out] buf    char*      line read
 *  @return             int        -1 if EOF, buf length otherwise
 */
int EV_read_line(char *buf)
{
    int		n;

    if(EV_FD == 0) return -1;
    EV_LINENB++;
    n = SCR_read_line(EV_FD, (unsigned char*) buf, EV_MAXBUF - 1);
    if(n < 0) {
        fclose(EV_FD);
        EV_FD = 0;
        return -1;
    }
    // Skip end of line from '
    for(n = 0; buf[n] && buf[n] != '\''; n++);
    buf[n] = 0;

    SCR_strip((unsigned char*) buf);
    SCR_upper((unsigned char*) buf);
    return(n);
}


/**
 *  Interpret an E-Views file and store all read objects into the current equation and scalar WS. 
 *  
 *  @param [in] filename char*  E-Views filename
 *  @return              int    -1 on error (file not found), 0 on success
 */
int EV_cc_file(char *filename)
{
    int		rc = -1, nbcoefs, i;
    char	buf[EV_MAXBUF];
    char	lasteq[EV_MAXBUF];
    char	lastsubeq[EV_MAXBUF];
    char 	lec[EV_MAXBUF], endo[80];
    double  coefs[101];

    EV_INIDENTITIES = 0;
    EV_FD = fopen(filename, "r");
    if(EV_FD == 0) return -1; // Cannot open file

    while(1) 
    {
        if(EV_read_line(buf) < 0) 
        {
            rc = 0;
            break;
        }

        // Empty and comment lines
        if(buf[0] == 0 || buf[0] == '\'') 
            continue;

        // Line "Estimation Command:" -> skip next 2 lines
        // Line "Forecasting Equation:" -> skip next 2 lines
        if(memcmp(buf, EV_ESTCOMMAND, strlen(EV_ESTCOMMAND)) == 0 ||
                memcmp(buf, EV_ESTFOREQ,      strlen(EV_ESTFOREQ)) == 0) 
        {
            if(EV_read_line(buf) < 0) break; // ===
            if(EV_read_line(buf) < 0) break; // list of endog : not used
            continue;
        }

        // Line "Estimation equation:" -> skip 1 line and save next
        if(memcmp(buf, EV_ESTEQ, strlen(EV_ESTEQ)) == 0) 
        {
            if(EV_read_line(buf) < 0) break; // ===
            if(EV_read_line(lasteq) < 0) break; // EQ with C(1) ...
            continue;
        }

        // Line "Substituted Coefficients:" -> skip 1 line and save next
        if(memcmp(buf, EV_ESTSUBCOEFS, strlen(EV_ESTSUBCOEFS)) == 0) 
        {
            if(EV_read_line(buf) < 0) break; // ===
            if(EV_read_line(lastsubeq) < 0) break; // EQ with C(1) ...
            if(EV_cc_eq(lasteq, endo, lec)) break;
            memset(coefs, 0, sizeof(coefs));
            nbcoefs = EV_cc_coefs(lasteq, lastsubeq, coefs);
            if(nbcoefs < 0) break;
            
            // Save in WS
            Equation* eq = global_ws_eqs->get_obj_ptr(endo);
            if(!eq)
                break;
            
            try
            {
                eq->set_lec(lec);
            }
            catch(std::exception&)
            {
                if(RP_DEBUG) kmsg("File '%s'[%d] : cannot compile LEC => '%s'", filename, EV_LINENB, lec);
                break;
            }

            eq->comment = std::string(lasteq);
            
            for(i = 0 ; i < nbcoefs ; i++) 
            {
                sprintf(buf, "%s_%d %lf 1", SCR_lower((unsigned char*) endo), i + 1, coefs[i + 1]);
                if(B_DataUpdate(buf, SCALARS)) 
                    goto err;
            }
            continue;
        }

        // Line beginning with @INNOV -> skip 1
        if(memcmp(buf, EV_INNOV, strlen(EV_INNOV)) == 0)
            continue;

        // Line "Identities:" -> skip 1 line
        if(memcmp(buf, EV_IDENTITIESSTART, strlen(EV_IDENTITIESSTART)) == 0) 
        {
            EV_INIDENTITIES = 1;
            if(EV_read_line(buf) < 0) break; // ===
            continue;
        }

        // Line "Keep Coefs"
        if(memcmp(buf, EV_KEEPCOEFS, strlen(EV_KEEPCOEFS)) == 0) 
        {
            EV_ISKEEPCOEFS = 1;
            continue;
        }

        // Line "Drop Coefs"
        if(memcmp(buf, EV_DROPCOEFS, strlen(EV_DROPCOEFS)) == 0) 
        {
            EV_ISKEEPCOEFS = 0;
            continue;
        }

        // Default ou Line "@IDENTITY" ou eq -> save eq
        if(EV_INIDENTITIES || memcmp(buf, EV_IDENTITY, strlen(EV_IDENTITY)) == 0) 
        {
            if(memcmp(buf, EV_IDENTITY, strlen(EV_IDENTITY)) == 0) 
            {
                strcpy(buf, buf + strlen(EV_IDENTITY)); // Skip @IDENTITTY
                EV_INIDENTITIES = 1;
            }
            if(EV_cc_idt(buf, endo, lec)) 
            {
                if(RP_DEBUG) kmsg("File '%s'[%d] : cannot translate Eviews into LEC => Eviews => '%s', Endo => '%s', LEC => '%s'", filename, EV_LINENB, buf, endo, lec);
                break;
            }
            
            // Save in WS
            Equation* eq = global_ws_eqs->get_obj_ptr(endo);
            if(!eq)
                break;
            
            try
            {
                eq->set_lec(lec);
            }
            catch(std::exception&)
            {
                if(RP_DEBUG) kmsg("File '%s'[%d] : cannot compile LEC => '%s'", filename, EV_LINENB, lec);
                break;
            }

            eq->comment = std::string(buf);
        }

        if(RP_DEBUG) kmsg("File '%s'[%d] : ok. LEC  => '%s'", filename, EV_LINENB, lec);
        continue;
    }

err:
    if(EV_FD) fclose(EV_FD);
    EV_FD = 0;
    return rc;
}


// $WsImportEviews filename
int B_WsImportEviews(char* arg, int unused)
{
    return(EV_cc_file(arg));
}