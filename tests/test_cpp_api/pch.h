//
// pch.h - precompiled header file
//

#pragma once

#include "gtest/gtest.h"
#include "cpp_api/iode_cpp_api.h"

#include <cmath>
#include <fstream>
#include <stdexcept>
#include <filesystem>

class KDBTest
{
protected:
	std::string prefix_filename;
	std::string input_test_dir;
	std::string output_test_dir;
	std::string report_test_dir;

public:
	KDBTest()
	{
		cpp_iode_init();
		kpause_continue = 1;

		// NOTE: we assume that: 
		//       - current path is binaryDir/tests/test_cpp_api
		//       - data directory has been copied in binaryDir/tests (see CMakeLists.txt in root directory)
		std::filesystem::path cwd = std::filesystem::current_path();
        std::filesystem::path data_dir = cwd.parent_path() / "data";
        std::filesystem::path output_dir = data_dir / "output";
        std::filesystem::path report_dir = data_dir / "reports";
#ifdef __GNUC__
		prefix_filename = "linux_";
		std::string separator = "/";
#else
		prefix_filename = "";
        std::string separator = "\\";
#endif
		input_test_dir = data_dir.string() + separator;
		output_test_dir = output_dir.string() + separator;
		report_test_dir = report_dir.string() + separator;
	}

protected:
	void compare_files(const std::string& filepath1, const std::string& filepath2)
	{	
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

		if(nb_lines_file1 >= nb_lines_file2)
		{
			longest_file.open(filepath1);
			filepath_longest_file = filepath1;

			smallest_file.open(filepath2);
			filepath_smallest_file = filepath2;
		}
		else
		{
			longest_file.open(filepath2);
			filepath_longest_file = filepath2;

			smallest_file.open(filepath1);
			filepath_smallest_file = filepath1;
		}

		// compare line by line
		int pos = 0;
		std::string line2;
		std::string error_msg;
		while (std::getline(smallest_file, line))
		{
			std::getline(longest_file, line2);
			if(line != line2)
			{
				error_msg = "Line at position " + std::to_string(pos) + " are different:\n";
				error_msg += "file " + filepath_smallest_file + " -> " + line;
				error_msg += "file " + filepath_longest_file + " -> " + line2;
				FAIL() << error_msg;
			} 
			pos++;
		}

		error_msg = "The file " + filepath1 + " has " + std::to_string(nb_lines_file1) + " lines";
		error_msg += "while the file " + filepath2 + " has " + std::to_string(nb_lines_file2) + " lines";
		EXPECT_EQ(nb_lines_file1, nb_lines_file2) << error_msg;
	}
};


std::vector<std::string> char_array_to_std_string_vector(char** char_array);
