#pragma once

#include <stdio.h>
#include "api/constants.h"
#include "api/b_errors.h"
#include "api/objs/kdb.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"

#include <iostream> // for std::cout
#include <fstream>  // std::ofstream
#include <array>    // for std::array
#include <memory>   // for std::unique_ptr

/*---------------- ENUMS -------------------------*/

enum IodeExportFormat
{
    EXPORT_CSV,
    EXPORT_DIF,
    EXPORT_WKS,
    EXPORT_TSP,
    EXPORT_RCSV
};

const static int IODE_NB_EXPORT_FORMATS = 5;

/*---------------- GLOBALS ------------------ */

inline char EXP_SEP[11];        // separator used in CSV
inline char EXP_NA[11];         // string to indicate a NaN value in CSV

/*---------------- CLASSES ------------------ */

// struct defining output File descriptor and methods for one type of data format to export
struct ExportToFile 
{
    std::ofstream file_descriptor;                                                                  // Output file descriptor (output of fopen)
    virtual ~ExportToFile() = default;
    virtual int   write_header(const KDBVariables& dbv, const KDBComments& dbc, char* outfile) { return 0; }                      // method that creates the output file and writes the header
    virtual char* write_object_name(char* name, char** code)                                       // method to create the output object name + the separator
    {
        *code = (char*) SCR_stracpy((unsigned char*) name);
        return *code;
    }
    virtual char* extract_comment(const KDBComments& dbc, char* name, char** cmt) { return 0; }                                // method to create the output object comment (if it exists in global_ws_cmt) + the separator for the output file
    virtual char* get_variable_value(const KDBVariables& dbv, int nb, int t, char** vec) { return 0; }                           // method constructing an allocated string of one value + sep
    virtual int   write_variable_and_comment(char* code, char* cmt, char* vec) { return 0; }      // method saving the VAR and CMT in the output file
    virtual int   close(const KDBVariables& dbv, const KDBComments& dbc, char* outfile) { return 0; }                             // method that closes the output file after having written its footer
};

/* k_ecsv.c */
struct ExportObjsCSV : public ExportToFile
{
    int write_header(const KDBVariables& dbv, const KDBComments& dbc, char* outfile) override;
    char* write_object_name(char* name, char** code) override;
    char* extract_comment(const KDBComments& dbc, char* name, char** cmt) override;
    char* get_variable_value(const KDBVariables& dbv, int nb, int t, char** vec) override;
    int write_variable_and_comment(char* code, char* cmt, char* vec) override;
    int close(const KDBVariables& dbv, const KDBComments& dbc, char* outfile) override;
};

/* k_edif.c */
struct ExportObjsDIF : public ExportToFile
{
    int write_header(const KDBVariables& dbv, const KDBComments& dbc, char* outfile) override;
    char* write_object_name(char* name, char** code) override;
    char* extract_comment(const KDBComments& dbc, char* name, char** cmt) override;
    char* get_variable_value(const KDBVariables& dbv, int nb, int t, char** vec) override;
    int write_variable_and_comment(char* code, char* cmt, char* vec) override;
    int close(const KDBVariables& dbv, const KDBComments& dbc, char* outfile) override;
};

/* k_ewks.c */
class ExportObjsWKS : public ExportToFile
{
    int WKS_COL = 1; 
    int WKS_ROW = 1;
    
public:
    int write_header(const KDBVariables& dbv, const KDBComments& dbc, char* outfile) override;
    char* write_object_name(char* name, char** code) override;
    char* extract_comment(const KDBComments& dbc, char* name, char** cmt) override;
    char* get_variable_value(const KDBVariables& dbv, int nb, int t, char** vec) override;
    int write_variable_and_comment(char* code, char* cmt, char* vec) override;
    int close(const KDBVariables& dbv, const KDBComments& dbc, char* outfile) override;
};

/* k_etsp.c */
struct ExportObjsTSP : public ExportToFile
{
    int write_header(const KDBVariables& dbv, const KDBComments& dbc, char* outfile) override;
    char* write_object_name(char* name, char** code) override;
    char* extract_comment(const KDBComments& dbc, char* name, char** cmt) override;
    char* get_variable_value(const KDBVariables& dbv, int nb, int t, char** vec) override;
    int write_variable_and_comment(char* code, char* cmt, char* vec) override;
    int close(const KDBVariables& dbv, const KDBComments& dbc, char* outfile) override;
};

/* k_ecsv.c */
struct ExportObjsRevertCSV : public ExportToFile
{
    int write_header(const KDBVariables& dbv, const KDBComments& dbc, char* outfile) override;
    char* write_object_name(char* name, char** code) override;
    char* get_variable_value(const KDBVariables& dbv, int nb, int t, char** vec) override;
    int write_variable_and_comment(char* code, char* cmt, char* vec) override;
    int close(const KDBVariables& dbv, const KDBComments& dbc, char* outfile) override;
};

inline std::array<std::unique_ptr<ExportToFile>, IODE_NB_EXPORT_FORMATS> export_handlers = 
{
    std::make_unique<ExportObjsCSV>(),
    std::make_unique<ExportObjsDIF>(),
    std::make_unique<ExportObjsWKS>(),
    std::make_unique<ExportObjsTSP>(),
    std::make_unique<ExportObjsRevertCSV>(),
};

/*---------------- FUNCS ------------------ */

/* k_emain.c */
void write_value(char* tmp, double val);
char *write_pre_post(char* pre, char* post, char* src, char** tg);
char *write_separator(char* src, char** tg);
int EXP_Ws(const std::unique_ptr<ExportToFile>& expdef, const KDBVariables& dbv, const KDBComments& dbc, char* rulefile, char* outfile, char* na, char* sep);
int EXP_Rev_Ws(const std::unique_ptr<ExportToFile>& expdef, const KDBVariables& dbv, const KDBComments& dbc, char* rulefile, char* outfile, char* na, char* sep);
int EXP_RuleExport(char* trace, char* rule, char* out, char* vfile, char* cfile, char* from, char* to, char* na, char* sep, int fmt);

/* k_wks.c */
int wks_init(char* file, int c, int r);
int wks_cwidth(int c, int w);
int wks_end(void);
void wks_string(char* label, int c, int r);
void wks_value(double value, int c, int r);
void wks_name(char* str, int c1, int r1, int c2, int r2);
