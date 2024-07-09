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

      // TableCellType
      m.attr("TABLE_CELL_LEC")    = (int) TABLE_CELL_LEC;
      m.attr("TABLE_CELL_STRING") = (int) TABLE_CELL_STRING;

      // EnumCellAttribute
      m.attr("TABLE_CELL_NORMAL")    = (int) TABLE_CELL_NORMAL;
      m.attr("TABLE_CELL_BOLD")      = (int) TABLE_CELL_BOLD;
      m.attr("TABLE_CELL_ITALIC")    = (int) TABLE_CELL_ITALIC;
      m.attr("TABLE_CELL_UNDERLINE") = (int) TABLE_CELL_UNDERLINE;
      //-----------------
      m.attr("TABLE_CELL_CENTER")  = (int) TABLE_CELL_CENTER;
      m.attr("TABLE_CELL_LEFT")    = (int) TABLE_CELL_LEFT;
      m.attr("TABLE_CELL_RIGHT")   = (int) TABLE_CELL_RIGHT;
      m.attr("TABLE_CELL_DECIMAL") = (int) TABLE_CELL_DECIMAL;

      // TableLineType
      m.attr("TABLE_LINE_FILES") = (int) TABLE_LINE_FILES;
      m.attr("TABLE_LINE_MODE")  = (int) TABLE_LINE_MODE;
      m.attr("TABLE_LINE_CELL")  = (int) TABLE_LINE_CELL;
      m.attr("TABLE_LINE")  = (int) TABLE_LINE;
      m.attr("TABLE_LINE_TITLE") = (int) TABLE_LINE_TITLE;
      m.attr("TABLE_LINE_DATE")  = (int) TABLE_LINE_DATE;

      // TableGraphType
      m.attr("TABLE_GRAPH_LINE")    = (int) TABLE_GRAPH_LINE;
      m.attr("TABLE_GRAPH_SCATTER") = (int) TABLE_GRAPH_SCATTER;
      m.attr("TABLE_GRAPH_BAR")     = (int) TABLE_GRAPH_BAR;

      // TableGraphGrid
      m.attr("TABLE_GRAPH_MAJOR") = (int) TABLE_GRAPH_MAJOR;
      m.attr("TABLE_GRAPH_NONE")  = (int) TABLE_GRAPH_NONE;
      m.attr("TABLE_GRAPH_MINOR") = (int) TABLE_GRAPH_MINOR;

      // TableGraphAlign
      m.attr("TABLE_GRAPH_LEFT")   = (int) TABLE_GRAPH_LEFT;
      m.attr("TABLE_GRAPH_CENTER") = (int) TABLE_GRAPH_CENTER;
      m.attr("TABLE_GRAPH_RIGHT")  = (int) TABLE_GRAPH_RIGHT;

      // TableGraphAxis
      m.attr("TABLE_GRAPH_VALUES")  = (int) TABLE_GRAPH_VALUES;
      m.attr("TABLE_GRAPH_LOG")     = (int) TABLE_GRAPH_LOG;
      m.attr("TABLE_GRAPH_SEMILOG") = (int) TABLE_GRAPH_SEMILOG;
      m.attr("TABLE_GRAPH_PERCENT") = (int) TABLE_GRAPH_PERCENT;

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
