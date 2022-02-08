#include "pch.h"


class KDBUtilsTest : public KDBTest, public ::testing::Test
{
protected:

	void SetUp() override {}

	// void TearDown() override {}
};


TEST(TestUtils, Utf8_oem_convertion)
{
	std::string str_utf8 = u8"aA!@^$jndq256ιοµηθ";

	std::string str_oem = convert_utf8_to_oem(str_utf8);
	std::string str_res = convert_oem_to_utf8(str_oem);

	EXPECT_EQ(str_res, str_utf8);
}


TEST_F(KDBUtilsTest, checkFilepath)
{
	std::string filepath;

	// fake directory
	filepath = "C:\\Users\\ald\\wrong\\path\\fun.cmt";
	EXPECT_THROW(check_filepath(filepath, I_COMMENTS, "save", false), std::runtime_error);

	// wrong extension
	filepath = output_test_dir + "fun.eqs";
	EXPECT_THROW(check_filepath(filepath, I_COMMENTS, "save", false), std::runtime_error);

	// file does not exist
	filepath = input_test_dir + "fun2.cmt";
	EXPECT_THROW(check_filepath(filepath, I_COMMENTS, "load", true), std::runtime_error);

	// file does not exist (no extension given)
	filepath = input_test_dir + "fun2";
	EXPECT_THROW(check_filepath(filepath, I_COMMENTS, "load", true), std::runtime_error);

	// extension added automatically
	filepath = input_test_dir + "fun";
	filepath = check_filepath(filepath, I_COMMENTS, "load", true);
	EXPECT_EQ(filepath, input_test_dir + "fun.cmt");
}
