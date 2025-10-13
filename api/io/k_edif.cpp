/**
 *  @header4iode
 * 
 *  Implementation of the DIF export functions. DIF in an old Excel / Lotus 123 interchange format.
 *  
 *  See k_emain.c for more information.
 *  See and k_ecsv.c for a detailed example.
 *   
 *  
 *  Example of a DIF file
 *  ---------------------
 *  
 *        TABLE
 *        0,1
 *        ""
 *        VECTORS
 *        0,13
 *        ""
 *        TUPLES
 *        0,395
 *        ""
 *        DATA
 *        0,0
 *        ""
 *        -1,0
 *        BOT
 *        1,0
 *        "CODE"
 *        1,0
 *        "COMMENT"
 *        1,0
 *        "1990Y1"
 *        ...
 *        
 *
 *   This DIF file, once read into Excel environment, has the structure below:
 *
 *        CODE	COMMENT	1990Y1	1991Y1	1992Y1	1993Y1	1994Y1	1995Y1	1996Y1	1997Y1	1998Y1	1999Y1	2000Y1
 *        ACAF	Ondernemingen: ontvangen kapitaaloverdrachten.	23.771	26.240999	30.159	34.661999	8.1610022	-13.130997	32.171001	39.935799	29.645657	13.530405	10.046611
 *        ACAG	Totale overheid: netto ontvangen kapitaaloverdrachten.	-28.172186	-30.934	-40.285999	-43.157997	-16.029003	-41.845993	-40.237	-32.93	-38.345695	-39.858174	-41.534787
 *        AOUC	Kost per eenheid produkt.	1	1.0244334	1.0314501	1.0309177	1.0462842	1.0498914	1.0536817	1.0798151	1.0897606	1.1080358	1.1162376
 *        AOUC_		0.93735915	0.96466659	0.97403904	0.97881286	0.98955638	0.99526324	1.0032037	1.0282588	1.0384929	1.0809661	1.1019572
 *        AQC	Kost per eenheid produkt: kapitaal en arbeid.	1	1.0628064	1.1102825	1.1532652	1.1571276	1.1616869	1.1580297	1.201328	1.2031082	1.34297	1.3386029
 *
 * 
 *  List of functions
 *  -----------------
 *      int write_header(ExportToFile *expdef, KDB* dbv, KDB* dbc, char* outfile)       Opens and initialise a DIF export file.
 *      int close(ExportToFile* expdef, KDB* dbv, KDB* dbc)                     Saves the footer and closes the DIF export files.
 *      char *write_object_name(char* name, char** code)                             Variable name translation for DIF output.
 *      char *extract_comment(KDB* dbc, char* name, char**cmt)                      Creates the CMT text + separator for DIF output. 
 *      char *get_variable_value(KDB* dbv, int nb, int t, char** vec)                 Adds one element of a VAR (KDB[nb][t]) to the export vector in DIF format.
 *      int write_variable_and_comment(ExportToFile* expdef, char* code, char* cmt, char* vec)       Saves one VAR in the DIF export file.
 *  
 */
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/comments.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/io/dif.h"
#include "api/io/export.h"


int ExportObjsDIF::write_header(ExportToFile* expdef, KDB* dbv, KDB* dbc, char* outfile)
{
    int dim = dbv->sample->nb_periods;
    int nb  = dbv->size();

    expdef->file_descriptor.open(outfile);

    expdef->file_descriptor << "TABLE\n0,1\n\"\"\nVECTORS\n0," << std::to_string(dim + 2)
                            << "\n\"\"\nTUPLES\n0," << std::to_string(nb + 1) << "\n\"\"\n";
    expdef->file_descriptor << "DATA\n0,0\n\"\"\n-1,0\nBOT\n1,0\n\"CODE\"\n1,0\n\"COMMENT\"\n";

    for(int i = 0; i < dim; i++) 
    {
        Period per = dbv->sample->start_period.shift(i);
        expdef->file_descriptor <<  "1,0\n\"" + per.to_string() + "\"\n";
    }
    return(0);
}

int ExportObjsDIF::close(ExportToFile* expdef, KDB* dbv, KDB* dbc, char* outfile)
{
    expdef->file_descriptor <<  "-1,0\nEOD\n";
    expdef->file_descriptor.close();
    return(0);
}

char* ExportObjsDIF::write_object_name(char* name, char** code)
{
    return(write_pre_post("-1,0\nBOT\n1,0\n\"", "\"\n", name, code));
}

char* ExportObjsDIF::extract_comment(KDB* dbc, char* name, char **cmt)
{
    int pos;

    pos = K_find(dbc, name);
    if(pos >= 0)
        return(write_pre_post("1,0\n\"", "\"\n", KCVAL(dbc, pos), cmt));
    else
        return(write_pre_post("1,0\n\"", "\"\n", "", cmt));
}

char* ExportObjsDIF::get_variable_value(KDB* dbv, int nb, int t, char** vec)
{
    int     lg, olg;
    char    tmp[81], *buf = NULL;

    write_value(tmp, (double)(*KVVAL(dbv, nb, t)));
    write_pre_post("0,", "\nV\n", tmp, &buf);
    lg = (int)strlen(buf) + 1;

    if(*vec == NULL) olg = 0;
    else olg = (int)strlen(*vec);
    *vec = (char*) SW_nrealloc(*vec, olg, olg + lg);

    strcat(*vec, buf);
    SW_nfree(buf);
    return(*vec);
}

int ExportObjsDIF::write_variable_and_comment(ExportToFile* expdef, char* code, char* cmt, char* vec)
{
    expdef->file_descriptor << code << cmt << vec;
    return(0);
}
