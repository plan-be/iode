//
// pch.h - precompiled header file
//

#pragma once

#include "gtest/gtest.h"
#include "cpp_api/iode_cpp_api.h"

#include <cmath>
#include <cstdlib>		// for system()

#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm> 	// for std::min, std::max


class KDBTest
{
protected:
	std::string prefix_filename;
	std::string str_tests_dir;
	std::string input_test_dir;
	std::string output_test_dir;
	std::string report_test_dir;

public:
	KDBTest()
	{
		cpp_iode_init();
		kpause_continue = 1;
		// suppress messages from kmsg()
		kmsg_toggle(0);

		// clear global workspaces
		global_ws_cmt->clear();
		global_ws_eqs->clear();
		global_ws_idt->clear();
		global_ws_lst->clear();
		global_ws_scl->clear();
		global_ws_tbl->clear();
		global_ws_var->clear();

		// NOTE: we assume that: 
		//       - current path is binaryDir/tests/test_cpp_api
		//       - data directory has been copied in binaryDir/tests (see CMakeLists.txt in root directory)
		std::string current_file = __FILE__;
		std::filesystem::path cwd(current_file);
		std::filesystem::path tests_dir = cwd.parent_path().parent_path();
        std::filesystem::path data_dir = tests_dir / "data";
        std::filesystem::path output_dir = data_dir / "output";
        std::filesystem::path report_dir = data_dir / "reports";
#ifdef __GNUC__
		prefix_filename = "linux_";
		std::string separator = "/";
#else
		prefix_filename = "";
        std::string separator = "\\";
#endif
		str_tests_dir   = tests_dir.string() + separator;
		input_test_dir  = data_dir.string() + separator;
		output_test_dir = output_dir.string() + separator;
		report_test_dir = report_dir.string() + separator;
	}

protected:
	void compare_files(const std::string& filepath1, const std::string& filepath2)
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

		// count number of lines
		std::string line;
		int nb_lines_file1 = 0;
		while (std::getline(file1, line))
			nb_lines_file1++;
		
		int nb_lines_file2 = 0;
		while (std::getline(file2, line))
			nb_lines_file2++;

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
};


std::vector<std::string> char_array_to_std_string_vector(char** char_array);
