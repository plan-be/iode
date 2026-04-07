#pragma once 

#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"

// K_LZH : indicates if a saved WS must be compressed or not 
// -> https://en.wikipedia.org/wiki/LHA_(file_format).
inline int  K_LZH = 0; 

inline char K_LABELS[] = "KOBJS 4.02\032";  // Version 1
inline char K_LABELD[] = "KOBJS 504d\032";  // Version 2
inline char K_LABELX[] = "KOBJS 564A\032";  // Version 3 
inline char K_LABEL[]  = "KOBJS 564A\032";  // Version 0 = Current version = Version 3

// k_objvers.cpp
int get_version(char* label);

// k_objfile.cpp
std::string set_file_extension(const std::string& filepath, const int file_type);
int get_file_type_from_file(const std::string& filepath, int file_type);
bool update_filename(const std::string& from, const std::string& to);
int X_findtype(char* arg);
