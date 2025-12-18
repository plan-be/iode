// The present source file has been generated from the source
// file test1.c (Google Test is not available with Borland).
// DO NOT MODIFY IT!
// Modify test1.c instead and run convert_tests.bat/sh


#include "pch.h"
#include <math.h>
#include <cstdio>
#include <cstdlib>          // for system()

#include <string>
#include <regex>
#include <chrono>
#include <filesystem>
#include <iostream>


// BEGIN_KEEP
//#ifdef __cplusplus
//extern "C"
//{
//#endif
//    int  A2mGIF_HTML(A2MGRF * go, U_ch * filename) { return(0); }
//#ifdef __cplusplus
//}
//#endif
// END_KEEP

// Temporarily removed fns (to allow linking)

// Pour tester l'estimation
//#ifdef __cplusplus
//extern "C"
//{
//#endif
//
//    extern char    SCR_NAME_ERR[255 + 1];
//    int     A2M_SEPCH;
//
//    //int o_estgr(char** titles, Sample *smpl, MAT* mlhs, MAT* mrhs, int view, int res) {return(0);}
//    //int B_A2mSetRtfTitle(U_ch* title) {return(0);}
//    //int B_A2mSetRtfCopy(U_ch* copyr) {return(0);}
//    //int B_PrintRtfTopic(char* x) { return(0); }
int A2mGIF_HTML(A2MGRF *go, U_ch* filename) {return(0);}
//    //int W_printf(char*fmt, ...) {return(0);}
//    //void K_load_iode_ini() {}
//    //void K_save_iode_ini() {}
//
//#ifdef __cplusplus
//}
//#endif

// END_KEEP


class IodeCAPITest : public TestAbstract, public ::testing::Test
{
public:
    char* separator;
	char* input_test_dir;
	char* output_test_dir;
	char* report_test_dir;

public:
    void SetUp() override 
    {
        separator = (char*) str_separator.c_str();
        input_test_dir  = (char*) str_input_test_dir.c_str();
        output_test_dir = (char*) str_output_test_dir.c_str();
        report_test_dir = (char*) str_report_test_dir.c_str();
        
	    IODE_assign_super_API();    // set *_super fn pointers
	    K_init_ws(0);               // Initialises 7 empty WS
	    B_A2mGetAllParms();
    }

    void TearDown() override 
    {
        K_end_ws(0);                // Deletes the 7 WS
        error_manager.clear();
    }

	int U_cmp_tbls(char** tbl1, char* vec)
	{
	    int     i, rc = 0;
	    char**  tbl2;
	
	    tbl2 = (char**)SCR_vtoms((unsigned char*) vec, (unsigned char*) "|");
	    if(tbl1 == NULL) {
	        if(tbl2 == NULL) return(-1);
	        goto fin;
	    }
	    if(tbl2 == NULL) return(0);
	    if(SCR_tbl_size((unsigned char**) tbl1) != SCR_tbl_size((unsigned char**) tbl2)) goto fin;
	    for(i = 0 ;  tbl1[i] ; i++)
	        if(strcmp(tbl1[i], tbl2[i])) goto fin;
	    rc = -1;
	
	fin:
	    SCR_free_tbl((unsigned char**) tbl2);
	    return(rc);
	
	}

	char* U_test_read_file(char*filename, long *size)
	{
	    return((char*)SCR_LoadRawFile(filename, size));
	
	//    FILE    *fd;
	//    char    *content = 0;
	//
	//    *size = 0;
	//    fd = fopen(filename, "rb");
	//    if(fd == 0) return(NULL);
	//
	//    while(!feof(fd)) {
	//        if(*size % 1024 == 0)
	//            content = SCR_realloc(content, 1, *size, 1 + 1024 + *size);
	//        content[*size] = fgetc(fd);
	//        (*size)++;
	//    }
	//    content[*size] = 0; // Juste pour dire
	//    fclose(fd);
	//    return(content);
	}

	void U_test_print_title(char* title)
	{
	    int i;
	
	    printf("\n\n%s\n", title);
	    for (i = 0; title[i]; i++) printf("-");
	    printf("\n");
	}

	void U_test_suppress_a2m_msgs()
	{
	    A2mMessage_toggle(0);
	}

	void U_test_reset_a2m_msgs()
	{
	    A2mMessage_toggle(1);
	}

	void U_test_suppress_kmsg_msgs()
	{
	    kmsg_toggle(0);
	}

	void U_test_reset_kmsg_msgs()
	{
	    kmsg_toggle(1);
	}

	void U_test_CreateObjects()
	{
        bool        success;
	    char*       lst;
        double*     values;
	    Sample*     smpl;
        CKDBLists*     kdb_lst = global_ws_lst.get();
        CKDBVariables* kdb_var = global_ws_var.get();
	    static int  done = 0;
	
	    // Create lists
	    success = kdb_lst->set_obj("LST1", "A,B");
        EXPECT_TRUE(success);
	    lst = KLVAL(kdb_lst, "LST1");
        EXPECT_NE(lst, nullptr);
        EXPECT_STREQ(lst, "A,B");
	    success = kdb_lst->set_obj("LST2", "A,B,A");
        EXPECT_TRUE(success);
        lst = KLVAL(kdb_lst, "LST2");
        EXPECT_NE(lst, nullptr);
        EXPECT_STREQ(lst, "A,B,A");

	    // Set the sample for the variable WS
	    smpl = new Sample("2000Y1", "2020Y1");
	    KV_sample(kdb_var, smpl);
	    EXPECT_TRUE(kdb_var->sample != nullptr);
	
	    // Creates new vars
	    int nb = smpl->nb_periods;
        double* A = new double[nb];
        double* B = new double[nb];
	    for(int i = 0; i < nb; i++) 
        {
	       A[i] = i;
	       B[i] = i*2;
	    }
	
	    success = kdb_var->set_obj("A", A);
        EXPECT_TRUE(success);
        values = KVVAL(kdb_var, "A", 0);
        EXPECT_NE(values, nullptr);
        EXPECT_DOUBLE_EQ(*KVVAL(kdb_var, "A", 0), A[0]);
        EXPECT_DOUBLE_EQ(*KVVAL(kdb_var, "A", nb-1), A[nb-1]);
	    
        success = kdb_var->set_obj("B", B);
        EXPECT_TRUE(success);
        values = KVVAL(kdb_var, "B", 0);
        EXPECT_NE(values, nullptr);
        EXPECT_DOUBLE_EQ(*KVVAL(kdb_var, "B", 0), B[0]);
        EXPECT_DOUBLE_EQ(*KVVAL(kdb_var, "B", nb-1), B[nb-1]);

        delete[] A;
        delete[] B;

	    // For B_DataPattern()
	    success = kdb_lst->set_obj("AB", "A,B");
        EXPECT_TRUE(success);
	    success = kdb_lst->set_obj("BC", "B,C");
        EXPECT_TRUE(success);
	    success = kdb_var->set_obj("AB", B);
        EXPECT_TRUE(success);
	    success = kdb_var->set_obj("AC", B);
	    EXPECT_TRUE(success);
        success = kdb_var->set_obj("BB", B);
        EXPECT_TRUE(success);
        success = kdb_var->set_obj("BC", B);
        EXPECT_TRUE(success);
	}

	void U_test_lec(char* title, char* lec, int t, double expected_val)
	{
	    CLEC*   clec;
	    double  calc_val;
	    int     rc;
	
	    Period per = global_ws_var->sample->start_period.shift(t);
	
	    clec = L_cc(lec);
        EXPECT_TRUE(clec != NULL);
	    rc = L_link(global_ws_var.get(), global_ws_scl.get(), clec);
	    EXPECT_EQ(rc, 0);
	    calc_val = L_exec(global_ws_var.get(), global_ws_scl.get(), clec, t);
	    SCR_free(clec);
	    EXPECT_DOUBLE_EQ(round(expected_val * 1e6) / 1e6, round(calc_val * 1e6) / 1e6);
	}

	double U_test_calc_lec(char* lec, int t)
	{
	    CLEC*   clec;
	    double  res;
	
	    clec = L_cc(lec);
	    if(clec == NULL) return(IODE_NAN);
	    if(L_link(global_ws_var.get(), global_ws_scl.get(), clec)) return(IODE_NAN);
	    res = L_exec(global_ws_var.get(), global_ws_scl.get(), clec, t);
	    SCR_free(clec);
	    return(res);
	}

	void U_test_load(int type, char* filename)
	{
	    char fullfilename[256];
	    sprintf(fullfilename, "%s%s", input_test_dir, filename);

        int rc = B_WsLoad(fullfilename, type);
        EXPECT_EQ(rc, 0);
	}

	void U_test_load_fun_esv(char* filename)
	{
        char filename_eqs[256];
        char filename_scl[256];
        char filename_var[256];

        sprintf(filename_eqs, "%s.ae", filename);
        sprintf(filename_scl, "%s.as", filename);
        sprintf(filename_var, "%s.av", filename);
	
	    // Loads 3 WS and check ok
	    U_test_load(EQUATIONS, filename_eqs);
	    U_test_load(SCALARS, filename_scl);
	    U_test_load(VARIABLES, filename_var);
	}

	void U_test_W_printf_cmds()
	{
	    W_printf(".sep |\n");
	    W_printf(".esc ~\n");
	
	    W_print_pg_header("IODE");
	    //W_printf(".pghead IODE\n");
	    W_print_pg_footer("- page %d - ");
	    //W_printf(".pgfoot - page %%d - \n");
	
	    W_print_tit(1);
	    W_printf("%s\n", "Mon titre");
	
	    W_print_enum(1);
	    W_printf("This is an enum paragraph\n\n");
	    W_printf("This is a normal paragraph\n\n");
	
	    W_printf(".tb\n");
	    W_printf(".tborder 1\n");
	    W_printf(".theader\n");
	    W_printf("| Syntax | Description\n");
	    W_printf(".tbody\n");
	    W_printf("| ~cint L_errno       | Last error number during LEC compilation\n");
	    W_printf("| ~cchar* L_error()   | Returns a static buffer containing the last LEC compilation error message.\n");
	    W_printf(".te\n");
	
	    W_printf(".gb 16.00 10.00\n");
	    W_printf(".ggrid TMM\n");
	    W_printf(".galign L\n");
	    W_printf(".gbox 0 b 0\n");
	    W_printf(".gtitle Equation ACAF : observed and fitted values\n");
	    W_printf(".gty L \"(ACAF/VAF[-1])  : observed\" 1980Y1  0.011412042  0.016028202  0.011903394  0.012051363  0.010215556  0.010582964  0.0090882893  0.009277778  0.0082268494  0.0051589358  0.0066405193  0.0068489061  0.0075258742  0.0082727193  0.0019340143  -0.0029850522  0.0069569681 \n");
	    W_printf(".gty L \"(ACAF/VAF[-1])  : fitted\" 1980Y1  0.012562124  0.012491075  0.012526504  0.011687035  0.011060337  0.010403968  0.0095320575  0.0090522427  0.008714914  0.0076539375  0.0065561227  0.006355248  0.0064942167  0.0062763884  0.0062678674  -0.0029850522  0.0044903364 \n");
	    W_printf(".ge\n");
	
	}

	void U_test_W_printf_1dest(int typeint, char *typeext)
	{
	    char reffilename[512];
	    char filename[512];
	
	    sprintf(filename, "%stest1.%s", output_test_dir, typeext);
	    sprintf(reffilename, "%stest1.ref.%s", output_test_dir, typeext);
	    W_dest(filename, typeint);
	    U_test_W_printf_cmds();
	    W_close();
	    compare_files(reffilename, filename);
	}

	void U_test_create_a_file(char* filename, int type)
	{
	    //  Create a file
	    U_test_suppress_a2m_msgs();
	    W_dest(filename, type);
	    W_printf("This is a paragraph with accents: éàâêë\n"); // the current source file (test1.c) is ANSI coded
	    W_close();
	}

	bool U_test_file_exists(char* filename, char* msg )
	{
	    int     rc;
	
	    //  Check that the file exists
	    rc = _access(filename, 0);
	    EXPECT_EQ(rc, 0);
	    return rc == 0;
	}

	bool U_test_file_not_exists(char* filename, char* msg)
	{
	    int     rc;
	
	    //  Check that the file exists
	    rc = _access(filename, 0);
	    EXPECT_NE(rc, 0);
	    return rc != 0;
	}

	bool U_test_B_WsLoad(char*filename, int type, int expected_nb_objects)
	{
	    char    fullfilename[256];
	    int     rc;
	
	    sprintf(fullfilename,  "%s%s", input_test_dir, filename);
	    rc = B_WsLoad(fullfilename, type);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(get_global_db(type)->size(), expected_nb_objects);
	    return rc == 0 && get_global_db(type)->size() == expected_nb_objects;
	}

	bool U_test_B_WsSave(char* source_file, char* out_file, int type, int expected_nb_objects)
	{
	    char    fullfilename[256];
	    int     rc;
	
	    _unlink(out_file);
	    sprintf(fullfilename,  "%s%s", input_test_dir, source_file);
	    rc = B_WsLoad(fullfilename, type);
	    EXPECT_EQ(rc, 0);
	    rc = B_WsSave(out_file, type);
	    EXPECT_EQ(rc, 0);
	    B_WsClear("", type);
	    rc = B_WsLoad(out_file, type);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(get_global_db(type)->size(), expected_nb_objects);
	    return rc == 0 && get_global_db(type)->size() == expected_nb_objects;
	}

	bool U_test_B_WsSaveCmp(char* source_file, char* out_file, int type, int expected_nb_objects)
	{
	    char    fullfilename[256];
	    char    out_fullfilename[256];
	    int     rc;
	
	    sprintf(fullfilename, "%s%s", input_test_dir, source_file);
	    sprintf(out_fullfilename, "%s%s", output_test_dir, out_file);

	    _unlink(out_file);
	    rc = B_WsLoad(fullfilename, type);
	    EXPECT_EQ(rc, 0);
	    rc = B_WsSaveCmp(out_fullfilename, type);
	    EXPECT_EQ(rc, 0);
	    B_WsClear("", type);
	    rc = B_WsLoad(out_fullfilename, type);
	    EXPECT_EQ(rc, 0);
	    EXPECT_EQ(get_global_db(type)->size(), expected_nb_objects);
	    return rc == 0 && get_global_db(type)->size() == expected_nb_objects;
	}

	bool U_test_B_WsExport(char* source_file, char* out_file, int type)
	{
	    char    full_source_file[256];
	    char    full_out_file[256];
	    char    ref_out_file[256];
	    int     rc;
	
	    sprintf(full_source_file,  "%s%s", input_test_dir, source_file);
	    sprintf(full_out_file,  "%s%s", output_test_dir, out_file);
	    sprintf(ref_out_file,  "%s%s", input_test_dir, out_file);
	
	    rc = B_WsLoad(full_source_file, type);
	    EXPECT_EQ(rc, 0);
	
	    _unlink(full_out_file);
	    rc = B_WsExport(full_out_file, type);
	    EXPECT_EQ(rc, 0);
	    compare_files(ref_out_file, full_out_file);
	    return true;
	}

	bool U_test_B_WsImport(char* source_file, int type, int expected_nb_objects)
	{
	    char    fullfilename[256];
	    int     rc;
	
	    B_WsClear("", type);
	    sprintf(fullfilename, "%s%s", input_test_dir, source_file);
	    rc = B_WsImport(fullfilename, type);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(get_global_db(type)->size(), expected_nb_objects);
	    return rc == 0 && get_global_db(type)->size() == expected_nb_objects;
	}

	bool U_test_B_WsClear(int type)
	{
	    int     rc;
	
	    rc = B_WsClear("", type);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(get_global_db(type)->size(), 0);
	    return get_global_db(type)->size() == 0;
	}

	bool U_test_B_WsDescr(char* descr, int type)
	{
	    int     rc;
	
	    rc = B_WsDescr(descr, type);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(get_global_db(type)->description, descr);
	    return rc == 0 && get_global_db(type)->description == descr;
	}

	bool U_test_B_WsName(char* c_name, int type)
	{
	    int     rc;
	
	    rc = B_WsName(c_name, type);
        EXPECT_EQ(rc, 0);
	    std::string name = get_global_db(type)->filepath;
	    EXPECT_EQ(name, std::string(c_name));
	    return rc == 0 && name == std::string(c_name);
	}

	bool U_test_B_WsCopyVar()
	{
        bool    success;
	    char    arg[256];
	    int     rc, nb;
	    double *ACAF, ACAF91, ACAF92, ACAG90, ACAG92;
	
	    B_WsClearAll("");
        EXPECT_EQ(global_ws_var->size(), 0);
        EXPECT_TRUE(global_ws_var->sample == nullptr);

	    // 1. Copy full VAR file (Warning: * required)
	    sprintf(arg,  "%sfun.av *", input_test_dir);
	    rc = B_WsCopy(arg, VARIABLES);
        EXPECT_EQ(rc, 0);
        EXPECT_EQ(global_ws_var->size(), 394);
        EXPECT_TRUE(global_ws_var->sample != nullptr);
	    ACAF92 = U_test_calc_lec("ACAF[1992Y1]", 0);
	    ACAG92 = U_test_calc_lec("ACAG[1992Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(ACAF92, 30.159000);
	    EXPECT_DOUBLE_EQ(ACAG92, -40.285998999999997);
	
	    // 2. Copy partial WS (92-93) on an existing one
	    // 2.1 Set Sample + create ACAF
	    B_WsClearAll("");
	    B_WsSample("1990Y1 2000Y1");
	    // Create ACAF = 0 1 2...
	    nb = 11;
	    ACAF = L_cc_link_exec("t", global_ws_var.get(), global_ws_scl.get());
	    success = global_ws_var->set_obj("ACAF", ACAF);
        EXPECT_TRUE(success);
	
	    // 2.2 Copy ACAF and ACAG on 1992 & 1993 (does not replace 1991 for example)
	    sprintf(arg,  "%sfun.av 1992Y1 1993Y1 ACAF ACAG", input_test_dir);
	    rc = B_WsCopy(arg, VARIABLES);
	
	    // 2.3 Tests
	    ACAF91 = U_test_calc_lec("ACAF[1991Y1]", 0);
	    ACAF92 = U_test_calc_lec("ACAF[1992Y1]", 0);
	    ACAG90 = U_test_calc_lec("ACAG[1990Y1]", 0);
	    ACAG92 = U_test_calc_lec("ACAG[1992Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(ACAF91, 1.0);
	    EXPECT_DOUBLE_EQ(ACAF92, 30.159000);
	    EXPECT_DOUBLE_EQ(ACAG90, IODE_NAN);	    
        EXPECT_DOUBLE_EQ(ACAG92, -40.285998999999997);
	
	    // 3. Copy partial WS on an existing one w/o specifying the sample
	    // 3.1 Set Sample
	    B_WsClearAll("");
	    B_WsSample("1990Y1 2000Y1");
	
	    // Copy ACAF and ACAG (does not specify a sample)
	    sprintf(arg,  "%sfun.av ACAF ACAG", input_test_dir);
	    rc = B_WsCopy(arg, VARIABLES);
	    ACAF92 = U_test_calc_lec("ACAF[1992Y1]", 0);
	    ACAG92 = U_test_calc_lec("ACAG[1992Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(ACAF92, 30.159000);
	    EXPECT_DOUBLE_EQ(ACAG92, -40.285998999999997);
	
	    return true;
	}

	bool U_test_B_WsCopy(char* filename, int type, int expected_nb_objects)
	{
	    char    arg[256];
	    int     rc;
	
	    // 1. Copy entire file (Att: * required)
	    B_WsClear("", type);
	    sprintf(arg,  "%s%s *", input_test_dir, filename);
	    rc = B_WsCopy(arg, type);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(get_global_db(type)->size(), expected_nb_objects);
	    return rc == 0 && get_global_db(type)->size() == expected_nb_objects;
	}

	bool U_test_B_WsMergeVar()
	{
        bool     success;
	    int      rc, nb;
	    char     arg[256];
	    double   *ACAF, ACAF92, ACAF00, ACAF16, ACAG92, ACAG00;
        KDB*     kdb_var = global_ws_var.get();
	
	    // 1. Merge into an empty WS
	    B_WsClearAll("");
	    sprintf(arg,  "%sfun.av", input_test_dir);
	    rc = B_WsMerge(arg, VARIABLES);
	    ACAF92 = U_test_calc_lec("ACAF[1992Y1]", 0);
	    ACAG92 = U_test_calc_lec("ACAG[1992Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(ACAF92, 30.159000);
	    EXPECT_DOUBLE_EQ(ACAG92, -40.285998999999997);
	
	    // 2. Merge into an existing WS inb a different Sample
	    B_WsClearAll("");
	    B_WsSample("2000Y1 2020Y1");
	    // Create ACAF = 0 1 2...
	    nb = 21;
	    ACAF = L_cc_link_exec("t", global_ws_var.get(), global_ws_scl.get());
	    success = global_ws_var->set_obj("ACAF", ACAF);
        EXPECT_TRUE(success);
	    // Merge
	    sprintf(arg,  "%sfun.av", input_test_dir);
	    rc = B_WsMerge(arg, VARIABLES);
	    //Check
	    ACAF00 = U_test_calc_lec("ACAF[2000Y1]", 0);
	    ACAF16 = U_test_calc_lec("ACAF[2016Y1]", 0);
	    ACAG00 = U_test_calc_lec("ACAG[2000Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(round(ACAF00 * 1e8) / 1e8, 10.04661079);
	    EXPECT_DOUBLE_EQ(ACAF16, 16.0);
	    EXPECT_DOUBLE_EQ(round(ACAG00 * 1e8) / 1e8, -41.53478657);

	    return true;
	}

	bool U_test_B_WsMerge(char* filename, int type, int expected_nb_objects)
	{
	    char    arg[256];
	    int     rc;
	
	    // 1. Copy entire file (Att: * required)
	    B_WsClear("", type);
	    sprintf(arg,  "%s%s *", input_test_dir, filename);
	    rc = B_WsMerge(arg, type);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(get_global_db(type)->size(), expected_nb_objects);
	    return rc == 0 && get_global_db(type)->size() == expected_nb_objects;
	}

	bool U_test_B_WsExtrapolate(int method, double expected_value)
	{
        bool      success;
	    double    *ACAF, ACAF2002;
	    char      arg[512];
	    int       rc, nb;
	
	    B_WsClearAll("");
	    B_WsSample("1995Y1 2020Y1");
	
	    // Create ACAF = 0 1 IODE_NAN...
	    nb = 11;
	    ACAF = L_cc_link_exec("t", global_ws_var.get(), global_ws_scl.get());
	    ACAF[7] = IODE_NAN;
	    success = global_ws_var->set_obj("ACAF", ACAF);
        EXPECT_TRUE(success);
	
	    // $WsExtrapolate [method] from to [variable list]
	    sprintf(arg, "%d 2000Y1 2010Y1 ACAF", method);
	    rc = B_WsExtrapolate(arg);
	    ACAF2002 = U_test_calc_lec("ACAF[2002Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(ACAF2002, expected_value);
	    return true;
	}

	bool U_test_create_var(char*name, char*lec)
	{
	    double  *A;
	    int     nb;
	
	    if(!global_ws_var.get()) 
            return false;
        
	    nb = global_ws_var->sample->nb_periods;
	    A = L_cc_link_exec(lec, global_ws_var.get(), global_ws_scl.get());
	    global_ws_var->set_obj(name, A);
	    SCR_free(A);
	    return true;
	}

	bool U_test_B_WsAggregate()
	{
	    double   A_2000, B_2000, AC_2000;
	    char        arg[512];
	    int         rc;
	
	    B_WsClearAll("");
	    B_WsSample("1995Y1 2020Y1");
	
	    // Create VARS A1, A2, B1, B2
	    U_test_create_var("A1", "t");
	    U_test_create_var("A2", "t*2");
	    U_test_create_var("B1", "t**2");
	    U_test_create_var("B2", "t**3");
	
	    //  $WsAggrChar [char]
	    rc = B_WsAggrChar("");
	    EXPECT_EQ(rc, 0);
	
	    // $WsAggrSum  pattern
	    strcpy(arg, "(?)[?]");
	    rc = B_WsAggrSum(arg);
	    A_2000 = U_test_calc_lec("A[2000Y1]", 0);
	    B_2000 = U_test_calc_lec("B[2000Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(A_2000, 5 + 5*2); 
        EXPECT_EQ(B_2000, 5 * 5 + 5 * 5 * 5);
	
	    // $WsAggrProd  pattern
	    strcpy(arg, "(?)[?]");
	    rc = B_WsAggrProd(arg);
	    A_2000 = U_test_calc_lec("A[2000Y1]", 0);
	    B_2000 = U_test_calc_lec("B[2000Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(A_2000, (5) * (5*2));
        EXPECT_EQ(B_2000, (5 * 5) * (5 * 5 * 5));
	
	    // $WsAggrMean  pattern
	    strcpy(arg, "(?)[?]");
	    rc = B_WsAggrMean(arg);
	    A_2000 = U_test_calc_lec("A[2000Y1]", 0);
	    B_2000 = U_test_calc_lec("B[2000Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(A_2000, 7.5); 
        EXPECT_DOUBLE_EQ(B_2000, 75.0);
	
	    // $WsAggrSum  pattern filename
	    sprintf(arg,  "(AC)[??] %sfun.av", input_test_dir);
	    rc = B_WsAggrSum(arg);
	    AC_2000 = U_test_calc_lec("AC[2000Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(round(AC_2000 * 1e8) / 1e8, -31.48817578);
	
	    return true;
	}

	bool U_test_B_StatUnitRoot(int drift, int trend, int order, char* varname, double expected_df)
	{
	    int     rc;
	    double  df;
	    char    arg[256], scalar[30], lowercase_var_name[27];
	
	    // Dickey-Fuller test (E_UnitRoot)
	    sprintf(arg, "%d %d %d %s", drift, trend, order, varname);
	    rc = B_StatUnitRoot(arg);
        // set varname to lower case
        memset(lowercase_var_name, 0, sizeof(lowercase_var_name));
        for (int i = 0; i < strlen(varname); i++)
            lowercase_var_name[i] = tolower((unsigned char) varname[i]);
	    sprintf(scalar, "df_%s", lowercase_var_name);
	    df = U_test_calc_lec(scalar, 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(df, expected_df);
	    return true;
	}

	bool U_test_B_Csv()
	{
	    int     rc;
	    char    arg[512];
	
	    // int B_CsvNbDec(char *nbdec, int unused)                       $CsvNbDec nn
	    // int B_CsvSep(char *sep, int unused)                           $CsvSep char
	    // int B_CsvNaN(char *nan, int unused)                           $CsvNaN text
	    // int B_CsvAxes(char *var, int unused)                          $CsvAxes AxisName
	    // int B_CsvDec(char *dec, int unused)                           $CsvDec char
	    // int B_CsvSave(char* arg, int type)                $CsvSave<type> file name1 name2 ...
	    rc = B_CsvNbDec("7");
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(CKDBVariables::CSV_NBDEC, 7);
	
	    rc = B_CsvSep(";");
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(CKDBVariables::CSV_SEP[0], ';');
	
	    rc = B_CsvNaN("--");
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(std::string(CKDBVariables::CSV_NAN), "--");
	
	    rc = B_CsvAxes("Name");
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(std::string(CKDBVariables::CSV_AXES), "Name");
	
	    rc = B_CsvDec(".");
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(std::string(CKDBVariables::CSV_DEC), ".");
	
        sprintf(arg, "%sfun", input_test_dir);
	    B_WsLoad(arg, VARIABLES);
        EXPECT_EQ(global_ws_var->size(), 394);
        EXPECT_TRUE(global_ws_var->sample != nullptr);
	    sprintf(arg, "%sfuncsv.csv A* *G", output_test_dir);
	    rc = B_CsvSave(arg, VARIABLES);
        EXPECT_EQ(rc, 0);
	    compare_files(output_test_dir, "funcsv.csv", output_test_dir, "funcsv.ref.csv");

	    return true;
	}
};


TEST_F(IodeCAPITest, Tests_BUF)
{
    U_test_print_title("Tests BUF");
    EXPECT_EQ(BUF_DATA, nullptr);
    EXPECT_NE(BUF_strcpy("ABCD"), nullptr);
    EXPECT_NE(BUF_alloc(100), nullptr);
    EXPECT_NE(BUF_strcpy("ABCD"), nullptr);
    BUF_free();
    EXPECT_EQ(BUF_DATA, nullptr);
}


TEST_F(IodeCAPITest, Tests_OBJECTS)
{
    bool        found;
    char*       lst;
    static int  done = 0;

    U_test_print_title("Tests OBJECTS");
    U_test_CreateObjects();

    // Create lists
    found = global_ws_lst->contains("LST1");
    EXPECT_TRUE(found);
    lst = KLVAL(global_ws_lst.get(), "LST1");
    EXPECT_EQ(strcmp(lst, "A,B"), 0);

    found = global_ws_var->contains("A");
    EXPECT_TRUE(found);

    bool success = global_ws_var->rename("A", "AAA");
    EXPECT_TRUE(success);
    found = global_ws_var->contains("AAA");
    EXPECT_TRUE(found);

    // Test KV_sample()
    std::string asmpl1 = global_ws_var->sample->to_string();
    KV_sample(global_ws_var.get(), NULL);
    std::string asmpl2 = global_ws_var->sample->to_string();
    EXPECT_EQ(asmpl1, asmpl2);
}


TEST_F(IodeCAPITest, Tests_Table_ADD_GET)
{
    Table*       tbl;
    Table*       extracted_tbl;
    TableLine*   line;
    TableLine*   line_original;
    TableLine*   line_restored;
    std::vector<TableCell> cells;
    std::vector<TableCell> cells_original;
    std::vector<TableCell> cells_restored;

    int nb_columns = 2;
    std::string title = "A title";
    std::vector<std::string> v_lecs = {"GOSG", "YDTG", "DTH", "DTF", "IT", 
                                       "YSSG+COTRES", "RIDG", "OCUG"};
    bool mode = true;
    bool files = true;
    bool date = true;

    U_test_print_title("Tests Table: Table(...) constructor vs KTVAL()");

    // --- create an instance of Table;
    tbl = new Table(nb_columns, title, v_lecs, mode, files, date);

    EXPECT_TRUE(tbl != nullptr);
    EXPECT_EQ(tbl->nb_columns, nb_columns);
    // divider line
    EXPECT_EQ(tbl->divider_line.get_type(), TABLE_LINE_CELL);
    EXPECT_EQ(tbl->divider_line.get_graph_type(), 0);
    EXPECT_EQ(tbl->divider_line.right_axis, false);
    cells = tbl->divider_line.cells;
    EXPECT_EQ(cells.size(), nb_columns);
    EXPECT_EQ(cells[0].get_type(), TABLE_CELL_LEC);
    EXPECT_EQ(cells[0].get_content(), "1");
    EXPECT_EQ(cells[1].get_type(), TABLE_CELL_LEC);
    EXPECT_EQ(cells[1].get_content(), "");
    cells.clear();
    // title + sep + line #S + sep + 8 lines with cells + sep + mode + files + date
    EXPECT_EQ(tbl->lines.size(), 16);
    // title line
    line = &tbl->lines[0];
    EXPECT_EQ(line->get_type(), TABLE_LINE_TITLE);
    cells = line->cells;
    EXPECT_EQ(cells.size(), 1);
    EXPECT_EQ(cells[0].get_content(), title);
    cells.clear();
    // sep line
    EXPECT_EQ(tbl->lines[1].get_type(), TABLE_LINE_SEP);
    // line #S
    line = &tbl->lines[2];
    EXPECT_EQ(line->get_type(), TABLE_LINE_CELL);
    cells = line->cells;
    EXPECT_EQ(cells.size(), nb_columns);
    EXPECT_EQ(cells[0].get_type(), TABLE_CELL_LEC);
    EXPECT_EQ(cells[0].get_content(), "");
    EXPECT_EQ(cells[1].get_type(), TABLE_CELL_STRING);
    EXPECT_EQ(cells[1].get_content(), std::string("#S"));
    cells.clear();
    // sep line
    EXPECT_EQ(tbl->lines[3].get_type(), TABLE_LINE_SEP);
    // variable lines
    int i = 4;
    for(std::string& lec: v_lecs)
    {
        line = &tbl->lines[i];
        EXPECT_EQ(line->get_type(), TABLE_LINE_CELL);
        EXPECT_EQ(line->cells.size(), nb_columns);
        cells = line->cells;
        // left column: line name
        EXPECT_EQ(cells[0].get_type(), TABLE_CELL_STRING);
        EXPECT_EQ(cells[0].get_content(), lec);
        // right column: lec expression
        EXPECT_EQ(cells[1].get_type(), TABLE_CELL_LEC);
        EXPECT_EQ(cells[1].get_content(), lec);
        // next line
        cells.clear();
        i++;
    }
    // sep line
    EXPECT_EQ(tbl->lines[i++].get_type(), TABLE_LINE_SEP);
    // mode, files, date lines
    EXPECT_EQ(tbl->lines[i++].get_type(), TABLE_LINE_MODE);
    EXPECT_EQ(tbl->lines[i++].get_type(), TABLE_LINE_FILES);
    EXPECT_EQ(tbl->lines[i++].get_type(), TABLE_LINE_DATE);

    // --- add the table to the Tables KDB
    char* name = "TABLE";
    global_ws_tbl->set_obj(name, tbl);

    // --- extract the table from the Table KDB
    extracted_tbl = KTVAL(global_ws_tbl.get(), name);

    // --- check that both table are exactly the same
    // ----- check all attributes that are not of type TableLine
    EXPECT_EQ(tbl->get_language(), extracted_tbl->get_language());
    EXPECT_EQ(tbl->repeat_columns, extracted_tbl->repeat_columns);
    EXPECT_EQ(tbl->nb_columns, extracted_tbl->nb_columns);
    EXPECT_EQ(tbl->lines.size(), extracted_tbl->lines.size());
    EXPECT_EQ(tbl->z_min, extracted_tbl->z_min);
    EXPECT_EQ(tbl->z_max, extracted_tbl->z_max);
    EXPECT_EQ(tbl->y_min, extracted_tbl->y_min);
    EXPECT_EQ(tbl->y_max, extracted_tbl->y_max);
    EXPECT_EQ(tbl->attribute, extracted_tbl->attribute);
    EXPECT_EQ(tbl->chart_box, extracted_tbl->chart_box);
    EXPECT_EQ(tbl->chart_shadow, extracted_tbl->chart_shadow);
    EXPECT_EQ(tbl->get_gridx(), extracted_tbl->get_gridx());
    EXPECT_EQ(tbl->get_gridy(), extracted_tbl->get_gridy());
    EXPECT_EQ(tbl->get_graph_axis(), extracted_tbl->get_graph_axis());
    EXPECT_EQ(tbl->get_text_alignment(), extracted_tbl->get_text_alignment());

    // ----- check div line
    EXPECT_EQ(tbl->divider_line.get_type(), extracted_tbl->divider_line.get_type());
    EXPECT_EQ(tbl->divider_line.get_graph_type(), extracted_tbl->divider_line.get_graph_type());
    EXPECT_EQ(tbl->divider_line.right_axis, extracted_tbl->divider_line.right_axis);
    cells_original = tbl->divider_line.cells;
    cells_restored = extracted_tbl->divider_line.cells;

    for(int j = 0; j < tbl->nb_columns; j++)
    {
        EXPECT_EQ(cells_original[j].get_type(), cells_restored[j].get_type());
        EXPECT_EQ(cells_original[j].get_attribute(), cells_restored[j].get_attribute());
    }

    cells_original.clear();
    cells_restored.clear();

    // ----- check all lines
    for(int i = 0; i < tbl->lines.size(); i++)
    {
        line_original = &tbl->lines[i];
        line_restored = &extracted_tbl->lines[i];

        EXPECT_EQ(line_original->get_type(), line_restored->get_type());
        EXPECT_EQ(line_original->get_graph_type(), line_restored->get_graph_type());
        EXPECT_EQ(line_original->right_axis, line_restored->right_axis);

        cells_original = line_original->cells;
        cells_restored = line_restored->cells;
        switch (line_original->get_type())
        {
          case TABLE_LINE_TITLE:
            EXPECT_EQ(cells_original[0].get_content(), cells_restored[0].get_content());
            break;
          case TABLE_LINE_CELL:
            for(int j = 0; j < tbl->nb_columns; j++)
            {
                EXPECT_EQ(cells_original[j].get_type(), cells_restored[j].get_type());
                EXPECT_EQ(cells_original[j].get_attribute(), cells_restored[j].get_attribute());
                EXPECT_EQ(cells_original[j].get_content(), cells_restored[j].get_content());
            }
            break;
          default:
            EXPECT_TRUE(cells_original.size() == 0); 
            EXPECT_TRUE(cells_restored.size() == 0);
            break;
        }
        cells_original.clear();
        cells_restored.clear();
    }

    // --- free memory
    delete tbl;
    delete extracted_tbl;
}


TEST_F(IodeCAPITest, Tests_LEC)
{
    double *A, *B;

    U_test_print_title("Tests LEC");

    // Create objects
    U_test_CreateObjects();

    A = (double*) KVVAL(global_ws_var.get(), "A");
    B = (double*) KVVAL(global_ws_var.get(), "B");

    // Tests LEC
    U_test_lec("LEC", "A+B",  2, A[2]+B[2]);
    U_test_lec("LEC", "ln A", 2, log(A[2]));
    U_test_lec("LEC", "A[2002Y1]",     2, A[2]);
    U_test_lec("LEC", "A[2002Y1][-1]", 2, A[2]);
    U_test_lec("LEC", "A[-1]",         2, A[1]);
    U_test_lec("LEC", "A[-1][2002Y1]", 2, A[1]);
    U_test_lec("LEC", "sum(2000Y1, 2010Y1, A)", 2, 55.0);
    U_test_lec("LEC", "sum(2000Y1, A)", 2, 3.0);

    char* lst = KLVAL(global_ws_lst.get(), "LST1");
    EXPECT_STREQ(lst, "A,B");
    lst = KLVAL(global_ws_lst.get(), "LST2");
    EXPECT_STREQ(lst, "A,B,A");

    // Using macros in LEC
    CLEC* clec = L_cc("1 + vmax($LST1)");
    EXPECT_TRUE(clec != NULL);
    EXPECT_EQ(clec->nb_names, 2);
    EXPECT_STREQ(clec->lnames[0].name, "A");
    EXPECT_STREQ(clec->lnames[1].name, "B");
    SCR_free(clec);

    clec = L_cc("1 + vmax($LST2)");
    EXPECT_TRUE(clec != NULL);
    EXPECT_EQ(clec->nb_names, 2);
    EXPECT_STREQ(clec->lnames[0].name, "A");
    EXPECT_STREQ(clec->lnames[1].name, "B");
    SCR_free(clec);

    U_test_lec("LEC-MACRO", "1 + vmax($LST1)", 2, 1+B[2]);
    U_test_lec("LEC-MACRO", "1 + vmax($LST2)", 2, 1+B[2]);
}


TEST_F(IodeCAPITest, Tests_EQS)
{
//    Equation*     eq;
//    char    lec[521];
//
//    K_upd_eqs("A", "ln A := B + t", NULL, 'L', NULL, NULL, NULL, NULL, NULL);
//    eq = KEPTR("A");
//    strcpy(lec, eq->lec);
//    S4ASSERT(strcmp(eq->lec, "ln A := B + t") == 0, "EQ %s = %s", "A", lec);
//
}


TEST_F(IodeCAPITest, Tests_ARGS)
{
    char **args;
    char *list[] = {"A1", "A2", 0};
    char filename[256];

    U_test_print_title("Tests ARGS");

    // Create objects
    U_test_CreateObjects();

    // A_init
    args = B_ainit_chk("A B;C,D", NULL, 0); // => "A" "B;C" "D"
    EXPECT_TRUE(U_cmp_tbls(args, "A|B;C|D"));
    SCR_free_tbl((unsigned char**) args);

    // A_init
    args = B_ainit_chk("$LST1", NULL, 0);
    EXPECT_TRUE(U_cmp_tbls(args, "A|B"));
    SCR_free_tbl((unsigned char**) args);

    // Test parameters in a file. test.args must exist in the current dir 
    // and contain the line A1 A2
    sprintf(filename, "@%stest.args", input_test_dir);
    args = B_ainit_chk(filename, NULL, 0);
    EXPECT_TRUE(U_cmp_tbls(args, "A1|A2"));
    SCR_free_tbl((unsigned char**) args);
}


TEST_F(IodeCAPITest, Tests_ERRMSGS)
{
    U_test_print_title("Tests Err Msgs");

    error_manager.append_error("bla bla: incorrect period");
    kerror(0, "Coucou de kerror %s", "Hello");
    kmsg("Coucou de kmsg %s -- %g", "Hello", 3.2);
}


TEST_F(IodeCAPITest, Tests_K_OBJFILE)
{
    char in_filename[256];
    char out_filename[256];

    U_test_print_title("Tests K_OBJFILE");

    sprintf(in_filename,  "%sfun.var", input_test_dir);
    sprintf(out_filename, "%sfun_copy.var", output_test_dir);

    CKDBVariables* kdb_var = new CKDBVariables(false);
    bool success = kdb_var->load(std::string(in_filename));
    EXPECT_TRUE(success);
    EXPECT_EQ(kdb_var->size(), 394);
    kdb_var->save_binary(out_filename);
    delete kdb_var;
    kdb_var = nullptr;

    // load (binary files)
    // load all objects
    kdb_var = new CKDBVariables(DB_GLOBAL);
    kdb_var->load_binary(VARIABLES, in_filename);
    EXPECT_NE(kdb_var, nullptr);
    EXPECT_NE(kdb_var->sample, nullptr);
    EXPECT_EQ(kdb_var->size(), 394);
    EXPECT_DOUBLE_EQ(round(*KVVAL(kdb_var, "ACAF", 32) * 1000) / 1000, 30.159);   // ACAF 1992Y1
    EXPECT_DOUBLE_EQ(round(*KVVAL(kdb_var, "ACAG", 32) * 1000) / 1000, -40.286);  // ACAG 1992Y1
    delete kdb_var;
    kdb_var = nullptr;

    // load only 2 objects
    char** objs = B_ainit_chk("ACAF ACAG", NULL, 0);
    std::vector<std::string> v_objs;
    for(int i = 0; objs[i] != NULL; i++)
        v_objs.push_back(std::string(objs[i]));
    SCR_free_tbl((unsigned char**) objs);

    kdb_var = new CKDBVariables(DB_GLOBAL);
    kdb_var->load_binary(VARIABLES, in_filename, v_objs);
    EXPECT_NE(kdb_var, nullptr);
    EXPECT_NE(kdb_var->sample, nullptr);
    EXPECT_EQ(kdb_var->size(), 2);
    EXPECT_DOUBLE_EQ(round(*KVVAL(kdb_var, "ACAF", 32) * 1000) / 1000, 30.159);   // ACAF 1992Y1
    EXPECT_DOUBLE_EQ(round(*KVVAL(kdb_var, "ACAG", 32) * 1000) / 1000, -40.286);  // ACAG 1992Y1
    delete kdb_var;
    kdb_var = nullptr;
}


TEST_F(IodeCAPITest, Tests_Simulation)
{
    CKDBVariables* kdbv;
    CKDBEquations* kdbe;
    CKDBScalars*   kdbs;
    Sample* smpl;
    char*   filename = "fun";
    U_ch**  endo_exo;
    int     rc;
    LIS     lst, expected_lst;
    void    (*kmsg_super_ptr)(const char*);
    double  XNATY_2000Y1;

    U_test_print_title("Tests Simulation");

    // Loads 3 WS and check ok
    U_test_load_fun_esv(filename);

    // Check
    kdbv = global_ws_var.get();
    EXPECT_NE(kdbv, nullptr);
    kdbs = global_ws_scl.get();
    EXPECT_NE(kdbs, nullptr);
    kdbe = global_ws_eqs.get();
    EXPECT_NE(kdbe, nullptr);

    // Check list _DIVER does not exist before simulation
    EXPECT_FALSE(global_ws_lst->contains("_DIVER"));

    // Simulation instance
    CSimulation simu;

    // Simulation parameters
    smpl = new Sample("2000Y1", "2002Y1");
    CSimulation::KSIM_START = VAR_INIT_TM1;
    CSimulation::KSIM_EPS = 0.0001;
    CSimulation::KSIM_MAXIT = 100;
    CSimulation::KSIM_RELAX = 0.7;
    CSimulation::KSIM_SORT = SORT_BOTH;
    CSimulation::KSIM_PASSES = 5;
    CSimulation::KSIM_DEBUG = 1;

    kmsg_super_ptr = kmsg_super;
    kmsg_super = kmsg_null; // Suppress messages at each iteration during simulation

    // Test simulation : divergence
    CSimulation::KSIM_MAXIT = 2;
    rc = simu.K_simul(kdbe, kdbv, kdbs, smpl, NULL, NULL);
    EXPECT_NE(rc, 0);

    // Check _PRE list after simulation (prolog)
    EXPECT_TRUE(global_ws_lst->contains("_PRE"));
    lst = KLVAL(global_ws_lst.get(), "_PRE");
    expected_lst = "BRUGP;DTH1C;EX;ITCEE;ITCR;ITGR;ITI5R;ITIFR;ITIGR;ITMQR;NATY;POIL;PW3;PWMAB;PWMS;PWXAB;PWXS;PXE;QAH;QWXAB;QWXS;QWXSS;SBGX;TFPFHP_;TWG;TWGP;ZZF_;DTH1;PME;PMS;PMT";
    //printf("     '%s'(%d)\n", expected_lst, strlen(expected_lst));
    EXPECT_EQ(std::string(lst), std::string(expected_lst));

    // Check _DIVER 
    EXPECT_TRUE(global_ws_lst->contains("_DIVER"));
    lst = KLVAL(global_ws_lst.get(), "_DIVER");
    //printf("'%s'\n", lst);
    expected_lst = "SSH3O,WBG,SSF3,YDH,DTH,YDTG,YSFIC,WMIN,WLCP,WBGP,YSEFT2,YSEFT1,YSEFP,SBG,PWBG,W,ZJ,QMT,QI5,QC_,SSFG,YDH_,SG,ACAG,FLG";
    EXPECT_EQ(std::string(lst), std::string(expected_lst));

    // Test with with convergence (increase MAXIT)
    CSimulation::KSIM_MAXIT = 100;
    rc = simu.K_simul(kdbe, kdbv, kdbs, smpl, NULL, NULL);
    EXPECT_EQ(rc, 0);

    // Test Endo-exo

    // Version with exchange in at least 2 equations
    // Set values of endo UY
    KV_set_at_aper("UY", "2000Y1", 650.0);
    KV_set_at_aper("UY", "2001Y1", 670.0);
    KV_set_at_aper("UY", "2002Y1", 680.0);

    // Simulate with exchange UY - XNATY
    endo_exo = SCR_vtoms((unsigned char*)"UY-XNATY", (unsigned char*)",; ");
    rc = simu.K_simul(kdbe, kdbv, kdbs, smpl, (char**)endo_exo, NULL);

    // Check result
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(KV_get_at_aper("UY", "2000Y1"), 650.0);
    XNATY_2000Y1 = KV_get_at_aper("XNATY", "2000Y1");
    EXPECT_DOUBLE_EQ(round(KV_get_at_aper("XNATY", "2000Y1") * 1e6) / 1e6, 0.800703);

    // Cleanup
    SCR_free_tbl(endo_exo);
    delete smpl;
    kmsg_super = kmsg_super_ptr; // reset default value
}


TEST_F(IodeCAPITest, Tests_PrintTablesAndVars)
{
    char    fullfilename[256];
    char    **varlist;
    Sample  *smpl;
    Table   *tbl;
    int     rc;
    CKDBTables*    kdbt;
    CKDBVariables* kdbv; 

    U_test_suppress_a2m_msgs();

    U_test_print_title("Tests Print Table as Tables and Graphs");

    // Load the VAR workspace
    U_test_load(VARIABLES, "fun.av");
    kdbv = global_ws_var.get();
    K_RWS[VARIABLES][0] = new CKDBVariables(*kdbv);
    EXPECT_NE(kdbv, nullptr);

    // Load the Table workspace
    U_test_load(TABLES, "fun.at");
    kdbt = global_ws_tbl.get();
    K_RWS[TABLES][0] = new CKDBTables(*kdbt);
    EXPECT_NE(kdbt, nullptr);

    // Load a second VAR workspace in K_RWS[VARIABLES][2]
    sprintf(fullfilename,  "%s%s", input_test_dir, "fun.av");
    rc = K_load_RWS(2, fullfilename);
    EXPECT_EQ(rc, 0);

    // Select a table
    tbl = KTVAL(global_ws_tbl.get(), "C8_1");
    EXPECT_NE(tbl, nullptr);

    // Select Print destination
    //W_dest("test1_tbl.htm", W_HTML);
    W_dest("test1_tbl.a2m", W_A2M);
    //W_dest("", W_GDI);

    // Print tbl as table
    rc = T_print_tbl(tbl, "2000:5[1;2]");
    EXPECT_EQ(rc, 0);

    // Print tbl as a graph
    rc = T_graph_tbl_1(tbl, "2000/1999:15[1;2]", 1);
    EXPECT_EQ(rc, 0);

    // deletes tbl
    delete tbl;

    // Print vars as graphs
    varlist = (char**) SCR_vtoms((U_ch*)"ACAF,ACAG,ACAF+ACAG", (U_ch*)",;");
    smpl = new Sample("1990Y1", "2010Y1");
    rc = V_graph(0, 0, 0, 1, 1, 0, IODE_NAN, IODE_NAN, smpl, varlist);
    EXPECT_EQ(rc, 0);
    SCR_free_tbl((U_ch**)varlist);
    delete smpl;

    // Close the output file
    W_close();

    // Cleanup the 2d VAR ws
    K_load_RWS(2, NULL);

    // Reset A2M messages
    U_test_reset_a2m_msgs();
}


TEST_F(IodeCAPITest, Tests_Estimation)
{
    int         rc;
    void        (*kmsg_super_ptr)(const char*);
    Sample      *smpl;
    double      r2, *df;

    U_test_suppress_a2m_msgs();
    U_test_print_title("Tests Estimation");

    kmsg_super_ptr = kmsg_super;
    kmsg_super = kmsg_null; // Suppress messages at each iteration during simulation

    // Select output destination
    W_dest("test1_estim.a2m", W_A2M);
    //W_dest("test1_estim.rtf", W_RTF);
    //W_dest("test1_estim.htm", W_HTML);
    //W_dest("test.gdi",        W_GDI);

    U_test_load_fun_esv("fun");
    Estimation est("ACAF");
    rc = est.estimate("1980Y1", "1996Y1");
    EXPECT_EQ(rc, 0);

    EXPECT_DOUBLE_EQ(round(U_test_calc_lec("_YRES0[1980Y1]", 0) * 1e8) / 1e8, -0.00115008);
    EXPECT_DOUBLE_EQ(round(K_e_r2(global_ws_eqs.get(), "ACAF") * 1e6) / 1e6, 0.821815);

    //TODO:add some tests with other estimation methods / on blocks / with instruments

    //W_flush();
    W_close();

    // estimate_step_wise
    smpl = new Sample("1980Y1", "1995Y1");
    r2 = estimate_step_wise(smpl, "ACAF", "", "r2");
    EXPECT_DOUBLE_EQ(round(r2 * 1e6) / 1e6, 0.848519);
    delete smpl;

    std::vector<std::string> coef_names = {"acaf1", "acaf2", "acaf3", "acaf4"};

    // B_EqsStepWise
    for(const std::string& name : coef_names)
    {
        KSVAL(global_ws_scl.get(), name)->value = 0.9;
        KSVAL(global_ws_scl.get(), name)->relax = 1.0;
    }
    rc = B_EqsStepWise("1980Y1 1995Y1 ACAF 1 r2");
    EXPECT_EQ(rc, 0);

    for(const std::string& name : coef_names)
    {
        KSVAL(global_ws_scl.get(), name)->value = 0.9;
        KSVAL(global_ws_scl.get(), name)->relax = 1.0;
    }
    rc = B_EqsStepWise("1980Y1 1995Y1 ACAF 1 fstat");
    EXPECT_EQ(rc, 0);

    for(const std::string& name : coef_names)
    {
        KSVAL(global_ws_scl.get(), name)->value = 0.9;
        KSVAL(global_ws_scl.get(), name)->relax = 1.0;
    }
    rc = B_EqsStepWise("1980Y1 1995Y1 ACAF \"acaf2 > 0\" r2");
    EXPECT_EQ(rc, 0);

    for(const std::string& name : coef_names)
    {
        KSVAL(global_ws_scl.get(), name)->value = 0.9;
        KSVAL(global_ws_scl.get(), name)->relax = 1.0;
    }
    rc = B_EqsStepWise("1980Y1 1995Y1 ACAF \"acaf2 > 0\" fstat");
    EXPECT_EQ(rc, 0);

    // Dickey-Fuller test (E_UnitRoot)
    df = E_UnitRoot("ACAF+ACAG", 0, 0, 0);
    EXPECT_DOUBLE_EQ(round(df[2] * 1e6) / 1e6, -1.602170);
    df = E_UnitRoot("ACAF+ACAG", 1, 0, 0);
    EXPECT_DOUBLE_EQ(round(df[2] * 1e6) / 1e6, -2.490054);
    df = E_UnitRoot("ACAF+ACAG", 1, 1, 0);
    EXPECT_DOUBLE_EQ(round(df[2] * 1e6) / 1e6, -2.638717);
    df = E_UnitRoot("ACAF+ACAG", 0, 0, 1);
    EXPECT_DOUBLE_EQ(round(df[2] * 1e6) / 1e6, -1.300049);

    // Reset initial kmsg fn
    kmsg_super = kmsg_super_ptr; // Reset initial output to
    U_test_reset_a2m_msgs();

}


TEST_F(IodeCAPITest, Tests_ALIGN)
{
    U_test_print_title("Tests ALIGN");
    
    Table* p_tbl = new Table(2);
    int offset = (int) ((char*)(p_tbl + 1) - (char*)p_tbl);
    printf("sizeof(Table)    = %d -- Offset = %d\n", (int)sizeof(Table), offset);
    delete p_tbl;
}


TEST_F(IodeCAPITest, Tests_W_printf)
{
    U_test_print_title("Tests W_printf");

    U_test_suppress_a2m_msgs();

    //W_gdiask = 0;  // 1 means that a popup window will be opened to select the printer => not practical for the automated tests!
    //W_dest("test1", W_GDI);
    //U_test_W_printf();

    U_test_W_printf_1dest(W_A2M, "a2m");
    U_test_W_printf_1dest(W_RTF, "rtf");
    U_test_W_printf_1dest(W_HTML, "htm");
    U_test_W_printf_1dest(W_CSV, "csv");
    U_test_W_printf_1dest(W_MIF, "mif");


    W_dest("", W_DUMMY);
    U_test_W_printf_cmds();

    //W_dest("test1", W_DISP);
    //U_test_W_printf();

    W_close(); // Closes the last "print" if any

    // Reset initial kmsg fn
    U_test_reset_a2m_msgs();
}


TEST_F(IodeCAPITest, Tests_SWAP)
{
    SWHDL   item, item2;

    U_test_print_title("Tests SWAP");

    // test 1 : deux frees successifs
    item = SW_alloc(20);
    SW_free(item);
    SW_free(item);

    // test 2 :  reusing freed space
    item = SW_alloc(15);
    SW_free(item);
    item2 = SW_alloc(10);
    SW_free(item);
    SW_free(item2);
}


TEST_F(IodeCAPITest, Tests_B_DATA)
{
    bool        success;
    bool        found;
    char        *lst, buf[512];
    int         rc, i;
    double      *A1, val;
    Sample      *smpl;
    char        *filename = "fun";
    SWHDL       handle;

    U_test_print_title("Tests B_DATA");

    CKDBComments* kdb_cmt = new CKDBComments(DB_GLOBAL);
    kdb_cmt->set_obj("AAA", "This is a test comment");
    handle = kdb_cmt->get_handle("AAA");
    EXPECT_TRUE(handle > 0);
    char* value = (char*) KCVAL(kdb_cmt, handle);
    EXPECT_EQ(std::string(value), "This is a test comment");
    delete kdb_cmt;
    kdb_cmt = nullptr;

    kdb_cmt = global_ws_cmt.get();
    KDB* kdb_eqs = global_ws_eqs.get();
    KDB* kdb_idt = global_ws_idt.get();
    KDB* kdb_lst = global_ws_lst.get();
    KDB* kdb_scl = global_ws_scl.get();
    KDB* kdb_tbl = global_ws_tbl.get();
    KDB* kdb_var = global_ws_var.get();

    // Clear WS, then loads 3 WS and check ok
    //K_end_ws(0);
    U_test_load_fun_esv(filename);

    // (re-)creates vars AA...
    global_ws_cmt->clear();
    global_ws_idt->clear();
    global_ws_lst->clear();
    global_ws_tbl->clear();
    U_test_CreateObjects();

    // B_DataPattern()
    // Foireux. Faut utiliser des listes (avec A;B au lieu de $AB ca marche pas...) => A changer ? Voir B_DataListSort()
    B_DataPattern("RC xy $AB $BC", VARIABLES);
    lst = KLVAL(global_ws_lst.get(), "RC");
    EXPECT_EQ(std::string(lst), "AB,AC,BB,BC");

    // B_DataCalcVar()
    rc = B_DataCalcVar("A1 2 * B");
    A1 = KVVAL(global_ws_var.get(), "A1");
    EXPECT_EQ(rc, 0);
    found = global_ws_var->contains("A1");
    EXPECT_TRUE(found);
    EXPECT_EQ(A1[1], 4);

    // B_DataCreate(char* arg, int type)
    // B_DataDuplicate(char* arg, int type)
    // B_DataRename(char* arg, int type)
    // B_DataDelete(char* arg, int type)
    KDB* kdb;
    int pos;
    char* ptr_obj;
    for(i = 0; i < 7 ; i++) 
    {
        kdb = get_global_db(i);
        char* x_name = (i == SCALARS) ? (char*) "xxx" : (char*) "XXX";
        rc = B_DataCreate(x_name, i);
        EXPECT_EQ(rc, 0);
        found = kdb->contains(x_name);
        EXPECT_TRUE(found);
        pos = kdb->index_of(x_name);
        EXPECT_TRUE(pos >= 0);
        handle = kdb->get_handle(x_name);
        EXPECT_TRUE(handle > 0);
        ptr_obj = kdb->get_ptr_obj(x_name);
        EXPECT_STRNE(ptr_obj, "");

        // Equations cannot be renamed or duplicated
        if(i != EQUATIONS) 
        {   
            char* y_name = (i == SCALARS) ? (char*) "yyy" : (char*) "YYY"; 
            char* dup_names = (i == SCALARS) ? (char*) "xxx yyy" : (char*) "XXX YYY";
            rc = B_DataDuplicate(dup_names, i);
            EXPECT_EQ(rc, 0);
            found = kdb->contains(y_name);
            EXPECT_TRUE(found);
            pos = kdb->index_of(y_name);
            EXPECT_TRUE(pos >= 0);
            handle = kdb->get_handle(y_name);
            EXPECT_TRUE(handle > 0);
            ptr_obj = kdb->get_ptr_obj(y_name);
            EXPECT_STRNE(ptr_obj, "");

            char* z_name = (i == SCALARS) ? (char*) "zzz" : (char*) "ZZZ";
            char* ren_name = (i == SCALARS) ? (char*) "yyy zzz" : (char*) "YYY ZZZ";
            rc = B_DataRename(ren_name, i);
            EXPECT_EQ(rc, 0);
            found = kdb->contains(z_name);
            EXPECT_TRUE(found);
        }

        rc = B_DataDelete(x_name, i);
        EXPECT_EQ(rc, 0);
        found = kdb->contains(x_name);
        EXPECT_TRUE(!found);
    }

    kdb_cmt->set_obj("AAA", "This is a comment");
    handle = kdb_cmt->get_handle("AAA");
    EXPECT_TRUE(handle > 0);
    char* comment = (char*) KCVAL(kdb_cmt, handle);
    EXPECT_EQ(std::string(comment), "This is a comment");
    kdb_cmt->clear();
    global_ws_cmt->set_obj("AAA", "This is a comment");
    handle = global_ws_cmt->get_handle("AAA");
    EXPECT_TRUE(handle > 0);
    comment = (char*) KCVAL(global_ws_cmt.get(), handle);
    EXPECT_EQ(std::string(comment), "This is a comment");

    // B_DataListSort()
    success = global_ws_lst->set_obj("LIST1", "A;C;B");
    EXPECT_TRUE(success);
    found = global_ws_lst->contains("LIST1");
    EXPECT_TRUE(found);
    rc = B_DataListSort("LIST1 LIST2");
    EXPECT_EQ(rc, 0);
    lst = KLVAL(global_ws_lst.get(), "LIST2");
    EXPECT_EQ(std::string(lst), "A;B;C");

    // B_DataListSort() Example 2
    global_ws_lst->set_obj("L1", "C;B;$L2;$L3");
    global_ws_lst->set_obj("L2", "X Z Y");
    global_ws_lst->set_obj("L3", "A B D");
    rc = B_DataListSort("L1 RES");
    EXPECT_EQ(rc, 0);
    lst = KLVAL(global_ws_lst.get(), "RES");
    EXPECT_EQ(std::string(lst), "A;B;B;C;D;X;Y;Z");

    // B_DataUpdate()
    rc = B_DataUpdate("U Comment of U"       , COMMENTS);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(global_ws_cmt->get_obj("U")), "Comment of U");

    rc = B_DataUpdate("U U := c1 + c2*Z"     , EQUATIONS);
    EXPECT_EQ(rc, 0);

    rc = B_DataUpdate("U 2 * A"              , IDENTITIES);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KILEC(global_ws_idt.get(), "U")), "2 * A");

    rc = B_DataUpdate("U A,B,C"             , LISTS);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLVAL(global_ws_lst.get(), "U")), "A,B,C");

    rc = B_DataUpdate("u  1.2 1"             , SCALARS);
    EXPECT_EQ(rc, 0);
    val = K_s_get_value (global_ws_scl.get(), "u");
    EXPECT_DOUBLE_EQ(val, 1.2);

    rc = B_DataUpdate("U  Title of U;U;2*U"  , TABLES);
    EXPECT_EQ(rc, 0);
    smpl = global_ws_var->sample;
    rc = B_DataUpdate("U L 2000Y1 2 3.1 4e2" , VARIABLES);
    EXPECT_EQ(rc, 0);

    // B_DataSearch(char* arg, int type)
    rc = B_DataSearch("of 0 0 1 0 1 NEWLIST", COMMENTS);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLVAL(global_ws_lst.get(), "NEWLIST")), "U");

    // B_DataScan(char* arg, int type)
    rc = B_DataScan("U", EQUATIONS);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLVAL(global_ws_lst.get(), "_SCAL")), "c1;c2");

    // B_DataExist(char* arg, int type)
    rc = B_DataExist("_SCAL", LISTS);
    EXPECT_TRUE((rc >= 0));

    // B_DataAppend(char* arg, int type)
    rc = B_DataAppend("_SCAL XXX,YYY", LISTS);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLVAL(global_ws_lst.get(), "_SCAL")), "c1;c2,XXX,YYY");

    rc = B_DataAppend("U - More comment on U", COMMENTS);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(global_ws_cmt->get_obj("U")), "Comment of U - More comment on U");

    // B_DataList(char* arg, int type)
    rc = B_DataList("LC ac*", SCALARS);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLVAL(global_ws_lst.get(), "LC")), "acaf1;acaf2;acaf3;acaf4");
    printf("LC = \"%s\"\n", KLVAL(global_ws_lst.get(), "LC"));

    // B_DataCalcLst(char* arg, int unused)
    B_DataUpdate("LST1 A,B,C", LISTS);
    B_DataUpdate("LST2 C,D,E", LISTS);

    rc = B_DataCalcLst("_RES LST1 + LST2");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLVAL(global_ws_lst.get(), "_RES")), "A;B;C;D;E");

    rc = B_DataCalcLst("_RES LST1 * LST2");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLVAL(global_ws_lst.get(), "_RES")), "C");

    rc = B_DataCalcLst("_RES LST1 - LST2");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLVAL(global_ws_lst.get(), "_RES")), "A;B");

    rc = B_DataCalcLst("_RES LST1 x LST2");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLVAL(global_ws_lst.get(), "_RES")), "AC;AD;AE;BC;BD;BE;CC;CD;CE");

    // B_DataCompare(char* arg, int type)
    std::string expected_list;
    sprintf(buf,  "%sfun.al WS_ONLY FILE_ONLY BOTH_EQ BOTH_DIFF", input_test_dir);
    rc = B_DataCompare(buf, LISTS);
    EXPECT_EQ(rc, 0);
    // names only in current WS
    expected_list = "AB;BC;L1;L2;L3;LC;LIST1;LIST2;LST1;LST2;NEWLIST;RC;RES;U;ZZZ;_EXO;_RES";
    EXPECT_EQ(std::string(KLVAL(global_ws_lst.get(), "WS_ONLY")), expected_list);
    // names only in file
    expected_list = "COPY;COPY0;COPY1;ENDO;ENDO0;ENDO1;ENVI;IDT;MAINEQ;MYLIST;TOTAL;TOTAL0;";
    expected_list += "TOTAL1;XENVI;XSCENARIO;_SEARCH";
    EXPECT_EQ(std::string(KLVAL(global_ws_lst.get(), "FILE_ONLY")), expected_list);
    // names in both current WS and file and IODE obj in WS == IODE obj in file
    expected_list = "";
    EXPECT_EQ(std::string(KLVAL(global_ws_lst.get(), "BOTH_EQ")), expected_list);
    // names in both current WS and file but IODE obj in WS != IODE obj in file
    expected_list = "_SCAL";
    EXPECT_EQ(std::string(KLVAL(global_ws_lst.get(), "BOTH_DIFF")), expected_list);

    rc = B_DataPrintGraph("Grt Line No No Level -- -- 2000Y1 2015Y1 ACAF ACAG ACAF+ACAG");
    EXPECT_EQ(rc, 0);

    // B_DataDisplayGraph()
    rc = B_DataDisplayGraph("Grt Line No No Level -- -- 2000Y1 2015Y1 ACAF ACAG ACAF+ACAG");
    EXPECT_EQ(rc, 0);
}


TEST_F(IodeCAPITest, Tests_B_EQS)
{
    int     rc;
    char    cmd_B_EqsEstimate[] = "1980Y1 1996Y1 ACAF";
    char    cmd_B_EqsSetSample[] = "1981Y1 1995Y1 ACAF";
    KDB*    kdb_eqs = global_ws_eqs.get();

    U_test_print_title("Tests B_EQS");
    U_test_suppress_kmsg_msgs();

    // (Re-)loads 3 WS and check ok
    U_test_load_fun_esv("fun");

    // B_EqsEstimate()
    rc = B_EqsEstimate(cmd_B_EqsEstimate);
    EXPECT_EQ(rc, 0);

    EXPECT_DOUBLE_EQ(round(K_e_r2(global_ws_eqs.get(), "ACAF") * 1e8) / 1e8, 0.82181543);
    EXPECT_DOUBLE_EQ(round(K_e_fstat(global_ws_eqs.get(), "ACAF") * 1e8) / 1e8, 32.28510666);

    // B_EqsSetSample()
    rc = B_EqsSetSample(cmd_B_EqsSetSample);
    Sample smpl = KESMPL(global_ws_eqs.get(), "ACAF");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(smpl.start_period.year, 1981);

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_FILE)
{
    int     rc;

    U_test_print_title("Tests B_FILE");

    // Cleanup files
    _unlink("test.a2m");
    _unlink("tata.a2m");
    _unlink("tutu.a2m");

    //  Create a file
    U_test_create_a_file("test", W_A2M);

    //  Check that the file exists
    U_test_file_exists("test.a2m", "File %s exists");

    //  Call B_FileRename()
    rc = B_FileRename("test tata", FILE_A2M);
    rc = U_test_file_exists("tata.a2m", "B_FileRename(\"test tata\", FILE_A2M)");

    //  Call B_FileCopy()
    rc = B_FileCopy("tata tutu", FILE_A2M);
    rc = U_test_file_exists("tutu.a2m", "B_FileCopy(\"tata tutu \", FILE_A2M)");

    //  Call B_FileDelete()
    rc = B_FileDelete("tata tutu", FILE_A2M);
    rc = U_test_file_not_exists("tata.a2m", "B_FileDelete(\"tata tutu\", FILE_A2M)");

/*
    "fileimportvar",            B_FileImportVar,        SB_XodeRuleImport,  0,
    "fileimportcmt",            B_FileImportCmt,        SB_XodeRuleImport,  0,
*/

    U_test_reset_a2m_msgs();
}


TEST_F(IodeCAPITest, Tests_B_FSYS)
{
    int     rc;
    char    arg[1024];

    U_test_print_title("Tests B_FSYS");

    char test_a2m_file[1024];
    sprintf(test_a2m_file, "%stest.a2m", output_test_dir);

    char test_a2m_oem_file[1024]; 
    sprintf(test_a2m_oem_file, "%stest.a2m.oem", output_test_dir);

    char test_a2m_utf8_file[1024];
    sprintf(test_a2m_utf8_file, "%stest.a2m.utf8", output_test_dir);

    char test_a2m_ansi_file[1024];
    sprintf(test_a2m_ansi_file, "%stest.a2m.ansi", output_test_dir);

    char testdbl_a2m_ansi_file[1024];
    sprintf(testdbl_a2m_ansi_file, "%stestdbl.a2m.ansi", output_test_dir);

    char brol_a2m_ansi_file[1024];
    sprintf(brol_a2m_ansi_file, "%sbrol.a2m.ansi", output_test_dir);

    char brol2_a2m_ansi_file[1024];
    sprintf(brol2_a2m_ansi_file, "%sbrol2.a2m.ansi", output_test_dir);

    // Cleanup files
    _unlink(test_a2m_file);
    _unlink(test_a2m_oem_file);
    _unlink(test_a2m_utf8_file);
    _unlink(test_a2m_ansi_file);
    _unlink(testdbl_a2m_ansi_file);
    _unlink(brol_a2m_ansi_file);
    _unlink(brol2_a2m_ansi_file);

    //  Create test.a2m -> ansi-coded file
    // sprintf(arg, "%s\\test", output_test_dir);
    // U_test_create_a_file("test", W_A2M); // Ansi-coded file
    // => Pb with conversions => files created by test1.c differ from those created by test_c_api.cpp.
    // Therefore, we copy data\test.a2m into local test.a2m
    sprintf(arg, "%stest.a2m %s", input_test_dir, test_a2m_file);
    rc = B_SysCopy(arg);
    EXPECT_EQ(rc, 0);
    compare_files(input_test_dir, "test.a2m", output_test_dir, "test.a2m");

    // B_SysAnsiToOem() : translate ansi to oem -> test.a2m.oem
    sprintf(arg, "%stest.a2m %s", input_test_dir, test_a2m_oem_file);
    rc = B_SysAnsiToOem(arg);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "test.a2m.oem", output_test_dir, "test.a2m.oem.ref");

    // B_SysAnsiToUTF8() : translate ansi to utf8 -> test.a2m.utf8
    sprintf(arg, "%stest.a2m %s", input_test_dir, test_a2m_utf8_file);
    rc = B_SysAnsiToUTF8(arg);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "test.a2m.utf8", output_test_dir, "test.a2m.utf8.ref");

    // B_SysOemToAnsi() : translate oem to ansi -> test.a2m.ansi
    sprintf(arg, "%stest.a2m.oem %s", output_test_dir, test_a2m_ansi_file);
    rc = B_SysOemToAnsi(arg);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "test.a2m.ansi", output_test_dir, "test.a2m.ansi.ref");

    // B_SysOemToUTF8() : translate ansi to utf8 -> test.a2m.utf8
    sprintf(arg, "%stest.a2m.oem %s", output_test_dir, test_a2m_utf8_file);
    rc = B_SysOemToUTF8(arg);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "test.a2m.utf8", output_test_dir, "test.a2m.utf8.ref");

    // B_SysRename(char* arg, int unused) : rename test.a2m.ansi -> brol.a2m.ansi
    sprintf(arg, "%stest.a2m.ansi %s", output_test_dir, brol_a2m_ansi_file);
    rc = B_SysRename(arg);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "brol.a2m.ansi", output_test_dir, "test.a2m.ansi.ref");

    // B_SysCopy(char* arg, int unused) : copy brol.a2m.ansi dans testdbl.a2m.ansi
    sprintf(arg, "%sbrol.a2m.ansi %s", output_test_dir, testdbl_a2m_ansi_file);
    rc = B_SysCopy(arg);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "testdbl.a2m.ansi", output_test_dir, "test.a2m.ansi.ref");

    // B_SysAppend(char* arg, int unused) : append testdbl.a2m.ansi to brol.a2m.ansi
    sprintf(arg, "%sbrol.a2m.ansi %s", output_test_dir, testdbl_a2m_ansi_file);
    rc = B_SysAppend(arg);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "testdbl.a2m.ansi", output_test_dir, "testdbl.a2m.ansi.ref");

    // B_SysDelete(char* arg, int unused) : delete totdbl.a2m.ansi and brol.a2m.ansi
    char tutu_a2m_file[1024];
    sprintf(tutu_a2m_file, "%stutu.a2m", output_test_dir);
    sprintf(arg, "%s %s %s %s %s %s", brol_a2m_ansi_file, testdbl_a2m_ansi_file, 
            tutu_a2m_file, test_a2m_file, test_a2m_oem_file, test_a2m_utf8_file);
    rc = B_SysDelete(arg);
    rc = U_test_file_not_exists(brol_a2m_ansi_file,  "B_SysDelete(\"brol.a2m.ansi\")");
    rc = U_test_file_not_exists(testdbl_a2m_ansi_file, "B_SysDelete(\"testdbl.a2m.ansi\")");

    U_test_reset_a2m_msgs();
}


TEST_F(IodeCAPITest, Tests_B_IDT)
{
    char filename[256];
    int  rc;

    U_test_print_title("Tests B_IDT");

    // Init -> clear ws
    global_ws_cmt->clear();
    global_ws_idt->clear();
    global_ws_lst->clear();
    global_ws_tbl->clear();

    U_test_CreateObjects(); // Create vars on 2000Y1:2010Y1 => A=[0, 1...], B=[0, 2, 4...], BC...
    global_ws_idt->set_obj("C", new Identity("D*2+ACAF"));
    global_ws_idt->set_obj("D", new Identity("A+B"));

    // Trace the execution
    W_dest("test_idt", W_HTML);
    rc = B_IdtExecuteTrace("Yes");

    // Erroneously define input filenames (WS forgotten !!)
    sprintf(filename,  "%sfun", input_test_dir);
    rc = B_IdtExecuteVarFiles(filename);
    EXPECT_EQ(rc, 0);

    // Compute the idts of a partial sample
    // C = "D*2+ACAF"
    // D = "A+B"
    char idtexec[] = "2002Y1 2007Y1 C D";
    rc = B_IdtExecute(idtexec);
    EXPECT_NE(rc, 0);

    // Second trial with WS in filenames
    global_ws_var->clear();
    // Create vars on 2000Y1:2020Y1 
    //  => A = [0, 1...], B = [0, 2, 4...], AB = AC = BB = BC = B
    U_test_CreateObjects();

    sprintf(filename,  "WS %sfun", input_test_dir);
    rc = B_IdtExecuteVarFiles(filename);
    EXPECT_EQ(rc, 0);

    // Compute the idts of a partial sample
    // C = "D*2+ACAF"
    // D = "A+B"
    char idtexec2[] = "2002Y1 2007Y1 C D";
    rc = B_IdtExecute(idtexec2);
    EXPECT_EQ(rc, 0);

    // Check the values
    double* C = (double*) KVVAL(global_ws_var.get(), "C");
    double* D = (double*) KVVAL(global_ws_var.get(), "D");

    EXPECT_DOUBLE_EQ(D[1], IODE_NAN);
    EXPECT_DOUBLE_EQ(D[2], 2.0 + 4.0);
    EXPECT_DOUBLE_EQ(round(C[2] * 1e8) / 1e8, 6.0 * 2 - 0.92921251);
}


TEST_F(IodeCAPITest, Tests_B_IDT_EXECUTE)
{
    char    **idts;
    Sample  *smpl = NULL;
    double  *AOUC;
    int     rc;

    U_test_print_title("Tests B_IDT_EXECUTE");

    // Loads 3 WS and check ok
    U_test_load(IDENTITIES, "fun");
    U_test_load(VARIABLES, "fun");

    AOUC = KVVAL(global_ws_var.get(), "AOUC");
    AOUC[1] = 0.1;

    // Sample (null => full sample, see K_exec())
    smpl = new Sample("1961Y1", "2015Y1");   
    idts = (char**) SCR_vtoms((U_ch*) "AOUC", (U_ch*) " ,;\t");
    rc = B_IdtExecuteIdts(smpl, idts);
    printf("rc=%d\n", rc);
    EXPECT_DOUBLE_EQ(round(AOUC[1] * 1e8) / 1e8, 0.24783192);
}


TEST_F(IodeCAPITest, Tests_IMP_EXP)
{
    char    outfile[256];
    char    reffile[256];
    char    varfile[256];
    char    cmtfile[256];
    char    rulefile[256];
    char    trace[] = " ";
    int     rc;
    bool    success;

    U_test_print_title("Tests EXP: Export CSV and rcsv");

    U_test_suppress_kmsg_msgs();

    // Export
    // Exports VAR files into external formats.
    // int EXP_RuleExport(" "char* trace, NULL, char* out, char* vfile, char* cfile, char* from, char* to, char* na, char* sep, int fmt)

    sprintf(varfile, "%sfun.av", input_test_dir);
    sprintf(cmtfile, "%sfun.ac", input_test_dir);

    sprintf(outfile, "%sfun_xode.csv", output_test_dir);
    sprintf(reffile, "%sfun_xode.csv.ref", input_test_dir);
    rc = EXP_RuleExport(trace, NULL, outfile, varfile, cmtfile, "2000Y1", "2010Y1", "#N/A", ";", EXPORT_CSV);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "fun_xode.csv", input_test_dir, "fun_xode.csv.ref");

    sprintf(outfile, "%sfun_xode.rcsv", output_test_dir);
    sprintf(reffile, "%sfun_xode.rcsv.ref", input_test_dir);
    rc = EXP_RuleExport(trace, NULL, outfile, varfile, cmtfile, "2000Y1", "2010Y1", "#N/A", ";", EXPORT_RCSV);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "fun_xode.rcsv", input_test_dir, "fun_xode.rcsv.ref");

    // Export with rules (partial /+ change names)
    sprintf(outfile, "%sfun2.tsp", output_test_dir);
    sprintf(reffile, "%sfun2.ref.tsp", output_test_dir);
    sprintf(rulefile, "%srules.txt", input_test_dir);
    rc = EXP_RuleExport(trace, rulefile, outfile, varfile, cmtfile, "1995Y1", "2005Y1", "#N/A", ";", EXPORT_TSP);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "fun2.tsp", output_test_dir, "fun2.ref.tsp");

    U_test_print_title("Tests IMP VAR: Import Ascii Variables");

    sprintf(reffile, "%sfun_xode.av.ref", input_test_dir);
    sprintf(outfile, "%sfun_xode.var", output_test_dir);
    rc = IMP_RuleImport(VARIABLES, trace, NULL, outfile, reffile, "2000Y1", "2010Y1", IMPORT_ASCII, 0);
    EXPECT_EQ(rc, 0);

    CKDBVariables* kdb_var = new CKDBVariables(DB_GLOBAL);
    success = kdb_var->load(std::string(outfile));
    EXPECT_TRUE(success);
    global_ws_var.reset(kdb_var);
    U_test_lec("ACAF[2002Y1]", "ACAF[2002Y1]", 0, -0.92921251);

    U_test_print_title("Tests IMP CMT: Import Ascii Comments");

    sprintf(reffile, "%sfun_xode.ac.ref", input_test_dir);
    sprintf(outfile, "%sfun_xode.cmt", output_test_dir);
    rc = IMP_RuleImport(COMMENTS, trace, rulefile, outfile, reffile, NULL, NULL, IMPORT_ASCII, 0);
    EXPECT_EQ(rc, 0);

    if(rc == 0) 
    {
        CKDBComments* kdb_cmt = new CKDBComments(DB_GLOBAL);
        success = kdb_cmt->load(std::string(outfile));
        EXPECT_TRUE(success);
        global_ws_cmt.reset(kdb_cmt);
        EXPECT_TRUE(global_ws_cmt != nullptr);
        EXPECT_EQ(std::string(global_ws_cmt->get_obj("KK_AF")), "Ondernemingen: ontvangen kapitaaloverdrachten.");
    }

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_XODE)
{
    char    outfile[256];
    char    reffile[256];
    char    rulefile[256];
    char    cmd[512];
    char    trace[] = " ";
    int     rc;

    U_test_print_title("Tests XODE: Import ASCII via report function");
    U_test_suppress_kmsg_msgs();

    sprintf(reffile, "%sfun_xode.av.ref", input_test_dir);
    sprintf(outfile, "%sfun_xode.var", output_test_dir);
    sprintf(rulefile, "%srules.txt", input_test_dir);
    sprintf(cmd, "ASCII \"%s\" %s %s 2000Y1 2010Y1", rulefile, reffile, outfile);

    rc = B_FileImportVar(cmd);
    EXPECT_EQ(rc, 0);

    CKDBVariables* kdb_var = new CKDBVariables(DB_GLOBAL);
    bool success = kdb_var->load(std::string(outfile));
    EXPECT_TRUE(success);
    global_ws_var.reset(kdb_var);
    U_test_lec("KK_AF[2002Y1]", "KK_AF[2002Y1]", 0, -0.92921251);

    U_test_reset_kmsg_msgs();

}


TEST_F(IodeCAPITest, Tests_B_LTOH)
{
    char    cmd[512];
    char    varfile[256];
    Sample  *smpl;
    int     rc;

    U_test_print_title("Tests B_LTOH: convert low periodicity to high periodicity");
    U_test_suppress_kmsg_msgs();

    // Clear the vars and set the sample for the variable WS
    global_ws_var->clear();
    smpl = new Sample("2010Q1", "2020Q4");
    KV_sample(global_ws_var.get(), smpl);
    delete smpl;

    sprintf(varfile, "%sfun.av", input_test_dir);

    // Linear interpolation / stock
    sprintf(cmd, "L %s ACAF", varfile);
    rc = B_WsLtoHStock(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAF[2014Q3]", "ACAF[2014Q3]", 0, -79.729132);

    // Linear interpolation / flow
    sprintf(cmd, "L %s ACAG", varfile);
    rc = B_WsLtoHFlow(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAG[2014Q3]", "ACAG[2014Q3]", 0, 8.105075);

    // Cubic Splines / stock
    sprintf(cmd, "C %s ACAF", varfile);
    rc = B_WsLtoHStock(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAF[2012Q3]", "ACAF[2012Q3]", 0, -52.805666);

    // Cubic splines / flow
    sprintf(cmd, "C %s ACAG", varfile);
    rc = B_WsLtoHFlow(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAG[2012Q3]", "ACAG[2012Q3]", 0, 7.6135768);

    // Step / stock
    sprintf(cmd, "S %s ACAF", varfile);
    rc = B_WsLtoHStock(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAF[2014Q3]", "ACAF[2014Q3]", 0, -83.340625);

    // Step / flow
    sprintf(cmd, "S %s ACAG", varfile);
    rc = B_WsLtoHFlow(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAG[2014Q3]", "ACAG[2014Q3]", 0, 8.1050747);

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_HTOL)
{
    char    cmd[512];
    char    varfile[256];
    Sample  *smpl;
    int     rc;
    //char    current_dir[256];

    //getcwd(current_dir, sizeof(current_dir));
    //printf("%s\n", current_dir);

    U_test_print_title("Tests B_HTOL: convert high periodicity to low periodicity");
    U_test_suppress_kmsg_msgs();

    // Clear the vars and set the sample for the variable WS
    global_ws_var->clear();
    smpl = new Sample("2000Y1", "2020Y1");
    KV_sample(global_ws_var.get(), smpl);
    delete smpl;

    sprintf(varfile, "%sfun_q.var", input_test_dir);

    // Last Obs
    sprintf(cmd, "%s ACAF", varfile);
    rc = B_WsHtoLLast(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAF[2014Y1]", "ACAF[2014Y1]", 0, -83.340625);

    // Mean
    sprintf(cmd, "%s ACAG", varfile);
    rc = B_WsHtoLMean(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAG[2014Y1]", "ACAG[2014Y1]", 0, 8.1050747);

    // Sum
    sprintf(cmd, "%s AOUC", varfile);
    rc = B_WsHtoLSum(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("AOUC[2014Y1]", "AOUC[2014Y1]", 0, 1.423714 );

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_MODEL)
{
    KDB         *kdbv,
                *kdbe,
                *kdbs;
    char        *filename = "fun";
    int         rc;
    double      XNATY_2000Y1;

    // B_Model*() tests
    // ----------------
    // X int B_ModelSimulate(char *arg)                              $ModelSimulate per_from per_to equation_list
    // X int B_ModelSimulateParms(char* arg, int unused)                         $ModelSimulateParms eps relax maxit {Connex | Triang | None } 0 - 4 (starting values) {Yes | no } {yes | No } nbtri
    // X int B_ModelExchange(char* arg, int unused)                              $ModelExchange eqname1-varname1,eqname2-varname2,...
    // X int B_ModelCompile(char* arg, int unused)                               $ModelCompile  [eqname1, eqname2, ... ]
    // X int B_ModelCalcSCC(char *arg)                               $ModelCalcSCC nbtris prename intername postname [eqs]
    // X int B_ModelSimulateSCC(char *arg)                           $ModelSimulateSCC from to pre inter post
    // int B_ModelSimulateSaveNIters(char *arg)                    $ModelSimulateSaveNiters varname
    // int B_ModelSimulateSaveNorms(char *arg)                     $ModelSimulateSaveNorms varname

    U_test_print_title("Tests B_Model*(): simulation parameters and model simulation");
    U_test_suppress_kmsg_msgs();

    // Loads 3 WS and check ok
    U_test_load_fun_esv(filename);

    // Check
    kdbv = global_ws_var.get();
    EXPECT_NE(kdbv, nullptr);
    kdbs = global_ws_scl.get();
    EXPECT_NE(kdbs, nullptr);
    kdbe = global_ws_eqs.get();
    EXPECT_NE(kdbe, nullptr);

    // B_ModelSimulateParms()
    CSimulation::KSIM_START = VAR_INIT_TM1;
    CSimulation::KSIM_EPS = 0.00001;
    CSimulation::KSIM_MAXIT = 1000;
    CSimulation::KSIM_RELAX = 1.0;
    CSimulation::KSIM_SORT = 0;
    CSimulation::KSIM_PASSES = 3;
    CSimulation::KSIM_DEBUG = 1;
    rc = B_ModelSimulateParms("0.0001 0.7 100 Triang 0 no 5 no");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(CSimulation::KSIM_EPS, 0.0001);
    EXPECT_EQ(CSimulation::KSIM_MAXIT, 100);
    EXPECT_EQ(CSimulation::KSIM_RELAX, 0.7);
    EXPECT_EQ(CSimulation::KSIM_DEBUG, 0);
    EXPECT_EQ(CSimulation::KSIM_PASSES, 5);

    // B_ModelSimulate()
    rc = B_ModelSimulate("2000Y1 2002Y1");
    EXPECT_EQ(rc, 0);
    // TODO: check result of one ENDO
    EXPECT_DOUBLE_EQ(round(KV_get_at_aper("ACAF", "2002Y1") * 1e6) / 1e6, -1.274623);

    // B_ModelExchange()

    // Reloads 3 WS
    U_test_load_fun_esv(filename);

    // Check
    kdbv = global_ws_var.get();
    EXPECT_NE(kdbv, nullptr);
    kdbs = global_ws_scl.get();
    EXPECT_NE(kdbs, nullptr);
    kdbe = global_ws_eqs.get();
    EXPECT_NE(kdbe, nullptr);

    // Set values of endo UY
    KV_set_at_aper("UY", "2000Y1", 650.0);
    KV_set_at_aper("UY", "2001Y1", 670.0);
    KV_set_at_aper("UY", "2002Y1", 680.0);

    // Exchange
    rc = B_ModelExchange("UY-XNATY");
    EXPECT_EQ(rc, 0);

    // Simulate
    rc = B_ModelSimulate("2000Y1 2002Y1");
    EXPECT_EQ(rc, 0);

    // Check some results
    EXPECT_EQ(KV_get_at_aper("UY", "2000Y1"), 650.0);
    XNATY_2000Y1 = KV_get_at_aper("XNATY", "2000Y1");
    //printf("XNATY_2000Y1 = %lg\n", XNATY_2000Y1);
    EXPECT_DOUBLE_EQ(round(KV_get_at_aper("XNATY", "2000Y1") * 1e6) / 1e6, 0.800673);

    // B_ModelCompile(char* arg, int unused)
    rc = B_ModelCompile("");
    EXPECT_EQ(rc, 0);

    // $ModelCalcSCC nbtris prename intername postname [eqs]
    rc = B_ModelCalcSCC("5 _PRE2 _INTER2 _POST2");
    EXPECT_EQ(rc, 0);
    std::string expected_list = "BRUGP;DTH1C;EX;ITCEE;ITCR;ITGR;ITI5R;ITIFR;ITIGR;ITMQR;";
    expected_list += "NATY;POIL;PW3;PWMAB;PWMS;PWXAB;PWXS;PXE;QAH;QWXAB;QWXS;QWXSS;SBGX;";
    expected_list += "TFPFHP_;TWG;TWGP;ZZF_;DTH1;PME;PMS;PMT";
    EXPECT_EQ(std::string(KLVAL(global_ws_lst.get(), "_PRE2")), expected_list);

    // int B_ModelSimulateSCC(char *arg)    $ModelSimulateSCC from to pre inter post
    //  1. Annuler Exchange
    rc = B_ModelExchange("");
    EXPECT_EQ(rc, 0);

    //  2. ReLoads 3 WS to reset EXO XNATY to its original value
    U_test_load_fun_esv(filename);

    // Check
    kdbv = global_ws_var.get();
    EXPECT_NE(kdbv, nullptr);
    kdbs = global_ws_scl.get();
    EXPECT_NE(kdbs, nullptr);
    kdbe = global_ws_eqs.get();
    EXPECT_NE(kdbe, nullptr);

    //  3. Simulate & compare
    rc = B_ModelSimulateSCC("2000Y1 2002Y1 _PRE2 _INTER2 _POST2");
    EXPECT_EQ(rc, 0);
    EXPECT_DOUBLE_EQ(round(KV_get_at_aper("ACAF", "2002Y1") * 1e6) / 1e6, -1.274623);

    // B_ModelSimulateSaveNIters(char *arg)                    
    // $ModelSimulateSaveNiters varname
    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_WsLoad)
{
    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    U_test_B_WsLoad("fun", COMMENTS, 317);
    U_test_B_WsLoad("fun", EQUATIONS, 274);
    U_test_B_WsLoad("fun", IDENTITIES, 48);
    U_test_B_WsLoad("fun", LISTS, 17);
    U_test_B_WsLoad("fun", SCALARS, 161);
    U_test_B_WsLoad("fun", TABLES, 46);
    U_test_B_WsLoad("fun", VARIABLES, 394);

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_KEVAL)
{
	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);

    // check equation->endo == equation name
    for(auto& [name, _] : global_ws_eqs->k_objs)
        ASSERT_EQ(KEVAL(global_ws_eqs.get(), name)->endo, name);

    U_test_reset_kmsg_msgs();
}

TEST_F(IodeCAPITest, Tests_B_WsSave)
{
	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);

    // int B_WsSave(char* arg, int type)                 $WsSave<type> filename
    U_test_print_title("B_WsSave()");
    U_test_B_WsSave("fun", "fun2", COMMENTS, 317);
    U_test_B_WsSave("fun", "fun2", EQUATIONS, 274);
    U_test_B_WsSave("fun", "fun2", IDENTITIES, 48);
    U_test_B_WsSave("fun", "fun2", LISTS, 17);
    U_test_B_WsSave("fun", "fun2", SCALARS, 161);
    U_test_B_WsSave("fun", "fun2", TABLES, 46);
    U_test_B_WsSave("fun", "fun2", VARIABLES, 394);

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_WsSaveCmp)
{
	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);

    // int B_WsSaveCmp(char* arg, int type)              $WsSaveCmp<type> filename
    U_test_print_title("B_WsSaveCmp()");
    U_test_B_WsSaveCmp("fun", "fun2cmp", COMMENTS, 317);
    U_test_B_WsSaveCmp("fun", "fun2cmp", EQUATIONS, 274);
    U_test_B_WsSaveCmp("fun", "fun2cmp", IDENTITIES, 48);
    U_test_B_WsSaveCmp("fun", "fun2cmp", LISTS, 17);
    U_test_B_WsSaveCmp("fun", "fun2cmp", SCALARS, 161);
    U_test_B_WsSaveCmp("fun", "fun2cmp", TABLES, 46);
    U_test_B_WsSaveCmp("fun", "fun2cmp", VARIABLES, 394);

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_WsExport)
{
	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);



    // int B_WsExport(char* arg, int type)               $WsExport<type> filename
    U_test_print_title("B_WsExport()");
    U_test_B_WsExport("fun.cmt", "fun2.ac", COMMENTS);
    U_test_B_WsExport("fun.eqs", "fun2.ae", EQUATIONS);
    U_test_B_WsExport("fun.idt", "fun2.ai", IDENTITIES);
    U_test_B_WsExport("fun.lst", "fun2.al", LISTS);
    U_test_B_WsExport("fun.scl", "fun2.as", SCALARS);
    U_test_B_WsExport("fun.tbl", "fun2.at", TABLES);
    U_test_B_WsExport("fun.var", "fun2.av", VARIABLES);

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_WsClear)
{
	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);

    // int B_WsClear(char* arg, int type)                $WsClear<type>
    U_test_print_title("B_WsClear()");
    U_test_B_WsClear(COMMENTS);
    U_test_B_WsClear(EQUATIONS);
    U_test_B_WsClear(IDENTITIES);
    U_test_B_WsClear(LISTS);
    U_test_B_WsClear(SCALARS);
    U_test_B_WsClear(TABLES);
    U_test_B_WsClear(VARIABLES);

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_WsImport)
{
	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);

    // int B_WsImport(char* arg, int type)               $WsImport<type> filename
    U_test_print_title("B_WsImport()");
    U_test_B_WsImport("fun2.ac", COMMENTS, 317);
    U_test_B_WsImport("fun2.ae", EQUATIONS, 274);
    U_test_B_WsImport("fun2.ai", IDENTITIES, 48);
    U_test_B_WsImport("fun2.al", LISTS, 17);
    U_test_B_WsImport("fun2.as", SCALARS, 161);
    U_test_B_WsImport("fun2.at", TABLES, 46);
    U_test_B_WsImport("fun2.av", VARIABLES, 394);
    // TODO : correct fun.eqs (W) and fun.idt (NAWRU)

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_WsSample)
{
    int     rc;
    Sample  *smpl;

	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);

    // int B_WsSample(char* arg, int unused)                         $WsSample period_from period_to
    U_test_print_title("B_WsSample()");
    rc = B_WsSample("1965Y1 2020Y1");
    smpl = new Sample("1965Y1", "2020Y1");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(*global_ws_var->sample, *smpl);
    delete smpl;

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_WsClearAll)
{
    int rc;

	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);

    // int B_WsClearAll(char* arg, int unused)                       $WsClearAll
    U_test_print_title("B_WsClearAll()");
    rc = B_WsClearAll("");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(global_ws_cmt->size(), 0);
    EXPECT_EQ(global_ws_eqs->size(), 0);
    EXPECT_EQ(global_ws_idt->size(), 0);
    EXPECT_EQ(global_ws_lst->size(), 0);
    EXPECT_EQ(global_ws_scl->size(), 0);
    EXPECT_EQ(global_ws_tbl->size(), 0);
    EXPECT_EQ(global_ws_var->size(), 0);

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_WsDescr)
{
	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);

    // int B_WsDescr(char* arg, int type)                $WsDescr<type> free text
    U_test_print_title("B_WsDescr()");
    U_test_B_WsDescr("Ws content description", COMMENTS);
    U_test_B_WsDescr("Ws content description", EQUATIONS);
    U_test_B_WsDescr("Ws content description", IDENTITIES);
    U_test_B_WsDescr("Ws content description", LISTS);
    U_test_B_WsDescr("Ws content description", SCALARS);
    U_test_B_WsDescr("Ws content description", TABLES);
    U_test_B_WsDescr("Ws content description", VARIABLES);

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_WsName)
{
	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);

    U_test_print_title("B_WsName()");
    U_test_B_WsName("funtest", COMMENTS);
    U_test_B_WsName("funtest", EQUATIONS);
    U_test_B_WsName("funtest", IDENTITIES);
    U_test_B_WsName("funtest", LISTS);
    U_test_B_WsName("funtest", SCALARS);
    U_test_B_WsName("funtest", TABLES);
    U_test_B_WsName("funtest", VARIABLES);

    U_test_reset_kmsg_msgs();
}

TEST_F(IodeCAPITest, Tests_B_WsCopy)
{
	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);

    // int B_WsCopy(char* arg, int type)                 $WsCopy<type> fichier;fichier;.. obj1 obj2... or $WsCopyVar file;file;.. [from to] obj1 obj2...
    U_test_print_title("B_WsCopy() - VARIABLES");
    U_test_B_WsCopyVar();

    U_test_print_title("B_WsCopy() - other objects");
    U_test_B_WsCopy("fun", COMMENTS, 317);
    U_test_B_WsCopy("fun", EQUATIONS, 274);  // scalar gamma in EQ W is illegal since the implementation th gamma function in LEC
    U_test_B_WsCopy("fun", IDENTITIES, 48);   // Idem in IDT NAWRU
    U_test_B_WsCopy("fun", LISTS, 17);
    U_test_B_WsCopy("fun", SCALARS, 161);
    U_test_B_WsCopy("fun", TABLES, 46);

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_WsMerge)
{
	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);

    // int B_WsMerge(char* arg, int type)                $WsMerge<type> filename
    U_test_print_title("B_WsMerge() - VARIABLES");
    U_test_B_WsMergeVar();

    U_test_print_title("B_WsMerge() - other objects");
    U_test_B_WsMerge("fun", COMMENTS, 317);
    U_test_B_WsMerge("fun", EQUATIONS, 274);
    U_test_B_WsMerge("fun", IDENTITIES, 48);
    U_test_B_WsMerge("fun", SCALARS, 161);
    U_test_B_WsMerge("fun", TABLES, 46);

    U_test_reset_kmsg_msgs();
}

TEST_F(IodeCAPITest, Tests_B_WsExtrapolate)
{
// disable this test function in case AddressSanitizer is activated because 
// it crashes on Windows Server 2022 (Github container)
#if !(defined _MSC_VER && defined __SANITIZE_ADDRESS__)

	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);

    // int B_WsExtrapolate(char* arg, int unused)                    $WsExtrapolate [method] from to [variable list]
    U_test_print_title("B_WsExtrapolate");
    U_test_B_WsExtrapolate(0, 6.0);
    U_test_B_WsExtrapolate(1, 4.0);
    U_test_B_WsExtrapolate(2, 7.0);
    U_test_B_WsExtrapolate(3, 7.0);
    U_test_B_WsExtrapolate(4, IODE_NAN);
    U_test_B_WsExtrapolate(5, 6.0);
    U_test_B_WsExtrapolate(6, 7.0);
    
    U_test_reset_kmsg_msgs();
#endif
}


TEST_F(IodeCAPITest, Tests_B_WsAggregate)
{
	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);

    // int B_WsAggrChar(char* arg, int unused)                       $WsAggrChar char
    // int B_WsAggrSum(char* arg, int unused)                        $WsAggrSum pattern filename
    // int B_WsAggrProd(char* arg, int unused)                       $WsAggrProd pattern filename
    // int B_WsAggrMean(char* arg, int unused)                       $WsAggrMean pattern filename
    U_test_print_title("B_WsAggregate");
    U_test_B_WsAggregate();

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_StatUnitRoot)
{
	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);

    // int B_StatUnitRoot(char* arg, int unused)                     $StatUnitRoot drift trend order expression
    U_test_print_title("B_StatUnitRoot");
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, VARIABLES);

    U_test_B_StatUnitRoot(0, 0, 0, "ACAF", 0.958325);
    U_test_B_StatUnitRoot(1, 0, 0, "ACAF", 1.117498);
    U_test_B_StatUnitRoot(1, 1, 0, "ACAF", -0.798686);
    U_test_B_StatUnitRoot(0, 0, 2, "ACAF", 1.419631);

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_Csv)
{
	char fullfilename[256];
	sprintf(fullfilename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    B_WsLoad(fullfilename, COMMENTS);
    B_WsLoad(fullfilename, EQUATIONS);
    B_WsLoad(fullfilename, IDENTITIES);
    B_WsLoad(fullfilename, LISTS);
    B_WsLoad(fullfilename, SCALARS);
    B_WsLoad(fullfilename, TABLES);
    B_WsLoad(fullfilename, VARIABLES);

    // int B_CsvNbDec(char *nbdec, int unused)                       $CsvNbDec nn
    // int B_CsvSep(char *sep, int unused)                           $CsvSep char
    // int B_CsvNaN(char *nan, int unused)                           $CsvNaN text
    // int B_CsvAxes(char *var, int unused)                          $CsvAxes AxisName
    // int B_CsvDec(char *dec, int unused)                           $CsvDec char
    // int B_CsvSave(char* arg, int type)                            $CsvSave<type> file name1 name2 ...
    U_test_print_title("B_Csv*");
    U_test_B_Csv();

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_REP_LINE)
{
    int     rc;
    char    fullfilename[256], cmd[1024];

    U_test_print_title("Tests B_ReportLine()");
    kmsg_toggle(1);

    // Simple test of a call to B_ReportLine(). More elaborate commands are tested with B_ReportExec()
    sprintf(fullfilename,  "%s%s", input_test_dir, "fun.av");
    sprintf(cmd, "\n"
                 "$WsClearVar\n"
                 "$WsLoadVar %s\n", fullfilename);

    rc = B_ReportLine(cmd, 1);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(global_ws_var->size(), 394);

    rc = B_ReportLine("$settime 2000Y1", 0);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(RP_PER.to_string(), "2000Y1");

    rc = B_ReportLine("$incrtime 2", 0);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(RP_PER.to_string(), "2002Y1");
}


TEST_F(IodeCAPITest, Tests_B_REP_ENGINE)
{
    int     rc;
    char    cmd[1024];

    U_test_print_title("Tests B_ReportExec(\"rep_expand.rep\")");
    kmsg_toggle(1);

    // Report rep_expand.rep: expand %% {lec}, {$cmd}, {$!cmd} and @fn().
    RP_STDOUT = 1;      // Enable report to stdout for this test
    sprintf(cmd,  "%srep_expand.rep %s %s", report_test_dir, input_test_dir, output_test_dir);
    rc = B_ReportExec(cmd);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "rep_expand.a2m", output_test_dir, "rep_expand.ref.a2m");
}


TEST_F(IodeCAPITest, Tests_B_REP_FNS)
{
    int     rc;
    char    cmd[1024];

    U_test_print_title("Tests B_ReportExec(\"rep_fns.rep\")");
    kmsg_toggle(1);

    // Execution of the report rep_fns.rep
    RP_STDOUT = 1;      // Enable report to stdout for this test
    sprintf(cmd,  "%srep_fns.rep %s %s", report_test_dir, input_test_dir, output_test_dir);
    rc = B_ReportExec(cmd);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "rep_fns.a2m", output_test_dir, "rep_fns.ref.a2m");
}


TEST_F(IodeCAPITest, Tests_B_REP_PROC)
{
    int     rc;
    char    cmd[1024];

    U_test_print_title("Tests B_ReportExec(\"rep_proc.rep\")");
    kmsg_toggle(1);

    // Execution of the report rep_fns.rep
    RP_STDOUT = 1;      // Enable report to stdout for this test
    sprintf(cmd,  "%srep_proc.rep %s %s", report_test_dir, input_test_dir, output_test_dir);
    rc = B_ReportExec(cmd);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "rep_proc.a2m", output_test_dir, "rep_proc.ref.a2m");
}

TEST_F(IodeCAPITest, Tests_B_PRINT_Table_DEF)
{
    char in_filename[256];
    sprintf(in_filename,  "%s%s", input_test_dir, "fun");

    U_test_print_title("Tests B_PrintObjDef()");

    B_WsLoad(in_filename, COMMENTS);
    B_WsLoad(in_filename, EQUATIONS);
    B_WsLoad(in_filename, IDENTITIES);
    B_WsLoad(in_filename, LISTS);
    B_WsLoad(in_filename, SCALARS);
    B_WsLoad(in_filename, TABLES);
    B_WsLoad(in_filename, VARIABLES);

    kmsg_toggle(1);
    char output_filename[256];

    B_TABLE_TITLE = 1;
    sprintf(output_filename, "%s%s", output_test_dir, "tables_titles.csv");
    W_dest(output_filename, A2M_DESTCSV);
    B_PrintObjDef("ANAKNFF ANAPRIX", TABLES);
    W_close();
    compare_files(output_test_dir, "tables_titles.csv", output_test_dir, "tables_titles.ref.csv");

    B_TABLE_TITLE = 0;
    sprintf(output_filename, "%s%s", output_test_dir, "tables_full_defs.csv");
    W_dest(output_filename, A2M_DESTCSV);
    B_PrintObjDef("ANAKNFF ANAPRIX", TABLES);
    W_close();
    compare_files(output_test_dir, "tables_full_defs.csv", output_test_dir, "tables_full_defs.ref.csv");

    kmsg_toggle(0);
}


TEST_F(IodeCAPITest, Tests_RAS_EXECUTE)
{
    char* pattern = "xy";
    char* xdim = "$X";
    char* ydim = "$Y";
    Period* ref_period = new Period("2000Y1");
    Period* sum_period = new Period("2001Y1");
    int maxit = 100;
    double eps = 0.0001;

    int res;
    B_WsClear("Var", VARIABLES);
    B_WsSample("2000Y1 2001Y1");

    KV_add(global_ws_var.get(), "R1C1");
    *KVVAL(global_ws_var.get(), "R1C1", 0) = 5.0;
    KV_add(global_ws_var.get(), "R1C2");
    *KVVAL(global_ws_var.get(), "R1C2", 0) = 3.0;
    KV_add(global_ws_var.get(), "R1C3");
    *KVVAL(global_ws_var.get(), "R1C3", 0) = 5.0;
    KV_add(global_ws_var.get(), "R1C4");
    *KVVAL(global_ws_var.get(), "R1C4", 0) = 7.0;
    *KVVAL(global_ws_var.get(), "R1C4", 1) = 5.0;
    KV_add(global_ws_var.get(), "R1CT");
    *KVVAL(global_ws_var.get(), "R1CT", 0) = 20.0;
    *KVVAL(global_ws_var.get(), "R1CT", 1) = 20.0;

    KV_add(global_ws_var.get(), "R2C1");
    *KVVAL(global_ws_var.get(), "R2C1", 0) = 1.0;
    KV_add(global_ws_var.get(), "R2C2");
    *KVVAL(global_ws_var.get(), "R2C2", 0) = 1.0;
    *KVVAL(global_ws_var.get(), "R2C2", 1) = 2.0;
    KV_add(global_ws_var.get(), "R2C3");
    *KVVAL(global_ws_var.get(), "R2C3", 0) = 4.0;
    KV_add(global_ws_var.get(), "R2C4");
    *KVVAL(global_ws_var.get(), "R2C4", 0) = 4.0;
    KV_add(global_ws_var.get(), "R2CT");
    *KVVAL(global_ws_var.get(), "R2CT", 0) = 10.0;
    *KVVAL(global_ws_var.get(), "R2CT", 1) = 10.0;

    KV_add(global_ws_var.get(), "R3C1");
    *KVVAL(global_ws_var.get(), "R3C1", 0) = 3.0;
    KV_add(global_ws_var.get(), "R3C2");
    *KVVAL(global_ws_var.get(), "R3C2", 0) = 1.0;
    KV_add(global_ws_var.get(), "R3C3");
    *KVVAL(global_ws_var.get(), "R3C3", 0) = 3.0;
    *KVVAL(global_ws_var.get(), "R3C3", 1) = 2.0;
    KV_add(global_ws_var.get(), "R3C4");
    *KVVAL(global_ws_var.get(), "R3C4", 0) = 3.0;
    KV_add(global_ws_var.get(), "R3CT");
    *KVVAL(global_ws_var.get(), "R3CT", 0) = 10.0;
    *KVVAL(global_ws_var.get(), "R3CT", 1) = 10.0;

    KV_add(global_ws_var.get(), "R4C1");
    *KVVAL(global_ws_var.get(), "R4C1", 0) = 1.0;
    *KVVAL(global_ws_var.get(), "R4C1", 1) = 0.0;
    KV_add(global_ws_var.get(), "R4C2");
    *KVVAL(global_ws_var.get(), "R4C2", 0) = 2.0;
    KV_add(global_ws_var.get(), "R4C3");
    *KVVAL(global_ws_var.get(), "R4C3", 0) = 1.0;
    KV_add(global_ws_var.get(), "R4C4");
    *KVVAL(global_ws_var.get(), "R4C4", 0) = 1.0;
    KV_add(global_ws_var.get(), "R4CT");
    *KVVAL(global_ws_var.get(), "R4CT", 0) = 5.0;
    *KVVAL(global_ws_var.get(), "R4CT", 1) = 5.0;

    KV_add(global_ws_var.get(), "RTC1");
    *KVVAL(global_ws_var.get(), "RTC1", 0) = 10.0;
    *KVVAL(global_ws_var.get(), "RTC1", 1) = 10.0;
    KV_add(global_ws_var.get(), "RTC2");
    *KVVAL(global_ws_var.get(), "RTC2", 0) = 7.0;
    *KVVAL(global_ws_var.get(), "RTC2", 1) = 7.0;
    KV_add(global_ws_var.get(), "RTC3");
    *KVVAL(global_ws_var.get(), "RTC3", 0) = 13.0;
    *KVVAL(global_ws_var.get(), "RTC3", 1) = 13.0;
    KV_add(global_ws_var.get(), "RTC4");
    *KVVAL(global_ws_var.get(), "RTC4", 0) = 15.0;
    *KVVAL(global_ws_var.get(), "RTC4", 1) = 15.0;

    KV_add(global_ws_var.get(), "RTCT");
    *KVVAL(global_ws_var.get(), "RTCT", 0) = 90.0;
    *KVVAL(global_ws_var.get(), "RTCT", 1) = 90.0;

    global_ws_lst->set_obj("X", "R1,R2,R3,R4,RT");
    global_ws_lst->set_obj("Y", "C1,C2,C3,C4,CT");

    bool found = global_ws_lst->contains("X");
    EXPECT_TRUE(found);
    found = global_ws_lst->contains("Y");
    EXPECT_TRUE(found);

    // Note: ref_period and sum_period are freed in RasExecute()
    res = RasExecute(pattern, xdim, ydim, ref_period, sum_period, maxit, eps);
    delete ref_period;
    delete sum_period;

    EXPECT_EQ(res, 0);
}
