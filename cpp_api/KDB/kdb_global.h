#pragma once
#include "../utils.h"


void load_global_kdb(EnumIodeType iode_type, std::string& filepath);

void save_global_kdb(EnumIodeType iode_type, std::string& filepath);

void clear_global_kdb(EnumIodeType iode_type);

char** filter_kdb_names(const EnumIodeType iode_type, const std::string& pattern);
