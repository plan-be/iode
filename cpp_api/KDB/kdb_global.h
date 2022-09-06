#pragma once
#include "cpp_api/utils.h"


void load_global_kdb(EnumIodeType iode_type, std::string& filepath);

void save_global_kdb(EnumIodeType iode_type, std::string& filepath);

void clear_global_kdb(EnumIodeType iode_type);

char** filter_kdb_names(const EnumIodeType iode_type, const std::string& pattern);

// QUESTION FOR JMP: is there a K function that already create a hard copy of subset of a KDB ?
KDB* hard_copy_kdb(KDB* source_kdb, char** names);
