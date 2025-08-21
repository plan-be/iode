#pragma once
#include "cpp_api/common.h"


// TODO ALD: remove K_RWS and make KDB classes to be able to represent a reference kdb

void load_reference_kdb(const int ref, IodeType iode_type, std::string& filepath);

void clear_reference_kdb(const int ref, IodeType iode_type);

void clear_all_reference_kdbs();