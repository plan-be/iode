#pragma once
#include "utils/utils.h"


// TODO ALD: remove K_RWS and make KDB classes to be able to represent a reference kdb

void load_reference_kdb(const int ref, EnumIodeFile iode_file, std::string& filepath);

void clear_reference_kdb(const int ref, EnumIodeFile iode_file);

void clear_all_reference_kdbs();