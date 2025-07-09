/**
 *  @header4iode
 * 
 *  Management of the rules used to select objects to import/export and to optionally adapt their names.
 *   
 *  List of functions 
 *  -----------------
 *      int IMP_readrule(char* filename)                                Reads a "rule file" and stores its contents in 2 global variables IMP_rule and IMP_pat.
 *      int IMP_change(char** rule, char** pat, char* in, char* out)    Modifies an object name according to the rule definitions.
 */
#include "api/constants.h"
#include "api/k_super.h"
#include "api/write/write.h"
#include "api/conversion/import.h"


/**
 *  
 *  Reads a "rule file" and stores its contents in 2 global variables IMP_rule and IMP_pat.
 *  
 *  Each rule is defined on one line and composed of two fields:
 *          - the selection mask (pattern) containing a description of the names concerned by the rule. 
 *            This mask is defined with the same syntax as the search in IODE objects (see Data menu), i.e. by possibly including * and ? to specify sets.
 *          - the name transcoding algorithm for this rule, which may contain: 
 *              + : indicating that the character at this position in the source name should be included as is in the output name 
 *              - : any character: indicates that the character at this position in the source name should be skipped 
 *              any other character is kept as is in the resulting name
 *  
 *  Example
 *  -------
 *      B*  C+-+            =>> changes B1234 to CB2, BCDEF to CBE, etc 
 *      *X  ++++++++++      =>> keeps names ending in X unchanged
 *  
 *  
 *  If the file is not found or undefined (filename == NULL), the default rule applies:
 *  
 *      * ++++++++++++++++++
 *    
 *  @param  [in]    char*   filename    rule file name
 *  @global [out]   char**  IMP_rule    transcoding rules
 *  @global [out]   char**  IMP_pat     pattern to determine which rule must be applied to a specific name
 *  @return         int                 0 always
 */
int IMP_readrule(char* filename)
{
    int         nbr = 0, nbp = 0, blen;
    char        buf[257], **tbl;
    FILE        *fd = NULL;

    if(filename == NULL) goto done;

    SCR_strip((unsigned char*) filename);
    if(filename[0] == 0) goto done;

    fd = fopen(filename, "r");
    if(fd == NULL) {
        kerror(0, "Rule file %s does not exist", filename);
        goto done;
    }

    while(1) {
        if(fgets(buf, 256, fd) == NULL)  goto done;
        while(1) { /* strip newline, carriage-return */
            blen = (int)strlen(buf);
            if(isspace(buf[blen - 1])) buf[blen - 1] = '\0';
            else break;
        }
        tbl = (char**) SCR_vtoms((unsigned char*) buf, (unsigned char*) " \t");
        if(SCR_tbl_size((unsigned char**) tbl) != 2) {
            SCR_free_tbl((unsigned char**) tbl);
            continue;
        }

        SCR_add_ptr((unsigned char***) &IMP_pat, &nbp, (unsigned char*) tbl[0]);
        SCR_add_ptr((unsigned char***) &IMP_rule, &nbr, (unsigned char*) tbl[1]);
        SCR_free_tbl((unsigned char**) tbl);
    }

done:
    if(nbr == 0 || (nbr != nbp)) {
        SCR_free_tbl((unsigned char**) IMP_pat);
        SCR_free_tbl((unsigned char**) IMP_rule);
        IMP_pat  = IMP_rule = NULL;
        nbp = nbr = 0;
        SCR_add_ptr((unsigned char***) &IMP_pat,  &nbp, (unsigned char*) "*");
        // SCR_add_ptr(&IMP_rule, &nbr, "++++++++++");
        SCR_add_ptr((unsigned char***) &IMP_rule, &nbr, (unsigned char*) "++++++++++++++++++++"); /* JMP 11-08-11 */
    }

    if(fd) fclose(fd);
    SCR_add_ptr((unsigned char***) &IMP_pat, &nbp, NULL);
    SCR_add_ptr((unsigned char***) &IMP_rule, &nbr, NULL);
    return(0);
}

/**
 *  Determines if a string (in) satifies one of the rules. 
 * 
 *  @see SCR_grep() for the available expressions.
 *  
 *  Sub function of IMP_change().
 *  
 *  @param [in] char** rule     list of rules
 *  @param [in] char*  in       name to be tested
 *  @return     int             position of the first rule that "in" satisfies
 */
static int IMP_grep(char** rule, char* in)
{
    int i, rc = -1;

    for(i = 0; i < SCR_tbl_size((unsigned char**) rule); i++) {
        rc = SCR_grep(rule[i], in, 0);
        if(rc == 0) return(i);
    }
    return(-1);
}


/**
 *  Modifies an object name (in) according to the rule definitions.
 *
 *  Example
 *  -------
 *      rule => {"B*",   "*X"} 
 *      pat  => {"C+-+", "+++++++++"}
 *      
 *      in = "B1234" => rc = 0,  out = "CB2" 
 *      in = "AA"    => rc = -1, out = unchanged ("AA" not recognized in "B*" or "C+-+")
 *      in = "A123X" => rc = 0,  out = "A123X"
 *     
 *  @param [in]  char**  rule   table of name rules 
 *  @param [in]  char**  pat    table of patterns
 *  @param [in]  char*   in     input name
 *  @param [out] char*   out    output name
 *  @return      int            0 on success, -1 on error (name in not found in pat)
 */
int IMP_change(char** rule, char** pat, char* in, char* out)
{
    int rc = -1,
        r, i, o, rlen, ilen, idone = 0;

    rc = IMP_grep(pat, in);
    if(rc < 0) return(-1);

    rlen = (int)strlen(rule[rc]);
    ilen = (int)strlen(in);
    for(r = i = o = 0; r < rlen && !idone && o < K_MAX_NAME ; r++) {
        switch(rule[rc][r]) {
            case '-' :
                if(i >= ilen) idone = 1;
                else i++;
                break;
            case '+' :
                if(i >= ilen) idone = 1;
                else {
                    out[o] = in[i];
                    i++;
                    o++;
                }
                break;
            default  :
                out[o] = rule[rc][r];
                o++;
                break;
        }
    }

    out[o] = '\0';

    if(IMP_trace)
        W_printf("%s -> %s\t(Rule %s)\n", in, out, rule[rc]);

    return(0);
}

