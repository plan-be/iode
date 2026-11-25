#include "pch.h"


class UtilsTest : public TestAbstract, public ::testing::Test
{
protected:

	void SetUp() override {}

	// void TearDown() override {}
};


TEST_F(UtilsTest, convertBetweenCodepages)
{
	std::string str_utf8 = "aA!@^$jndq256µ&ï";

	std::string str_oem = convert_between_codepages(str_utf8, false);
	std::string str_res = convert_between_codepages(str_oem, true);

	EXPECT_EQ(str_res, str_utf8);
}


TEST_F(UtilsTest, replaceLineBreak)
{
    std::string text = "unchanged";
    W_replace_line_break(text);
    EXPECT_EQ(text, "unchanged");

    text = "first\nsecond\r\nthird\rfourth\n\nlast";
    W_replace_line_break(text);
    EXPECT_EQ(text, "first\\psecond\\pthird\\pfourth\\p\\plast");
}

TEST_F(UtilsTest, splitMultiQuoted)
{
    EXPECT_EQ(split_multi_quoted("", ",", '"'),
              std::vector<std::string>({}));
    EXPECT_EQ(split_multi_quoted(",,,", ",", '"'),
              std::vector<std::string>({}));

    EXPECT_EQ(split_multi_quoted("abc,def,ghi", ",", '"'),
              std::vector<std::string>({"abc", "def", "ghi"}));
    EXPECT_EQ(split_multi_quoted("  abc,,def;\tghi", " ,;\t", '"'),
              std::vector<std::string>({"abc", "def", "ghi"}));

    EXPECT_EQ(split_multi_quoted("abc,def,\"hij,klm\",z", ",", '"'),
              std::vector<std::string>({"abc", "def", "hij,klm", "z"}));
    EXPECT_EQ(split_multi_quoted("abc,\"\",def", ",", '"'),
              std::vector<std::string>({"abc", "", "def"}));
    EXPECT_EQ(split_multi_quoted("abc,\"def,ghi", ",", '"'),
              std::vector<std::string>({"abc", "def,ghi"}));
}

TEST_F(UtilsTest, splitSkipEmpty)
{
    EXPECT_EQ(split_skip_empty("", ","), std::vector<std::string>({}));
    EXPECT_EQ(split_skip_empty(",,,", ","), std::vector<std::string>({}));

    // single separator (= SCR_vtom)
    EXPECT_EQ(split_skip_empty("abc", ","), std::vector<std::string>({"abc"}));
    EXPECT_EQ(split_skip_empty("abc,def,ghi", ","),
              std::vector<std::string>({"abc", "def", "ghi"}));
    EXPECT_EQ(split_skip_empty(",abc,,def,", ","),
              std::vector<std::string>({"abc", "def"}));

    // several separators (= SCR_vtoms)
    EXPECT_EQ(split_skip_empty("Jean    Marc;,,Pierre", " ,;"),
              std::vector<std::string>({"Jean", "Marc", "Pierre"}));
    EXPECT_EQ(split_skip_empty("line1\n\nline2\n", "\n"),
              std::vector<std::string>({"line1", "line2"}));

    // no separator
    EXPECT_EQ(split_skip_empty("a b", ""), std::vector<std::string>({"a b"}));
}

TEST_F(UtilsTest, textToLines)
{
    // example of SCR_text() (the lines keep their trailing separator, even a space)
    EXPECT_EQ(text_to_lines("Jean, Marc, Pierre, Louis, Marie-Christine-Petit", ", ;", 15),
              std::vector<std::string>({"Jean, Marc, ", "Pierre, Louis, ", "Marie-Christine", "-Petit"}));

    // empty text, only white spaces, lg <= 0
    EXPECT_EQ(text_to_lines("", ",", 10), std::vector<std::string>({}));
    EXPECT_EQ(text_to_lines(" \t\n ", ",", 10), std::vector<std::string>({}));
    EXPECT_EQ(text_to_lines("abc", ",", 0), std::vector<std::string>({}));

    // short text: one line, leading spaces skipped, trailing spaces kept
    EXPECT_EQ(text_to_lines("  abc  ", ",", 10), std::vector<std::string>({"abc  "}));

    // '\n' ends a line
    EXPECT_EQ(text_to_lines("ab\ncd\n\nef", ",", 10), std::vector<std::string>({"ab", "cd", "ef"}));

    // no separator: words are cut after lg chars
    EXPECT_EQ(text_to_lines("abcdefgh", ",", 3), std::vector<std::string>({"abc", "def", "gh"}));

    // separator at index 0 is ignored
    EXPECT_EQ(text_to_lines(",abcdef", ",", 3), std::vector<std::string>({",ab", "cde", "f"}));

    // K_wrap() separators
    EXPECT_EQ(text_to_lines("A+B*ln(C)-D", ";,*+-/) ", 5),
              std::vector<std::string>({"A+B*", "ln(C)", "-D"}));
}

TEST_F(UtilsTest, longToDate)
{
    // default format
    EXPECT_EQ(long_to_date(19901231), "31/12/1990");

    // upper and lower case letters, any order
    EXPECT_EQ(long_to_date(19901231, "dd/mm/yy"), "31/12/90");
    EXPECT_EQ(long_to_date(19901231, "mm-dd-yy"), "12-31-90");
    EXPECT_EQ(long_to_date(19901231, "YYYY-MM-DD"), "1990-12-31");
    EXPECT_EQ(long_to_date(20260105, "yyyymmdd"), "20260105");

    // n Y's => last n digits of the year
    EXPECT_EQ(long_to_date(19901231, "Y"), "0");
    EXPECT_EQ(long_to_date(19901231, "YYY"), "990");

    // extra letters and other characters are kept as is
    EXPECT_EQ(long_to_date(19901231, "YYYYY"), "1990Y");
    EXPECT_EQ(long_to_date(19901231, "DDD MMM"), "31D 12M");
    EXPECT_EQ(long_to_date(19901231, "on dd.mm"), "on 31.12");
    EXPECT_EQ(long_to_date(19901231, ""), "");

    // WARNING: every Y, M or D of fmt is a date letter, even inside a word
    EXPECT_EQ(long_to_date(19901231, "Date: dd.mm"), "3ate: 1d.12");

    // year on less than 4 digits
    EXPECT_EQ(long_to_date(50501, "DD/MM/YYYY"), "01/05/0005");

    // zero or negative date
    EXPECT_THROW(long_to_date(0L), std::invalid_argument);
    EXPECT_THROW(long_to_date(-19901231L, "dd/mm/yy"), std::invalid_argument);

    // smallest date: year 0, month 0, day 1
    EXPECT_EQ(long_to_date(1L), "01/00/0000");
    EXPECT_EQ(long_to_date(99991231L, "YYYY-MM-DD"), "9999-12-31");

    // year on 5 digits (12345): the Y's take the digits from the start of the year
    // (as SCR_long_to_fdate() which assumes a year on 4 digits)
    EXPECT_EQ(long_to_date(123456789L, "DD/MM/YYYY"), "89/67/1234");
    EXPECT_EQ(long_to_date(123456789L, "dd/mm/yy"), "89/67/34");

    // no date letter
    EXPECT_EQ(long_to_date(19901231L, "abc"), "abc");
}

TEST_F(UtilsTest, replaceAll)
{
    std::string str = "AAAA";
    EXPECT_EQ(replace_all(str, "AA", "B"), 2);
    EXPECT_EQ(str, "BB");

    // no rescan of the inserted text
    str = "AAAA";
    EXPECT_EQ(replace_all(str, "AA", "A"), 2);
    EXPECT_EQ(str, "AA");

    str = "a_b_c";
    EXPECT_EQ(replace_all(str, "_", "__"), 2);
    EXPECT_EQ(str, "a__b__c");

    str = "x\ty\tz";
    EXPECT_EQ(replace_all(str, "\t", " "), 2);
    EXPECT_EQ(str, "x y z");

    str = "unchanged";
    EXPECT_EQ(replace_all(str, "zz", "y"), 0);
    EXPECT_EQ(str, "unchanged");

    // empty pattern: nothing is done
    EXPECT_EQ(replace_all(str, "", "y"), 0);
    EXPECT_EQ(str, "unchanged");

    // empty replacement: removes the pattern
    str = "a, b, c";
    EXPECT_EQ(replace_all(str, ", ", ""), 2);
    EXPECT_EQ(str, "abc");
}


TEST_F(UtilsTest, getIodeFileType)
{
	std::string filename = "";
	EXPECT_EQ(get_iode_file_type(filename), FILE_ANY);

	filename = "ws";
	EXPECT_EQ(get_iode_file_type(filename), FILE_ANY);

	EXPECT_EQ(get_iode_file_type(str_input_test_dir), DIRECTORY);

	filename = str_input_test_dir + prefix_filename + "fun.cmt";
	EXPECT_EQ(get_iode_file_type(filename), FILE_COMMENTS);
	filename = str_input_test_dir + "fun.ac";
	EXPECT_EQ(get_iode_file_type(filename), FILE_COMMENTS);

	filename = str_input_test_dir + prefix_filename + "fun.eqs";
	EXPECT_EQ(get_iode_file_type(filename), FILE_EQUATIONS);
	filename = str_input_test_dir + "fun.ae";
	EXPECT_EQ(get_iode_file_type(filename), FILE_EQUATIONS);

	filename = str_input_test_dir + prefix_filename + "fun.idt";
	EXPECT_EQ(get_iode_file_type(filename), FILE_IDENTITIES);
	filename = str_input_test_dir + "fun.ai";
	EXPECT_EQ(get_iode_file_type(filename), FILE_IDENTITIES);

	filename = str_input_test_dir + prefix_filename + "fun.lst";
	EXPECT_EQ(get_iode_file_type(filename), FILE_LISTS);
	filename = str_input_test_dir + "fun.al";
	EXPECT_EQ(get_iode_file_type(filename), FILE_LISTS);

	filename = str_input_test_dir + prefix_filename + "fun.scl";
	EXPECT_EQ(get_iode_file_type(filename), FILE_SCALARS);
	filename = str_input_test_dir + "fun.as";
	EXPECT_EQ(get_iode_file_type(filename), FILE_SCALARS);

	filename = str_input_test_dir + prefix_filename + "fun.tbl";
	EXPECT_EQ(get_iode_file_type(filename), FILE_TABLES);
	filename = str_input_test_dir + "fun.at";
	EXPECT_EQ(get_iode_file_type(filename), FILE_TABLES);

	filename = str_input_test_dir + prefix_filename + "fun.var";
	EXPECT_EQ(get_iode_file_type(filename), FILE_VARIABLES);
	filename = str_input_test_dir + "fun.av";
	EXPECT_EQ(get_iode_file_type(filename), FILE_VARIABLES);

	filename = str_input_test_dir + "fun.rep";
	EXPECT_EQ(get_iode_file_type(filename), FILE_REP);

	filename = str_input_test_dir + "fun.log";
	EXPECT_EQ(get_iode_file_type(filename), FILE_LOG);

	filename = str_input_test_dir + "fun.ini";
	EXPECT_EQ(get_iode_file_type(filename), FILE_SETTINGS);

	filename = str_input_test_dir + "fun.txt";
	EXPECT_EQ(get_iode_file_type(filename), FILE_TXT);

	filename = str_input_test_dir + "fun.a2m";
	EXPECT_EQ(get_iode_file_type(filename), FILE_A2M);

	filename = str_input_test_dir + "fun.agl";
	EXPECT_EQ(get_iode_file_type(filename), FILE_AGL);

	filename = str_input_test_dir + "fun.prf";
	EXPECT_EQ(get_iode_file_type(filename), FILE_PRF);

	filename = str_input_test_dir + "fun.dif";
	EXPECT_EQ(get_iode_file_type(filename), FILE_DIF);

	filename = str_input_test_dir + "fun.mif";
	EXPECT_EQ(get_iode_file_type(filename), FILE_MIF);

	filename = str_input_test_dir + "fun.rtf";
	EXPECT_EQ(get_iode_file_type(filename), FILE_RTF);

	filename = str_input_test_dir + "fun.asc";
	EXPECT_EQ(get_iode_file_type(filename), FILE_AAS);

	filename = str_input_test_dir + "fun.ref";
	EXPECT_EQ(get_iode_file_type(filename), FILE_REF);
}


TEST_F(UtilsTest, checkFilepath)
{
	std::string filepath;

	// fake directory
	std::filesystem::path cwd = std::filesystem::current_path();
    std::filesystem::path fake_dir = cwd.parent_path() / "fake_dir";
	filepath = (fake_dir / "fun.cmt").string();
	EXPECT_THROW(check_filepath(filepath, FILE_COMMENTS, "save", false), std::invalid_argument);

	// wrong extension
	filepath = str_output_test_dir + "fun.eqs";
	EXPECT_THROW(check_filepath(filepath, FILE_COMMENTS, "save", false), std::invalid_argument);

	filepath = str_output_test_dir + "fun.docx";
	EXPECT_THROW(check_filepath(filepath, FILE_TXT, "save", false), std::invalid_argument);

	// file does not exist
	filepath = str_input_test_dir + "funxxx.cmt";
	EXPECT_THROW(check_filepath(filepath, FILE_COMMENTS, "load", true), std::invalid_argument);

	// file does not exist (no extension given)
	filepath = str_input_test_dir + "funxxx";
	EXPECT_THROW(check_filepath(filepath, FILE_COMMENTS, "load", true), std::invalid_argument);

	// No extension but not an IODE objects file
	filepath = str_input_test_dir + "fun";
	EXPECT_THROW(check_filepath(filepath, FILE_TXT, "load", true), std::invalid_argument);

	// extension added automatically
	filepath = str_input_test_dir + prefix_filename + "fun";
	filepath = check_filepath(filepath, FILE_COMMENTS, "load", true);
	EXPECT_EQ(filepath, str_input_test_dir + prefix_filename + "fun.cmt");
}
