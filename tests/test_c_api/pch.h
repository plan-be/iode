//
// pch.h
//

#pragma once

#include "gtest/gtest.h"
#include "api/all.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <string>
#include <vector>
#include <set>
#include <algorithm> 	// for std::min, std::max


inline bool lec_contains(const std::shared_ptr<CLEC> clec, const std::string& name)
{
    bool found = false;
    for(const std::string& obj_name : clec->v_obj_names)
    {
        if(obj_name == name)
        {
            found = true;
            break;
        }
    }
    return found;
} 


class TestAbstract
{
protected:
	std::string prefix_filename;
    std::string str_separator;
	std::string str_tests_dir;
	std::string str_input_test_dir;
	std::string str_output_test_dir;
	std::string str_report_test_dir;

public:
	TestAbstract()
	{
		// set *_super fn pointers
		IODE_assign_super_API();

		KPAUSE_CONTINUE = 1;
		// suppress messages from kmsg()
		skip_message(true);
		
		// NOTE: we assume that: 
		//       - current path is binaryDir/tests/test_c_api
		//       - data directory has been copied in binaryDir/tests (see CMakeLists.txt in root directory)
		std::string current_file = __FILE__;
		std::filesystem::path cwd(current_file);
		std::filesystem::path tests_dir = cwd.parent_path().parent_path();
        std::filesystem::path data_dir = tests_dir / "data";
        std::filesystem::path output_dir = data_dir / "output";
        std::filesystem::path report_dir = data_dir / "reports";
#ifdef __GNUC__
		prefix_filename = "linux_";
		str_separator = "/";
#else
		prefix_filename = "";
        str_separator = "\\";
#endif
		str_tests_dir	    = tests_dir.string() + str_separator;
		str_input_test_dir  = data_dir.string() + str_separator;
		str_output_test_dir = output_dir.string() + str_separator;
		str_report_test_dir = report_dir.string() + str_separator;

		// clear global workspaces
		global_ws_cmt->clear();
		global_ws_eqs->clear();
		global_ws_idt->clear();
		global_ws_lst->clear();
		global_ws_scl->clear();
		global_ws_tbl->clear();
		global_ws_var->clear();
	}

protected:
	void print_test_title(const std::string& title)
	{
		std::cout << "\n\n" << title << std::endl;
		std::cout << std::string(title.size(), '-') << std::endl;
	}

	void create_dummy_lists_and_vars()
	{
		// Create or update lists
		std::string lst;
        KDBListsPtr kdb_lst = global_ws_lst;

        if(kdb_lst->contains("LST1"))
	        kdb_lst->update("LST1", "A,B");
        else
            kdb_lst->add("LST1", "A,B");
	    lst = kdb_lst->get("LST1");
        EXPECT_EQ(lst, "A,B");

        if(kdb_lst->contains("LST2"))
            kdb_lst->update("LST2", "A,B,A");
        else
	        kdb_lst->add("LST2", "A,B,A");
        lst = kdb_lst->get("LST2");
        EXPECT_EQ(lst, "A,B,A");

	    // Set the sample for the variable WS
		KDBVariablesPtr kdb_var = global_ws_var;
	    std::shared_ptr<Sample> smpl_ptr = std::make_shared<Sample>("2000Y1", "2020Y1");
	    kdb_var->set_sample(*smpl_ptr);
	    EXPECT_TRUE(kdb_var->get_sample() != nullptr);
	
	    // Creates or update new vars
        Variable A;
        Variable B;
	    int nb = smpl_ptr->nb_periods;
	    for(int i = 0; i < nb; i++) 
        {
	       A.push_back(i);
	       B.push_back(i*2);
	    }
	
        if(kdb_var->contains("A"))
            kdb_var->update("A", A);
        else
	        kdb_var->add("A", A);
        double* values = kdb_var->get_var_ptr("A");
        EXPECT_NE(values, nullptr);
        EXPECT_DOUBLE_EQ(kdb_var->get_value("A", 0), A[0]);
        EXPECT_DOUBLE_EQ(kdb_var->get_value("A", nb-1), A[nb-1]);
	    
        if(kdb_var->contains("B"))
            kdb_var->update("B", B);
        else
            kdb_var->add("B", B);
        values = kdb_var->get_var_ptr("B");
        EXPECT_NE(values, nullptr);
        EXPECT_DOUBLE_EQ(kdb_var->get_value("B", 0), B[0]);
        EXPECT_DOUBLE_EQ(kdb_var->get_value("B", nb-1), B[nb-1]);

	    // For B_DataPattern()
        if(kdb_lst->contains("AB"))
            kdb_lst->update("AB", "A,B");
        else
	        kdb_lst->add("AB", "A,B");

        if(kdb_lst->contains("BC"))
            kdb_lst->update("BC", "B,C");
        else
	        kdb_lst->add("BC", "B,C");

        if(kdb_var->contains("AB"))
	        kdb_var->update("AB", B);
        else
	        kdb_var->add("AB", B);

        if(kdb_var->contains("AC"))
	        kdb_var->update("AC", B);
        else
	        kdb_var->add("AC", B);
        
        if(kdb_var->contains("BB"))
            kdb_var->update("BB", B);
        else
            kdb_var->add("BB", B);
        
        if(kdb_var->contains("BC"))
            kdb_var->update("BC", B);
        else
            kdb_var->add("BC", B);
	}

	double calculate_lec(const std::string& lec, int t)
	{
        // make sure that the 'lec' expression is valid
	    std::shared_ptr<CLEC> clec = nullptr; 
        try
        {
            clec = std::make_shared<CLEC>(lec);
        }
        catch(const std::exception&)
        {
            return IODE_NAN;
        }
        
	    if(clec->link(global_ws_var, global_ws_scl) != 0) 
            return IODE_NAN;
        
	    double res = clec->execute(global_ws_var, global_ws_scl, t);
	    return res;
	}

	void check_lec(const std::string& lec, int t, double expected_val)
	{
		double precision = 1e6;

        // make sure that 't' is valid
        Period per = global_ws_var->get_sample()->start_period.shift(t);
        
        // make sure that the LEC expression is valid
	    std::shared_ptr<CLEC> clec = std::make_shared<CLEC>(lec);

		std::cout << "linking LEC expression '" << lec << "'" << std::endl;
	    int rc = clec->link(global_ws_var, global_ws_scl);
	    EXPECT_EQ(rc, 0);

		std::cout << "computing LEC expression for the period '" << per.to_string() << "'" << std::endl;
	    double calc_val = clec->execute(global_ws_var, global_ws_scl, t);
	    EXPECT_DOUBLE_EQ(round(expected_val * 1e6) / 1e6, round(calc_val * 1e6) / 1e6);
	}

	void compare_files(const std::string& filepath1, const std::string& filepath2, 
		std::set<int> ignore_lines = std::set<int>())
	{	
		std::string error_msg;

		// check if files exist
		if(!std::filesystem::exists(filepath1))
			throw std::runtime_error("The file " + filepath1 + " does not exist");

		if(!std::filesystem::exists(filepath2))
			throw std::runtime_error("The file " + filepath2 + " does not exist");

		// open the files
	    std::ifstream file1(filepath1);
		if(!file1.is_open())
			throw std::runtime_error("Could not open the file " + filepath1);

	    std::ifstream file2(filepath2);
		if(!file2.is_open())
			throw std::runtime_error("Could not open the file " + filepath2);

		std::cout << "\nComparing files:\n - " << filepath1 << "\n - " << filepath2 << std::endl;

		// count number of lines
		std::string line;
		int nb_lines_file1 = 0;
		while (std::getline(file1, line))
			nb_lines_file1++;

		std::cout << "File " << filepath1 << " has " << nb_lines_file1 << " lines" <<  std::endl;
		
		int nb_lines_file2 = 0;
		while (std::getline(file2, line))
			nb_lines_file2++;

		std::cout << "File " << filepath2 << " has " << nb_lines_file2 << " lines" <<  std::endl;
	
		std::ifstream longest_file;
		std::ifstream smallest_file;

		std::string filepath_longest_file;
		std::string filepath_smallest_file;

		std::string filename_longest_file;
		std::string filename_smallest_file;

		if(nb_lines_file1 >= nb_lines_file2)
		{
			longest_file.open(filepath1);
			filepath_longest_file = filepath1;
			std::filesystem::path p(filepath1);
			filename_longest_file = p.filename().string();
			
			smallest_file.open(filepath2);
			filepath_smallest_file = filepath2;
			std::filesystem::path p2(filepath2);
			filename_smallest_file = p2.filename().string();
		}
		else
		{
			longest_file.open(filepath2);
			filepath_longest_file = filepath2;
			std::filesystem::path p(filepath2);
			filename_longest_file = p.filename().string();

			smallest_file.open(filepath1);
			filepath_smallest_file = filepath1;
			std::filesystem::path p2(filepath1);
			filename_smallest_file = p2.filename().string();
		}

		// compare line by line
		int pos = 1;
		bool is_different = false;
		std::string line2;
		error_msg = "";
		while (std::getline(smallest_file, line))
		{
			std::getline(longest_file, line2);

			if(ignore_lines.contains(pos))
			{
				ignore_lines.erase(pos);
				std::cout << "Ignoring line (" << pos << "):" << std::endl;
				std::cout << "file 1: " << line << std::endl;
				std::cout << "file 2: " << line2 << std::endl;
				std::cout << std::endl;
				pos++;
				continue;
			}

			if(line != line2)
			{
				is_different = true;
				size_t min_len = std::min(line.size(), line2.size());

				std::string diff_indicator;
				std::string diff_indicator2;
				std::vector<std::string> diff_positions;
				for (size_t i = 0; i < min_len; ++i)
				{
					diff_indicator += (line[i] != line2[i]) ? "^" : " ";
					if(line[i] != line2[i])
						diff_positions.push_back(std::to_string(i+1));
				}
				diff_indicator2 = diff_indicator;

				if(line.size() > min_len)
				{
					diff_indicator += std::string(line.size() - min_len, '^');
					diff_positions.push_back(">" + std::to_string(min_len));
				}
				else if(line2.size() > min_len)
				{
					diff_indicator2 += std::string(line2.size() - min_len, '^');
					diff_positions.push_back(">" + std::to_string(min_len));
				}

				error_msg += "Line at position " + std::to_string(pos) + " are different:\n";
				
				std::string prefix  = "file " + filename_smallest_file + " (" + std::to_string(line.size()) + ")";		
				std::string prefix2 = "file " + filename_longest_file + " (" + std::to_string(line2.size()) + ")";
				if(prefix.size() < prefix2.size())
					prefix += std::string(prefix2.size() - prefix.size(), ' ');
				else if(prefix2.size() < prefix.size())
					prefix2 += std::string(prefix.size() - prefix2.size(), ' ');

				error_msg += prefix + " -> " + line + "\n";
				error_msg += std::string(prefix.size() + 4, ' ') + diff_indicator + "\n";
				error_msg += prefix2 + " -> " + line2 + "\n";
				error_msg += std::string(prefix2.size() + 4, ' ') + diff_indicator2 + "\n";
				error_msg += "Different character at positions: " + join(diff_positions, ", ") + "\n";
				
				error_msg += "\n";
			} 
			pos++;
		}

		if(is_different)
			FAIL() << error_msg;

		std::filesystem::path p(filepath1);
		std::string filename1 = p.filename().string();
		std::filesystem::path p2(filepath2);
		std::string filename2 = p2.filename().string();

		error_msg = "The file " + filename1 + " has " + std::to_string(nb_lines_file1) + " lines ";
		error_msg += "while the file " + filename2 + " has " + std::to_string(nb_lines_file2) + " lines";
		EXPECT_EQ(nb_lines_file1, nb_lines_file2) << error_msg;
	}

    void compare_files(char* filepath1, char* filepath2, std::set<int> ignore_lines = std::set<int>())
    {
        compare_files(std::string(filepath1), std::string(filepath2), ignore_lines);
    }

    void compare_files(char* dir1, char* filepath1, char* dir2, char* filepath2, 
		std::set<int> ignore_lines = std::set<int>())
    {
        std::string fullpath1 = std::string(dir1) + std::string(filepath1);
        std::string fullpath2 = std::string(dir2) + std::string(filepath2);
        compare_files(fullpath1, fullpath2, ignore_lines);
    }
};
