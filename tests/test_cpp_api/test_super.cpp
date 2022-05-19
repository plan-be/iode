#include "pch.h"


TEST(KSuperTest, KError)
{
	cpp_assign_super_API();

	// check that call to kerror() throws an exception
	EXPECT_THROW(KC_load_asc("not_existing_file.ac"), std::runtime_error);
}
