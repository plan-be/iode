/**
 * @header4iode
 * 
 * Functions for manipulating PERIOD and SAMPLE in IODE.
 *
 * @note Perhaps you feel like these functions were written by total beginners discovering 
 * the beauties of the C language. If so, you're right :-).
 *
 * List of functions
 * ----------------- 
 *  
 * Calculation functions
 * 
 *      int PER_diff_per(PERIOD *p1, PERIOD *p2): calculates the number of sub periods between two periods. 
 *      PERIOD *PER_addper(PERIOD *period, int shift): adds (sub-)periods to period.
 *      int PER_common_smpl(SAMPLE* smp1, SAMPLE* smp2, SAMPLE* res): calculates the intersection between 2 SAMPLEs.
 *  
 * Representation functions
 * 
 *      char *PER_pertoa((PERIOD* per, char* text): creates a text representation of a PERIOD.
 *      PERIOD *PER_atoper(char *text): translates a text in a PERIOD.
 *      char *PER_smpltoa(SAMPLE* smpl, char* text) : writes a SAMPLE in a string
 *      SAMPLE *PER_atosmpl(char* a1, char* a2): creates a new allocated SAMPLE based on two strings containing PERIODs.
 *      SAMPLE *PER_pertosmpl(PERIOD* p1, PERIOD* p2): returns a new allocated SAMPLE build on two given PERIOD.
 * 
 * Miscellaneous functions 
 * 
 *      int PER_nb(int ch): gives the nbr of periods in one year for the periodicity ch
 *      int PER_nbper(PERIOD* period): retrieves the number of periods in one year in a period.
 *      double PER_per2real(PERIOD* from, int i): adds a number of sub-periods to a PERIOD and returns a numerical representation of the result (used mainly for tables and graphs formatting).
 * 
 */
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "scr4/s_alldoc.h"
#include "scr4/s_swap.h"

#include "api/b_errors.h"
#include "api/utils/time.h"

  
/**
 *  Compute position of character ch in string str.
 *  
 *  @param [in] str char*  string to be analyzed
 *  @param [in] ch  int    character to be searched
 *  @return         int    pos of ch in str or -1 if not found or str is NULL.
 *  
 *  TODO: replace L_pos(str, ch) by U_pos(ch, str) ?
 */
int L_pos(char *str, int ch)
{
    int     i;

    if(str == 0) return(-1);        // JMP 6/10/2021
    for(i = 0 ; str[i] != 0 ; i++)
        if(str[i] == ch) return(i);
    return(-1);
}


/**
 *  Gives the nbr of periods in one year for the periodicity ch.
 *  
 *  @param [in] ch  int periodicity (from L_PERIOD_CH). Can be upper or lowercase.
 *  @return int         number of periods for periodicity ch or -1 if ch not in L_PERIOD_CH.
 *  
 */
int PER_nb(int ch)
{
    int pos;

    if(ch >= 'a') ch += 'A' -'a';
    pos = L_pos(L_PERIOD_CH, ch);
    if(pos < 0) return(-1);
    return(L_PERIOD_NB[pos]);
}


/**
 *  Calculates the number of sub periods between two periods. Periods must have the same periodicity.
 *  
 *  Examples: 
 *  
 *      2010Y1 - 2008Y1 = 2
 *      2020Q3 - 2019Q2 = 5
 *  
 *  @param [in] p1 PERIOD * First period
 *  @param [in] p2 PERIOD * Second period
 *  @return        int      Number of periods bw p1 and p2. -1 if periodicities are different or illegal.
 *  
 *  @details On error, call IodeErrorManager::append_error() and returns -1.
 */
int PER_diff_per(PERIOD *p1, PERIOD *p2)
{
    int     nb;
    int     period_pos;

    if(p1->p_p != p2->p_p) {
        error_manager.append_error("Different periodicities");
        return(-1);
    }
    period_pos = L_pos(L_PERIOD_CH, p1->p_p);
    if(period_pos < 0) {
        error_manager.append_error("Illegal periodicity");
        return(-1);
    }
    nb = L_PERIOD_NB[period_pos];
    nb = nb * (p1->p_y - p2->p_y) + p1->p_s - p2->p_s;

    return(nb);
}


/**
 *  Adds shift (sub-)periods to period and returns a pointer to a static PERIOD struct. shift may be negative.
 *  
 *  @note As the result is a static variable, its value is only valid until the next call to PER_addper().
 *  
 *  @param [in] period  PERIOD* First period
 *  @param [in] shift   in      number of sub-periods to add to period
 *  @return             PERIOD* Pointer to a static PERIOD struct or NULL if period is illegal
 *  
 */
PERIOD  *PER_addper(PERIOD *period, int shift)
{
    int     nb_per = L_PERIOD_NB[L_pos(L_PERIOD_CH, period->p_p)],
            shiftper, 
            backw, 
            period_pos;
    static  PERIOD  per;

    period_pos = L_pos(L_PERIOD_CH, period->p_p);
    if(period_pos < 0) {
        error_manager.append_error("Illegal periodicity");
        return(NULL);
    }

        
    nb_per = L_PERIOD_NB[period_pos];
    
    memcpy(&per, period, sizeof(PERIOD));
    if(shift >= 0) {
        per.p_y += (per.p_s + shift - 1) / nb_per;
        per.p_s  =  1 + (per.p_s + shift - 1) % nb_per;
    }
    else {
        shiftper = per.p_s + shift;
        if(shiftper <= 0) { // année recule
            backw = 1 + (-shiftper) / nb_per;
            per.p_y -= backw;
            per.p_s = backw * nb_per + shiftper;
        }
        else { // année inchangée
            per.p_s = shiftper;
        }
    }
    return(&per);
}


/**
 *  Creates a text representation of a PERIOD.
 *  
 *  @param [in]         per     PERIOD period 
 *  @param [in, out]    text    char*  placeholder for the period representation. 
 *                              Must be large enough or null in which case a static buffer is returned.
 *  @return                     char * same as text
 *  
 */
char *PER_pertoa(PERIOD* per, char* text)
{
    static char buf[10];

    if(per->p_y == 0 || per->p_p == 0 || per->p_s == 0)
        buf[0] = 0;
    else
        sprintf(buf, "%ld%c%ld", per->p_y, per->p_p, per->p_s);
    if(text == 0) return(buf);

    strcpy(text, buf);
    return(text);
}


/**
 * Translates a text (e.g. "2010M6") into a PERIOD. 
 *  
 * If year < 50, adds 2000 (49Y1 => 2049Y1).
 * If year < 200, adds 1900 (60Y1 => 1960Y1).
 *
 *    
 *  @param [in] aper    char*       period in a string (Ex. : "1990M2")
 *  @return             PERIOD*     pointer to an allocated struct PERIOD or null on error.
 *  
 *  @details On error, call IodeErrorManager::append_error() and returns NULL.
 *  @details The resulting PERIOD being allocated, it is the programmer's responsibility to free it later (SW_nfree()).
 */
 
PERIOD *PER_atoper(char *aper)
{
    int     i, j, nb_per, year, sub;
    PERIOD  *per = (PERIOD *) SW_nalloc(sizeof(PERIOD));
    char    text[64];

    per->p_y = 0;
    per->p_p = 0;
    per->p_s = 0;
    if(aper == 0 || aper[0] == 0 || aper[0] == ' ') return(per); // JMP 21/04/023 to avoid unwanted messages
    
    // work on a local copy of aper 
    SCR_strlcpy((unsigned char*) text, (unsigned char*) aper, sizeof(text) -1);

    // Search the periodicity sign and keeps the digits
    for(i = 0 ; i < 4 ; i++)
        if(!SCR_is_num(text[i])) break;
    if(i == 0) goto fin;

    // Check that the periodicity is valid
    nb_per = L_pos(L_PERIOD_CH, toupper(text[i]));
    if(nb_per < 0) goto fin;

    // Retrieve the year (first digits). 
    // If year < 50, adds 2000 (49Y1 => 2049Y1)
    // if year < 200, adds 1900 (60Y1 => 1960Y1)
    text[i] = 0;
    year = atoi(text);
    if(year < 50) year += 2000;
    else if(year < 200) year += 1900;
    text[i] = L_PERIOD_CH[nb_per];

    // calc sub period
    // If == 0 => 1
    // If > max subperiod, error
    i++;
    j = i;
    while(SCR_is_num(text[i])) i++;
    text[i] = 0;
    sub = atoi(text + j);
    if(sub <= 0) sub = 1;
    if(L_PERIOD_NB[nb_per] < sub) goto fin;

    per->p_y = year;
    per->p_p = L_PERIOD_CH[nb_per];
    per->p_s = sub;

fin:
    if(per->p_y == 0) {
        std::string error_msg = "'";
        error_msg += std::string(text);
        error_msg += "' : illegal period";
        error_manager.append_error(error_msg);
        SW_nfree(per);
        per = 0;
    }
    return(per);
}


/**
 *  Calculates the intersection between 2 SAMPLEs.
 *  
 *  @param [in]  smp1 SAMPLE* Sample 1
 *  @param [in]  smp2 SAMPLE* Sample 2
 *  @param [out] res  SAMPLE* Intersection of the 2 samples
 *  @return           int 0 if ok. On error returns -1 and set B_errno.
 *  
 *  @details On error, call IodeErrorManager::append_error() and returns -1.
 */
int PER_common_smpl(SAMPLE* smp1, SAMPLE* smp2, SAMPLE* res)
{
    if((smp1->s_p1).p_p != (smp2->s_p1).p_p) {
        error_manager.append_error("Different periodicities");
        return(-1);
    }

    if(PER_diff_per(&(smp1->s_p1), &(smp2->s_p1)) < 0)
        res->s_p1 = smp2->s_p1;
    else
        res->s_p1 = smp1->s_p1;

    if(PER_diff_per(&(smp1->s_p2), &(smp2->s_p2)) > 0)
        res->s_p2 = smp2->s_p2;
    else
        res->s_p2 = smp1->s_p2;

    if((res->s_nb = PER_diff_per(&(res->s_p2), &(res->s_p1))) < 0) {
        error_manager.append_error("Sample out of workspace boundaries");
        return(-1);
    }

    res->s_nb += 1;
    return(0);
}

/**
 *  Returns a new allocated SAMPLE build on two given strings representing PERIODs ("1990Y1", "2025Y1").
 *  
 *  @param [in] a1 char*    text of period 1 
 *  @param [in] a2 char*    text of period 2
 *  @return        SAMPLE   allocated SAMPLE on success, null on error.
 *  
 *  @details On error, call IodeErrorManager::append_error() and returns NULL.
 *  @details The resulting SAMPLE being allocated, it is the programmer's responsability to free it later (SW_nfree()).
 */
SAMPLE  *PER_atosmpl(char* a1, char* a2)
{
    SAMPLE  *smpl = 0;
    PERIOD  *p1, *p2 = 0;

    p1 = PER_atoper(a1);
    if(p1 == 0) goto err;
    p2 = PER_atoper(a2);
    if(p2 == 0) goto err;

    smpl = PER_pertosmpl(p1, p2);

err:
    if(smpl == 0)
    {
        std::string error_msg = "Illegal sample : ";
        if (a1 != 0) error_msg += std::string(a1);
        if (a2 != 0) error_msg += " " + std::string(a2);
        error_manager.append_error(error_msg);
    }
    SW_nfree(p1);
    SW_nfree(p2);
    return(smpl);
}


/**
 *  Returns a new allocated SAMPLE build on two given PERIOD.
 *  
 *  @param [in] p1 PERIOD* period 1. Must be <= period 2.
 *  @param [in] p2 PERIOD* period 2.
 *  @return        SAMPLE* allocated sample [period1, period2]. If the periodicity of p1 or p2 is null, 
 *                         the resulting sample has no observation (smpl->s_nb = 0)
 *              
 *  @details The resulting SAMPLE being allocated, it is the programmer's responsability to free afterwards (SW_nfree()).
 */
SAMPLE  *PER_pertosmpl(PERIOD* p1, PERIOD* p2)
{
    SAMPLE  *smpl = (SAMPLE *) SW_nalloc(sizeof(SAMPLE));
    int     dif;

    memcpy(&(smpl->s_p1), p1, sizeof(PERIOD));
    memcpy(&(smpl->s_p2), p2, sizeof(PERIOD));
    smpl->s_nb = 0;                             
    if(p1->p_p != 0 && p2->p_p != 0) {                  // Modified to accept periods {0,0,0}
        dif = PER_diff_per(&(smpl->s_p2), &(smpl->s_p1));
        if(dif < 0) {
            SW_nfree(smpl);
            return(NULL);
        }
        smpl->s_nb = dif + 1;
    }

    return(smpl);
}


/**
 *  Writes a SAMPLE in a string.
 *  
 *  @param [in]     smpl SAMPLE*    pointer to the sample to print
 *  @param [out]    text char*      pointer to the result
 *  @return char*                   pointer to text
 *  
 */
char *PER_smpltoa(SAMPLE* smpl, char* text)
{
    char    b1[20], b2[20];

    sprintf(text, "%s:%s", PER_pertoa(&(smpl->s_p1), b1),
            PER_pertoa(&(smpl->s_p2), b2));
    return(text);
}


/**
 *  Retrieves the number of periods in one year in a period.
 *  
 *  @param [in] period  PERIOD*  Period to analyse.
 *  @return             int      Nb of sub period in 1 year or 0 if period is NULL
 *  
 */
int PER_nbper(PERIOD* period)
{
    if(period == NULL) return(0);
    return(PER_nb(period->p_p));
}

/**
 *  Adds a number of sub periods to a PERIOD and returns a numerical representation of the result.
 *  Used mainly for tables and graphs formatting.
 *  
 *  Examples : 
 *   
 *      2001Q3 + 2 => 2002.1
 *      2001Y1 + 0 => 2001.0
 *      2001Y1 + 2 => 2003.0
 *  
 *  @param [in] from    PERIOD*
 *  @param [in] i       int
 *  @return             double
 *  
 */
double PER_per2real(PERIOD* from, int i)
{
    int     per;
    double    value;

    per = PER_nbper(from);
    if(per == 0) return(0.0);
    value = (double) from->p_y + 
            (from->p_s + i - 1) / per + 
            ((from->p_s + i - 1) % per)/ (double) per;

    return(value);
}
