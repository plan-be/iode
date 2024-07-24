#include "pch.h"


class UtilsTest : public KDBTest, public ::testing::Test
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


TEST_F(UtilsTest, getIodeFileType)
{
	std::string filename = "";
	EXPECT_EQ(get_iode_file_type(filename), FILE_ANY);

	filename = "ws";
	EXPECT_EQ(get_iode_file_type(filename), FILE_ANY);

	EXPECT_EQ(get_iode_file_type(input_test_dir), DIRECTORY);

	filename = input_test_dir + prefix_filename + "fun.cmt";
	EXPECT_EQ(get_iode_file_type(filename), FILE_COMMENTS);
	filename = input_test_dir + "fun.ac";
	EXPECT_EQ(get_iode_file_type(filename), FILE_COMMENTS);

	filename = input_test_dir + prefix_filename + "fun.eqs";
	EXPECT_EQ(get_iode_file_type(filename), FILE_EQUATIONS);
	filename = input_test_dir + "fun.ae";
	EXPECT_EQ(get_iode_file_type(filename), FILE_EQUATIONS);

	filename = input_test_dir + prefix_filename + "fun.idt";
	EXPECT_EQ(get_iode_file_type(filename), FILE_IDENTITIES);
	filename = input_test_dir + "fun.ai";
	EXPECT_EQ(get_iode_file_type(filename), FILE_IDENTITIES);

	filename = input_test_dir + prefix_filename + "fun.lst";
	EXPECT_EQ(get_iode_file_type(filename), FILE_LISTS);
	filename = input_test_dir + "fun.al";
	EXPECT_EQ(get_iode_file_type(filename), FILE_LISTS);

	filename = input_test_dir + prefix_filename + "fun.scl";
	EXPECT_EQ(get_iode_file_type(filename), FILE_SCALARS);
	filename = input_test_dir + "fun.as";
	EXPECT_EQ(get_iode_file_type(filename), FILE_SCALARS);

	filename = input_test_dir + prefix_filename + "fun.tbl";
	EXPECT_EQ(get_iode_file_type(filename), FILE_TABLES);
	filename = input_test_dir + "fun.at";
	EXPECT_EQ(get_iode_file_type(filename), FILE_TABLES);

	filename = input_test_dir + prefix_filename + "fun.var";
	EXPECT_EQ(get_iode_file_type(filename), FILE_VARIABLES);
	filename = input_test_dir + "fun.av";
	EXPECT_EQ(get_iode_file_type(filename), FILE_VARIABLES);

	filename = input_test_dir + "fun.rep";
	EXPECT_EQ(get_iode_file_type(filename), FILE_REP);

	filename = input_test_dir + "fun.log";
	EXPECT_EQ(get_iode_file_type(filename), FILE_LOG);

	filename = input_test_dir + "fun.ini";
	EXPECT_EQ(get_iode_file_type(filename), FILE_SETTINGS);

	filename = input_test_dir + "fun.txt";
	EXPECT_EQ(get_iode_file_type(filename), FILE_TXT);

	filename = input_test_dir + "fun.a2m";
	EXPECT_EQ(get_iode_file_type(filename), FILE_A2M);

	filename = input_test_dir + "fun.agl";
	EXPECT_EQ(get_iode_file_type(filename), FILE_AGL);

	filename = input_test_dir + "fun.prf";
	EXPECT_EQ(get_iode_file_type(filename), FILE_PRF);

	filename = input_test_dir + "fun.dif";
	EXPECT_EQ(get_iode_file_type(filename), FILE_DIF);

	filename = input_test_dir + "fun.mif";
	EXPECT_EQ(get_iode_file_type(filename), FILE_MIF);

	filename = input_test_dir + "fun.rtf";
	EXPECT_EQ(get_iode_file_type(filename), FILE_RTF);

	filename = input_test_dir + "fun.asc";
	EXPECT_EQ(get_iode_file_type(filename), FILE_AAS);

	filename = input_test_dir + "fun.ref";
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
	filepath = output_test_dir + "fun.eqs";
	EXPECT_THROW(check_filepath(filepath, FILE_COMMENTS, "save", false), std::invalid_argument);

	filepath = output_test_dir + "fun.docx";
	EXPECT_THROW(check_filepath(filepath, FILE_TXT, "save", false), std::invalid_argument);

	// file does not exist
	filepath = input_test_dir + "funxxx.cmt";
	EXPECT_THROW(check_filepath(filepath, FILE_COMMENTS, "load", true), std::invalid_argument);

	// file does not exist (no extension given)
	filepath = input_test_dir + "funxxx";
	EXPECT_THROW(check_filepath(filepath, FILE_COMMENTS, "load", true), std::invalid_argument);

	// No extension but not an IODE objects file
	filepath = input_test_dir + "fun";
	EXPECT_THROW(check_filepath(filepath, FILE_TXT, "load", true), std::invalid_argument);

	// extension added automatically
	filepath = input_test_dir + prefix_filename + "fun";
	filepath = check_filepath(filepath, FILE_COMMENTS, "load", true);
	EXPECT_EQ(filepath, input_test_dir + prefix_filename + "fun.cmt");
}


TEST_F(UtilsTest, removeDuplicates)
{
	char* pattern = "B*;G*;QA*;*_";

	KDBComments kdb_cmt(input_test_dir + "fun.ac");

	std::vector<std::string> v_expected_duplicate = { "BENEF", "BENEF_", "BQY", "BVY", 
		"GAP", "GOSF", "GOSG", "GOSH", "GOSH_", "QAFF", "QAFF_", "QAF_", "QAG", "QAH", 
		"QAI_", "QAT", "QAT_", "BENEF_", "GOSH_", "IDH_", "PAFF_", "PC_", "PFI_", "QAFF_", 
		"QAF_", "QAI_", "QAT_", "QBBPPOT_", "QC_", "QQMAB_", "QS_", "Q_", "TFPHP_", 
		"VAFF_", "VAI_", "VAT_", "VC_", "VS_", "WBF_", "WBU_", "WCF_", "WCR1_", "WCR2_", 
		"WIND_", "WNF_", "YDH_", "ZZ_" };
	int nb_names_with_duplicate = (int) v_expected_duplicate.size();

	std::vector<std::string> v_expected_unique = { "BENEF", "BENEF_", "BQY", "BVY", 
		"GAP", "GOSF", "GOSG", "GOSH", "GOSH_", "IDH_", "PAFF_", "PC_", "PFI_", "QAFF", 
		"QAFF_", "QAF_", "QAG", "QAH", "QAI_", "QAT", "QAT_", "QBBPPOT_", "QC_", "QQMAB_", 
		"QS_", "Q_", "TFPHP_", "VAFF_", "VAI_", "VAT_", "VC_", "VS_", "WBF_", "WBU_", 
		"WCF_", "WCR1_", "WCR2_", "WIND_", "WNF_", "YDH_", "ZZ_" };
	int nb_names_unique = (int) v_expected_unique.size();

	// Before removing duplicates
	// Retrieves all object names matching one or more patterns in K_WS (similar to grep)
	char* c_lst = K_expand(COMMENTS, NULL, pattern, '*');
	// Parses a string and replaces @filename and $listname by their contents
	char** c_names = B_ainit_chk(c_lst, NULL, 0);
	int nb_names = SCR_tbl_size((unsigned char**) c_names);
	std::vector<std::string> v_names;
	for (int i=0; i < nb_names; i++) v_names.push_back(std::string(c_names[i]));
	EXPECT_EQ(nb_names, nb_names_with_duplicate);
	EXPECT_EQ(v_names, v_expected_duplicate);

	// (char**) After removing duplicates
	c_names = remove_duplicates(c_names);
	nb_names = SCR_tbl_size((unsigned char**) c_names);
	v_names.clear();
	for (int i=0; i < nb_names; i++) v_names.push_back(std::string(c_names[i]));
	EXPECT_EQ(nb_names, nb_names_unique);
	EXPECT_EQ(v_names, v_expected_unique);
	
	// (vector<string>) After removing duplicates
	v_names = v_expected_duplicate;
	remove_duplicates(v_names);
	nb_names = (int) v_names.size();
	EXPECT_EQ(nb_names, nb_names_unique);
	EXPECT_EQ(v_names, v_expected_unique);
}
