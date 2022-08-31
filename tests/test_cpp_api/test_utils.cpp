#include "pch.h"


class UtilsTest : public KDBTest, public ::testing::Test
{
protected:

	void SetUp() override {}

	// void TearDown() override {}
};


TEST_F(UtilsTest, convertBetweenCodepages)
{
	std::string str_utf8 = u8"aA!@^$jndq256����";

	std::string str_oem = convert_between_codepages(str_utf8, CP_UTF8, CP_OEMCP);
	std::string str_res = convert_between_codepages(str_oem, CP_OEMCP, CP_UTF8);

	EXPECT_EQ(str_res, str_utf8);
}


TEST_F(UtilsTest, checkFilepath)
{
	std::string filepath;

	// fake directory
	filepath = "C:\\Users\\ald\\wrong\\path\\fun.cmt";
	EXPECT_THROW(check_filepath(filepath, I_COMMENTS, "save", false), IodeExceptionFunction);

	// wrong extension
	filepath = output_test_dir + "fun.eqs";
	EXPECT_THROW(check_filepath(filepath, I_COMMENTS, "save", false), IodeExceptionFunction);

	// file does not exist
	filepath = input_test_dir + "fun2.cmt";
	EXPECT_THROW(check_filepath(filepath, I_COMMENTS, "load", true), IodeExceptionFunction);

	// file does not exist (no extension given)
	filepath = input_test_dir + "fun2";
	EXPECT_THROW(check_filepath(filepath, I_COMMENTS, "load", true), IodeExceptionFunction);

	// extension added automatically
	filepath = input_test_dir + "fun";
	filepath = check_filepath(filepath, I_COMMENTS, "load", true);
	EXPECT_EQ(filepath, input_test_dir + "fun.cmt");
}
