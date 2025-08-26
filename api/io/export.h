#pragma once

#include <stdio.h>
#include "api/constants.h"
#include "api/b_errors.h"
#include "api/objs/kdb.h"

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

// struct defining output File descriptor and fn pointers for one type of data format to export
struct ExportToFile {
    std::ofstream file_descriptor;                                                                         // Output file descriptor (output of fopen)
    virtual int   write_header(ExportToFile*, KDB*, KDB*, char*) { return 0; }                      // method that creates the output file and writes the header
    virtual char* write_object_name(char* oname, char** code)                                       // method to create the output object name + the separator
    {
        *code = (char*) SCR_stracpy((unsigned char*) oname);
        return *code;
    }
    virtual char* extract_comment(KDB*, char*, char**) { return 0; }                                // method to create the output object comment (if it exists in KC_WS) + the separator for the output file
    virtual char* get_variable_value(KDB*, int, int, char**){ return 0; }                           // method constructing an allocated string of one value + sep
    virtual int   write_variable_and_comment(ExportToFile*, char*, char*, char*) { return 0; }      // method saving the VAR and CMT in the output file
    virtual int   close(ExportToFile*, KDB*, KDB*, char*) { return 0; }                             // method that closes the output file after having written its footer
};

/* k_ecsv.c */
struct ExportObjsCSV : public ExportToFile
{
    int write_header(ExportToFile *,KDB *,KDB *,char *) override;
    char* write_object_name(char *,char **) override;
    char* extract_comment(KDB *,char *,char **) override;
    char* get_variable_value(KDB *,int ,int ,char **) override;
    int write_variable_and_comment(ExportToFile *,char *,char *,char *) override;
    int close(ExportToFile *,KDB *,KDB *, char*) override;
};

/* k_edif.c */
struct ExportObjsDIF : public ExportToFile
{
    int write_header(ExportToFile *,KDB *,KDB *,char *) override;
    char* write_object_name(char *,char **) override;
    char* extract_comment(KDB *,char *,char **) override;
    char* get_variable_value(KDB *,int ,int ,char **) override;
    int write_variable_and_comment(ExportToFile *,char *,char *,char *) override;
    int close(ExportToFile *,KDB *,KDB *, char*) override;
};

/* k_ewks.c */
class ExportObjsWKS : public ExportToFile
{
    int WKS_COL = 1; 
    int WKS_ROW = 1;
    
public:
    int write_header(ExportToFile *,KDB *,KDB *,char *) override;
    char* write_object_name(char *,char **) override;
    char* extract_comment(KDB *,char *,char **) override;
    char* get_variable_value(KDB *,int ,int ,char **) override;
    int write_variable_and_comment(ExportToFile *,char *,char *,char *) override;
    int close(ExportToFile *,KDB *,KDB *, char*) override;
};

/* k_etsp.c */
struct ExportObjsTSP : public ExportToFile
{
    int write_header(ExportToFile *,KDB *,KDB *,char *) override;
    char* write_object_name(char *,char **) override;
    char* extract_comment(KDB *,char *,char **) override;
    char* get_variable_value(KDB *,int ,int ,char **) override;
    int write_variable_and_comment(ExportToFile *,char *,char *,char *) override;
    int close(ExportToFile *,KDB *,KDB *, char*) override;
};

/* k_ecsv.c */
struct ExportObjsRevertCSV : public ExportToFile
{
    int write_header(ExportToFile *,KDB *,KDB *,char *) override;
    char* write_object_name(char *,char **) override;
    char* get_variable_value(KDB *,int ,int ,char **) override;
    int write_variable_and_comment(ExportToFile *,char *,char *,char *) override;
    int close(ExportToFile *,KDB *,KDB *, char*) override;
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
//void EXP_set(char *,char *);
void write_value(char *,double );
char *write_pre_post(char *,char *,char *,char **);
char *write_separator(char *,char **);
int EXP_Ws(ExportToFile *,KDB *,KDB *,char *,char *,char *,char *);
int EXP_Rev_Ws(ExportToFile *,KDB *,KDB *,char *,char *,char *,char *);
int EXP_RuleExport(char *,char *,char *,char *,char *,char *,char *,char *,char *,int );

/* k_wks.c */
int wks_init(char *,int ,int );
int wks_cwidth(int ,int );
int wks_end(void);
void wks_string(char *,int ,int );
void wks_value(double ,int ,int );
void wks_name(char *,int ,int ,int ,int );
