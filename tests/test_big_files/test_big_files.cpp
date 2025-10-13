#pragma once
#include "gtest/gtest.h"
#include "api/all.h"
#include "cpp_api/iode_cpp_api.h"

#include <string>
#include <filesystem>


TEST(BigFilesTest, Tests_BIG_WS)
{
    int load_all;
    int db_global;
    int nb_names;
    int all_nb_names;
    KDB* global_kdb;
    KDB* kdb_var = nullptr;
    KDB* kdb_shallow_copy = nullptr;
    KDBVariables* cpp_kdb_var;
    char fullfilename[256];
    char out_filename[256];
    std::chrono::duration<double> elapsed;

    kpause_continue = 1;
    // suppress messages from kmsg()
    kmsg_toggle(0);

	std::string current_file = __FILE__;
	std::filesystem::path cwd(current_file);
	std::filesystem::path current_dir = cwd.parent_path();
    std::filesystem::path data_dir = current_dir / "data";

    std::string str_separator;
    std::string prefix_filename;
#ifdef __GNUC__
    str_separator = "/";
	prefix_filename = "linux_";
#else
    str_separator = "\\";
	prefix_filename = "";
#endif
    std::string str_current_dir = current_dir.string() + str_separator;
	std::string str_data_dir  = data_dir.string() + str_separator;

    // WARNING: Do not use a pattern that could lead to duplicated names 
    //          when expand (e.g. "A*;0*")
    const char* pattern = "A*;Z*";
   
    // >>>>>>>>>>>> BINARY FILES <<<<<<<<<<<<
    sprintf(fullfilename,  "%s%s", str_data_dir.c_str(), "big.var");
    if(std::filesystem::exists(fullfilename))
    {
        std::cout << "File big.var found" << std::endl;

        // **** K_load() performance tests ****
        std::cout << "\n---- Testing performance of loading the binary file 'big.var' ----\n" << std::endl;

        // ==== Load all variables from big.var in GLOBAL and STANDALONE modes ====
        load_all = 0;

        // ---- GLOBAL ----
        db_global = 1;
        auto start = std::chrono::high_resolution_clock::now();
        kdb_var = K_load(VARIABLES, fullfilename, load_all, NULL, db_global);
        EXPECT_TRUE(kdb_var != nullptr);
        EXPECT_EQ(kdb_var->size(), 175760);
        EXPECT_EQ(kdb_var->sample->to_string(), "1990Y1:2060Y1");
        EXPECT_EQ(kdb_var->sample->nb_periods, 71);
        auto end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(GLOBAL - ALL VARS)      loaded " << std::to_string(kdb_var->size()) 
                    << " variables in " << elapsed.count() << " seconds" << std::endl;

        delete kdb_var; 

        // ---- STANDALONE ----
        db_global = 0;
        start = std::chrono::high_resolution_clock::now();
        kdb_var = K_load(VARIABLES, fullfilename, load_all, NULL, db_global);
        EXPECT_TRUE(kdb_var != nullptr);
        EXPECT_EQ(kdb_var->size(), 175760);
        EXPECT_EQ(kdb_var->sample->to_string(), "1990Y1:2060Y1");
        EXPECT_EQ(kdb_var->sample->nb_periods, 71);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(STANDALONE - ALL VARS)  loaded " << std::to_string(kdb_var->size()) 
                    << " variables in " << elapsed.count() << " seconds" << std::endl; 

        // ---- Retrieve list of variable names matching a specific pattern ----
        char* OLD_SEPS = A_SEPS;
        A_SEPS = (char*) ";\t\n";

        start = std::chrono::high_resolution_clock::now();
        // Retrieves all object names matching one or more patterns in K_WS (similar to grep)
        char* list_names = K_expand_kdb(kdb_var, (int) VARIABLES, (char*) pattern, '*');
        // Parses a string and replaces @filename and $listname by their contents
        char** objs = B_ainit_chk(list_names, NULL, 0);
        nb_names = SCR_tbl_size((unsigned char**) objs);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(PATTERN MATCHING)       found " << std::to_string(nb_names) 
                    << " variable names matching pattern '" << pattern << "' in " 
                    << elapsed.count() << " seconds" << std::endl;

        /* NOTE: TOO SLOW -> ~ 35 sec -> skipped
        start = std::chrono::high_resolution_clock::now();
        // Retrieves all object names from K_WS[...]
        list_names = K_expand_kdb(kdb_var, (int) VARIABLES, "*", '*');
        // Parses a string and replaces @filename and $listname by their contents
        char** all_objs = B_ainit_chk(list_names, NULL, 0);
        all_nb_names = kdb_var->size();
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(PATTERN MATCHING)       found " << std::to_string(all_nb_names) 
                    << " variable names matching pattern '" << pattern << "' in " 
                    << elapsed.count() << " seconds" << std::endl;
        */

        A_SEPS = OLD_SEPS;

        // ---- Build a char** table containing all variable names ----
        start = std::chrono::high_resolution_clock::now();
        all_nb_names = kdb_var->size();
        char** all_objs = new char*[all_nb_names];
        int i = 0;
        for(const auto& [name, _] : kdb_var->k_objs)
            all_objs[i++] = (char*) SCR_stracpy((unsigned char*) name.c_str());
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(BUILD VECTOR ALL NAMES) built a char** table of " << std::to_string(all_nb_names) 
                    << " variable names in " << elapsed.count() << " seconds" << std::endl;

        delete kdb_var;

        // ==== Load list of variables from big.var in GLOBAL and STANDALONE modes ====
        load_all = 1;

        // ---- GLOBAL ----
        db_global = 1;
        start = std::chrono::high_resolution_clock::now();
        kdb_var = K_load(VARIABLES, fullfilename, load_all, objs, db_global);
        EXPECT_TRUE(kdb_var != nullptr);
        EXPECT_EQ(kdb_var->size(), nb_names);
        EXPECT_EQ(kdb_var->sample->to_string(), "1990Y1:2060Y1");
        EXPECT_EQ(kdb_var->sample->nb_periods, 71);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(GLOBAL - SOME VARS)     loaded " << std::to_string(nb_names) 
                    << " variables in " << elapsed.count() << " seconds" << std::endl;  
        delete kdb_var; 

        // ---- STANDALONE ----
        db_global = 0;
        start = std::chrono::high_resolution_clock::now();
        kdb_var = K_load(VARIABLES, fullfilename, load_all, objs, db_global);
        EXPECT_TRUE(kdb_var != nullptr);
        EXPECT_EQ(kdb_var->size(), nb_names);
        EXPECT_EQ(kdb_var->sample->to_string(), "1990Y1:2060Y1");
        EXPECT_EQ(kdb_var->sample->nb_periods, 71);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(STANDALONE - SOME VARS) loaded " << std::to_string(nb_names) 
                    << " variables in " << elapsed.count() << " seconds" << std::endl; 
        delete kdb_var;

        // **** B_WsLoad() performance test ****
        // ==== Load all variables from big.var into K_WS[VARIABLES] ====
        start = std::chrono::high_resolution_clock::now();
        B_WsLoad(fullfilename, VARIABLES);
        EXPECT_TRUE(K_WS[VARIABLES] != nullptr);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(B_WsLoad)               loaded " << std::to_string(K_WS[VARIABLES]->size()) 
                    << " variables in " << elapsed.count() << " seconds" << std::endl;

        // **** K_refer and K_quick_refer performance tests ****
        // ==== Only names matching a specific pattern ====
        start = std::chrono::high_resolution_clock::now();
        kdb_shallow_copy = K_refer(K_WS[VARIABLES], nb_names, objs);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(K_refer)                created a shallow copy of " << std::to_string(nb_names) 
                    << " variables in " << elapsed.count() << " seconds" << std::endl;
        EXPECT_TRUE(kdb_shallow_copy != nullptr);
        EXPECT_EQ(kdb_shallow_copy->size(), nb_names);
        kdb_shallow_copy->clear(false);

        start = std::chrono::high_resolution_clock::now();
        kdb_shallow_copy = K_quick_refer(K_WS[VARIABLES], nb_names, objs);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(K_quick_refer)          created a shallow copy of " << std::to_string(nb_names) 
                    << " variables in " << elapsed.count() << " seconds" << std::endl;
        EXPECT_TRUE(kdb_shallow_copy != nullptr);
        EXPECT_EQ(kdb_shallow_copy->size(), nb_names);
        kdb_shallow_copy->clear(false);

        // ==== All names from K_WS[VARIABLES] ====
        start = std::chrono::high_resolution_clock::now();
        kdb_shallow_copy = K_refer(K_WS[VARIABLES], all_nb_names, all_objs);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(K_refer)                created a shallow copy of " << std::to_string(all_nb_names) 
        << " variables in " << elapsed.count() << " seconds" << std::endl;
        EXPECT_TRUE(kdb_shallow_copy != nullptr);
        EXPECT_EQ(kdb_shallow_copy->size(), all_nb_names);
        kdb_shallow_copy->clear(false);

        start = std::chrono::high_resolution_clock::now();
        kdb_shallow_copy = K_quick_refer(K_WS[VARIABLES], all_nb_names, all_objs);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(K_quick_refer)          created a shallow copy of " << std::to_string(all_nb_names) 
                    << " variables in " << elapsed.count() << " seconds" << std::endl;
        EXPECT_TRUE(kdb_shallow_copy != nullptr);
        EXPECT_EQ(kdb_shallow_copy->size(), all_nb_names);
        kdb_shallow_copy->clear(false);

        SCR_free_tbl((unsigned char**) objs);
        delete[] all_objs;

        // **** KDBVariables(filepath) performance tests ****
        start = std::chrono::high_resolution_clock::now();
        cpp_kdb_var = new KDBVariables(std::string(fullfilename));
        EXPECT_TRUE(cpp_kdb_var != nullptr);
        global_kdb = cpp_kdb_var->get_database();
        EXPECT_TRUE(global_kdb != nullptr);
        EXPECT_EQ(global_kdb->size(), 175760);
        EXPECT_EQ(global_kdb->sample->to_string(), "1990Y1:2060Y1");
        EXPECT_EQ(global_kdb->sample->nb_periods, 71);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "loaded " << std::to_string(cpp_kdb_var->size()) << " variables in " 
                    << elapsed.count() << " seconds" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        std::size_t hash_val = hash_value(Variables);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "hashed " << std::to_string(cpp_kdb_var->size()) << " variables in " 
                    << elapsed.count() << " seconds" << std::endl;
        delete cpp_kdb_var;

        // **** K_save() performance tests ****
        memset(out_filename, 0, sizeof(out_filename));
        sprintf(out_filename,  "%s%s", str_data_dir.c_str(), "big_save.var");
        start = std::chrono::high_resolution_clock::now();
        K_save(KV_WS, out_filename);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(K_save) [.var file]  saved " << std::to_string(K_WS[VARIABLES]->size()) 
                << " variables in " << elapsed.count() << " seconds" << std::endl;
    }
    else
    {
        std::cout << "File big.var not found. Skipping big files tests." << std::endl;
        std::cout << "To run these tests, run the script tests/generate_big_vars_ws.py" << std::endl;
    }
    
    // >>>>>>>>>>>> ASCII FILES <<<<<<<<<<<<
    memset(fullfilename, 0, sizeof(fullfilename));
    sprintf(fullfilename,  "%s%s", str_data_dir.c_str(), "big.av");

    if(std::filesystem::exists(fullfilename))
    {
        // **** B_WsLoad() performance test ****
        // ==== Load all variables from big.av into K_WS[VARIABLES] ====
        std::cout << "\n---- Testing performance of loading the ASCII file 'big.av' ----\n" << std::endl;

        auto start = std::chrono::high_resolution_clock::now();
        B_WsLoad(fullfilename, VARIABLES);
        auto end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(B_WsLoad) [.av file]      loaded " << std::to_string(K_WS[VARIABLES]->size()) 
                << " variables in " << elapsed.count() << " seconds" << std::endl;

        // **** KDBVariables(filepath) performance tests ****
        start = std::chrono::high_resolution_clock::now();
        cpp_kdb_var = new KDBVariables(std::string(fullfilename));
        EXPECT_TRUE(cpp_kdb_var != nullptr);
        global_kdb = cpp_kdb_var->get_database();
        EXPECT_TRUE(global_kdb != nullptr);
        EXPECT_EQ(global_kdb->size(), 175760);
        EXPECT_EQ(global_kdb->sample->to_string(), "1990Y1:2060Y1");
        EXPECT_EQ(global_kdb->sample->nb_periods, 71);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(B_WsLoad) [.av file]      loaded " << std::to_string(K_WS[VARIABLES]->size()) 
                    << " variables in " << elapsed.count() << " seconds" << std::endl;
        delete cpp_kdb_var;

        // **** K_save() performance tests ****
        memset(out_filename, 0, sizeof(out_filename));
        sprintf(out_filename,  "%s%s", str_data_dir.c_str(), "big_save.av");
        start = std::chrono::high_resolution_clock::now();
        K_save(KV_WS, out_filename);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "(K_save) [.av file]  saved " << std::to_string(K_WS[VARIABLES]->size()) 
                << " variables in " << elapsed.count() << " seconds" << std::endl;
    }
    else
    {
        std::cout << "File big.av not found. Skipping big files tests." << std::endl;
        std::cout << "To run these tests, run the script tests/generate_big_vars_ws.py" << std::endl;
    }
}
