#include "py_constants.h"


void init_constants(nb::module_ &m)
{
      // IODE CONSTANTS
      // --------------

      // NAN value
      m.attr("nan") = IODE_NAN;

      // Object and file types
      m.attr("COMMENTS") = (int) COMMENTS;
      m.attr("EQUATIONS") = (int) EQUATIONS;
      m.attr("IDENTITIES") = (int) IDENTITIES;
      m.attr("LISTS") = (int) LISTS;
      m.attr("SCALARS") = (int) SCALARS;
      m.attr("TABLES") = (int) TABLES;
      m.attr("VARIABLES") = (int) VARIABLES;

      // Methods to estimation equations
      m.attr("EQ_LSQ")            = (int) EQ_LSQ;
      m.attr("EQ_ZELLNER")        = (int) EQ_ZELLNER;
      m.attr("EQ_INSTRUMENTAL")   = (int) EQ_INSTRUMENTAL;
      m.attr("EQ_GLS")            = (int) EQ_GLS;
      m.attr("EQ_MAX_LIKELIHOOD") = (int) EQ_MAX_LIKELIHOOD;

      // test values
      m.attr("EQ_CORR")    = (int) EQ_CORR;
      m.attr("EQ_STDEV")   = (int) EQ_STDEV;
      m.attr("EQ_MEANY")   = (int) EQ_MEANY; 
      m.attr("EQ_SSRES")   = (int) EQ_SSRES; 
      m.attr("EQ_STDERR")  = (int) EQ_STDERR;
      m.attr("EQ_STDERRP") = (int) EQ_STDERRP;
      m.attr("EQ_FSTAT")   = (int) EQ_FSTAT; 
      m.attr("EQ_R2")      = (int) EQ_R2;
      m.attr("EQ_R2ADJ")   = (int) EQ_R2ADJ; 
      m.attr("EQ_DW")      = (int) EQ_DW;
      m.attr("EQ_LOGLIK")  = (int) EQ_LOGLIK;

      // EnumCellType
      m.attr("IT_LEC")    = (int) IT_LEC;
      m.attr("IT_STRING") = (int) IT_STRING;

      // EnumCellAttribute
      m.attr("IT_NORMAL")    = (int) IT_NORMAL;
      m.attr("IT_BOLD")      = (int) IT_BOLD;
      m.attr("IT_ITALIC")    = (int) IT_ITALIC;
      m.attr("IT_UNDERLINE") = (int) IT_UNDERLINE;
      //-----------------
      m.attr("IT_CENTER")  = (int) IT_CENTER;
      m.attr("IT_LEFT")    = (int) IT_LEFT;
      m.attr("IT_RIGHT")   = (int) IT_RIGHT;
      m.attr("IT_DECIMAL") = (int) IT_DECIMAL;

      // EnumLineType
      m.attr("IT_FILES") = (int) IT_FILES;
      m.attr("IT_MODE")  = (int) IT_MODE;
      m.attr("IT_CELL")  = (int) IT_CELL;
      m.attr("IT_LINE")  = (int) IT_LINE;
      m.attr("IT_TITLE") = (int) IT_TITLE;
      m.attr("IT_DATE")  = (int) IT_DATE;

      // EnumGraphType
      m.attr("IG_LINE")    = (int) IG_LINE;
      m.attr("IG_SCATTER") = (int) IG_SCATTER;
      m.attr("IG_BAR")     = (int) IG_BAR;

      // EnumGraphGrid
      m.attr("IG_MAJOR") = (int) IG_MAJOR;
      m.attr("IG_NONE")  = (int) IG_NONE;
      m.attr("IG_MINOR") = (int) IG_MINOR;

      // EnumGraphAlign
      m.attr("IG_LEFT")   = (int) IG_LEFT;
      m.attr("IG_CENTER") = (int) IG_CENTER;
      m.attr("IG_RIGHT")  = (int) IG_RIGHT;

      // EnumGraphAxis
      m.attr("IG_VALUES")  = (int) IG_VALUES;
      m.attr("IG_LOG")     = (int) IG_LOG;
      m.attr("IG_SEMILOG") = (int) IG_SEMILOG;
      m.attr("IG_PERCENT") = (int) IG_PERCENT;

      // Simulation parameters
      m.attr("SORT_CONNEX") = (int) SORT_CONNEX;
      m.attr("SORT_BOTH")   = (int) SORT_BOTH;
      m.attr("SORT_NONE")   = (int) SORT_NONE;

      m.attr("VAR_INIT_TM1")      = (int) VAR_INIT_TM1;
      m.attr("VAR_INIT_TM1_A")    = (int) VAR_INIT_TM1_A;
      m.attr("VAR_INIT_EXTRA")    = (int) VAR_INIT_EXTRA;
      m.attr("VAR_INIT_EXTRA_A")  = (int) VAR_INIT_EXTRA_A;
      m.attr("VAR_INIT_ASIS")     = (int) VAR_INIT_ASIS;
      m.attr("VAR_INIT_TM1_NA")   = (int) VAR_INIT_TM1_NA;
      m.attr("VAR_INIT_EXTRA_NA") = (int) VAR_INIT_EXTRA_NA;

      // Print outputs
      m.attr("W_GDI")   = (int) W_GDI; 
      m.attr("W_A2M")   = (int) W_A2M;
      m.attr("W_MIF")   = (int) W_MIF;
      m.attr("W_HTML")  = (int) W_HTML;
      m.attr("W_RTF")   = (int) W_RTF;
      m.attr("W_CSV")   = (int) W_CSV;
      m.attr("W_DUMMY") = (int) W_DUMMY;

      // HTOL methods
      m.attr("HTOL_LAST") = (int) HTOL_LAST;
      m.attr("HTOL_MEAN") = (int) HTOL_MEAN;
      m.attr("HTOL_SUM")  = (int) HTOL_SUM;

      // LTOH defines 
      m.attr("LTOH_STOCK")  = (int) LTOH_STOCK;
      m.attr("LTOH_FLOW")   = (int) LTOH_FLOW;
      m.attr("LTOH_LIN")    = LTOH_LIN; 
      m.attr("LTOH_CS")     = LTOH_CS;
      m.attr("LTOH_STEP")   = LTOH_STEP;
}
