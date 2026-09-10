#pragma once
#include "api/pch.h"
#include "api/constants.h"
#include "api/objs/kdb.h"


// TODO ALD: remove global_ref_xxx and make KDB classes to be able to represent a reference kdb

void load_reference_kdb(const int ref, std::string& filepath);

void clear_reference_kdb(const int ref);

void clear_all_reference_kdbs();
