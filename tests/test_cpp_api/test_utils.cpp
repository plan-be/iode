#include "pch.h"
#include "../../cpp_api/iode_cpp_api.h"


TEST(TestUtils, Utf8_oem_convertion)
{
	std::string str_utf8 = u8"aA!@^$jndq256";

	std::string str_oem = convert_utf8_to_oem(str_utf8);
	std::string str_res = convert_oem_to_utf8(str_oem);

	EXPECT_EQ(str_res, str_utf8);
}
