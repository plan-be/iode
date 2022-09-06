//
// pch.h - precompiled header file
//

#pragma once

#include "gtest/gtest.h"
#include "cpp_api/iode_cpp_api.h"

#include <filesystem>

class KDBTest
{
protected:
	std::string input_test_dir;
	std::string output_test_dir;

public:
	KDBTest()
	{
		IodeInit();
		cpp_assign_super_API();

		// NOTE: we assume that: 
		//       - current path is binaryDir/tests/test_cpp_api
		//       - data directory has been copied in binaryDir/tests (see CMakeLists.txt in root directory)
		std::filesystem::path cwd = std::filesystem::current_path();
		std::string str_path = cwd.parent_path().string() + "\\";
		input_test_dir = str_path + "data\\";
		output_test_dir = str_path + "output\\";
	}
};


std::vector<std::string> char_array_to_std_string_vector(char** char_array);
