from pathlib import Path
import argparse
import re

from typing import List, Union


test_function_pattern = re.compile(r"^void Test\w+\(.*\)")
utility_function_pattern = re.compile(r"^[\w\*]+\sU_\w+\(.*\)")

indent_pattern = re.compile(r"^ *")
macro_first_arg_pattern = re.compile(r"S4ASSERT\s?\(.*?(==|!=|>=|<=|>|<).*?,")
macro_call_utility_func = re.compile(r"S4ASSERT\s?\(U_\w+\(.*\),")


def get_test_func_signature(line: str) -> Union[None, str]:
    match = test_function_pattern.search(line)
    if (match):
        return match[0].strip()
    else:
        return None


def get_utility_func_signature(line: str) -> Union[None, str]:
    match = utility_function_pattern.search(line)
    if (match):
        return match[0].strip()
    else:
        return None


def convert_test_function_signature(borland_signature: str) -> str:
    assert(borland_signature)
    # skip qualifier
    func = borland_signature.split(" ")[1]
    # remove arguments (there are none)
    func_name = func.split("(")[0]
    return f"TEST_F(IodeCAPITest, {func_name})"


def convert_assert_to_google_test_macro(macro: str) -> str:
    match = indent_pattern.search(macro)
    indent = match[0]
    macro = macro.strip()

    new_macro = f"{indent}EXPECT_"

    match = macro_first_arg_pattern.search(macro)
    # first arg contains boolean operatior (==, !=, >=, <=, <, >
    if match:
        macro = match[0]
        # skip the ( and ,
        position_first_arg = macro.find("(") + 1
        condition = macro[position_first_arg:-1]
        if "==" in condition:
            left_arg, right_arg = condition.split("==")
            left_arg, right_arg = left_arg.strip(), right_arg.strip()
            # When comparing a pointer to NULL, use EXPECT_EQ(ptr, nullptr) instead of EXPECT_EQ(ptr, NULL)
            # See https://google.github.io/googletest/reference/assertions.html#EXPECT_EQ
            if right_arg == "NULL":
                right_arg = "nullptr"
            new_macro += f"EQ({left_arg}, {right_arg});"
        elif "!=" in condition:
            left_arg, right_arg = condition.split("!=")
            left_arg, right_arg = left_arg.strip(), right_arg.strip()
            if right_arg == "NULL":
                right_arg = "nullptr"
            new_macro += f"NE({left_arg}, {right_arg});"
        else:
            new_macro += f"TRUE({condition});"
    else:
        match = macro_call_utility_func.search(macro)
        # first arg contains call to utility func U_(...)
        if match:
            macro = match[0]
            # skip the ( and ,
            position_U_func = macro.find("(") + 1
            U_func_call = macro[position_U_func:-1]
            new_macro += f"TRUE({U_func_call});"

    if not new_macro.endswith(";"):
        raise ValueError(f"Could not extract first argument of macro {macro}")

    return new_macro


def generate_google_test_function(content: List[str]) -> List[str]:
    assert(get_test_func_signature(content[0]) is not None)
    assert(content[1] == "{")
    assert(content[-1] == "}")

    print(f"parsing test function {content[0]} ", end='...')
    new_content = [convert_test_function_signature(content[0])]

    new_content.append("{")
    # [2:-1] to skip signature function and bracelets
    for line in content[2:-1]:
        if line.strip().startswith("S4ASSERT"):
            line = convert_assert_to_google_test_macro(line)
        new_content.append(line)
        
    new_content.append("}")

    print("done")

    return new_content


def generate_utility_function(content: List[str]) -> List[str]:
    assert(get_utility_func_signature(content[0]) is not None)
    assert(content[1] == "{")
    assert(content[-1] == "}")

    indent = "\t"

    print(f"parsing utility function {content[0]} ", end='...')
    new_content = [indent + content[0]]

    new_content.append(indent + "{")
    # [2:-1] to skip signature function and bracelets
    for line in content[2:-1]:
        if line.strip().startswith("S4ASSERT"):
            line = convert_assert_to_google_test_macro(line)
        new_content.append(indent + line)
        
    new_content.append(indent + "}")

    print("done")

    return new_content


if __name__ == "__main__":
    prog = "BORLAND TO GOOGLE TEST CONVERTER"
    parser = argparse.ArgumentParser(prog=prog, description='Convert test functions written for Borland to Google Test syntax', 
                                     epilog="Don't forget to update the CMakeLists.txt file if you create a new Google Test source file!")

    parser.add_argument('input_filepath', type=str, nargs=1, help='filepath for the input source file')
    parser.add_argument('output_directory', type=str, nargs=1, help='filepath for the output source file')
    parser.add_argument('output_filename', type=str, nargs=1, help='filename for the output source file')

    args = parser.parse_args()

    print(f"======== {prog} ========")

    # check input file exists
    input_filepath_str = args.input_filepath[0]
    filepath_in = Path.cwd() / input_filepath_str if '/' not in input_filepath_str else Path(input_filepath_str)
    filepath_in_absolute = filepath_in.resolve()
    if not filepath_in.exists():
        raise ValueError(f"Input file {filepath_in_absolute} cannot be found")
    if not filepath_in.is_file():
        raise ValueError(f"{filepath_in_absolute} is not a file")
    print("Input source file: ", filepath_in.name)

    # check of output file exists
    output_directory_str = args.output_directory[0]
    # output_directory = Path.cwd() / output_directory_str if '/' not in output_directory_str else Path(output_directory_str)
    output_directory = Path.cwd() if '/' not in output_directory_str else Path('.')
    if not output_directory.is_dir():
        raise ValueError(f"Output file: Destination directory {output_directory.resolve()} does not exist")
    filepath_out = output_directory / args.output_filename[0]
    print("Output file: ", output_directory_str, "/", filepath_out.name)

    print(f"Loading input source file {filepath_in.name} ", end="...")
    file_in = open(filepath_in, 'r')
    lines = file_in.readlines()
    file_in.close()
    print("done")

    # definition of the SetUp method of the Fixture class
    test_class_lines = """
class IodeCAPITest : public ::testing::Test 
{
protected:
	char input_test_dir[256];
	char output_test_dir[256];

public:
    void SetUp() override 
    {
        // C++ consider all passed string value of the kind "..." to C function as CONSTANT
        char buf1[64];
        char buf2[64];

        // NOTE: we assume that: 
		//       - current path is binaryDir/tests/test_cpp_api
		//       - data directory has been copied in binaryDir/tests (see CMakeLists.txt in root directory)
		std::filesystem::path cwd = std::filesystem::current_path();
		std::string str_path = cwd.parent_path().string() + "\\\\";
		strcpy_s(input_test_dir, (str_path + "data\\\\").c_str());
		strcpy_s(output_test_dir, (str_path + "output\\\\").c_str());

        // other initializations
        U_test_init();
    }
""".split('\n')  

    # extra functions
    list_func_to_export_as_it = ["int W_printf(char* fmt,...)", "void kmsg_null(char*msg)"]
    func_to_export_lines = []

    # parse input source file
    keep = False
    function_lines = []
    function = []
    func_type = ""
    for line in lines:
        line = line.rstrip()
        # test if line = beginning of a test function
        test_func_signature = get_test_func_signature(line)
        utility_func_signature = get_utility_func_signature(line)
        if test_func_signature is not None:
            print("test function: ", line)
            func_type = "TEST"
            function = [line]
        # test if we are currenctly inside a test function
        elif utility_func_signature is not None:
            print("utility function: ", line)
            func_type = "UTIL"
            function = [line]
        elif line in list_func_to_export_as_it:
            print("function to export as it: ", line)
            func_type = "EXPORT"
            function = [line]
        elif len(function):
            '''if "IODE_DATA_DIR" in line:
                line = line.replace("IODE_DATA_DIR", "input_test_dir")
            if "IODE_OUTPUT_DIR" in line:
                line = line.replace("IODE_OUTPUT_DIR", "output_test_dir")
            '''    
            function.append(line)
            # test if line = end of a test function
            if line == "}":
                if func_type == "TEST":
                    function = generate_google_test_function(function)
                    function_lines += function + ["", ""]
                elif func_type == "UTIL":
                    function = generate_utility_function(function)
                    test_class_lines += function + [""]
                elif func_type == "EXPORT":
                    func_to_export_lines += function + [""]
                else:
                    raise RuntimeError(f"Unknown kind of function with signature: {function[0]}")
                function = []
        elif line.startswith("// BEGIN_KEEP"):
            keep = True
            func_to_export_lines += [line]
        elif line.startswith("// END_KEEP"):
            keep = False
            func_to_export_lines += [line, "\n"]
        elif keep:
            func_to_export_lines += [line]
        # assume we are outside a test function
        else:
            pass

    #global remark
    out_lines = ["// The present source file has been generated from the source", 
                 "// file test1.c (Google Test is not available with Borland).", 
                 "// DO NOT MODIFY IT!",
                 "// Modify test1.c instead and run convert_tests.bat/sh", 
                 "", ""]

    #include
    out_lines += ['#include "pch.h"', "#include <filesystem>", "", ""]

    # extra funcs
    out_lines += func_to_export_lines + [""]
    
    # fixture class
    test_class_lines += ["", "\t//void TearDown() override {}", "", "};"]
    out_lines += test_class_lines + ["", ""]

    # test functions
    out_lines += function_lines

    # dump output
    print(f"Writing output source file {filepath_out.name} ", end="...")
    file_out = open(filepath_out, 'w+', encoding='utf_8')
    file_out.writelines('\n'.join(out_lines) + '\n')
    file_out.close()
    print("done")

    print("\nfinished: succes")
