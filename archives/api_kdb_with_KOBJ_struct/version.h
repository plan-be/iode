#pragma once

#include <string>
#include "api/constants.h"

#define IODE_VERSION "IODE Modeling Software 7.0.4 - (c) 1990-2025 Federal Planning Bureau - Brussels"
#define IODE_VERSION_MAJOR 7
#define IODE_VERSION_MINOR 0
#define IODE_VERSION_PATCH 4

#ifndef ARCH
    #ifdef __GNUC__
        #define ARCH    "AMD64"
    #else
        #define ARCH    "INTEL86 DOS"
    #endif
#endif

inline const static std::string IODE_WEBSITE = "https://iode.readthedocs.io/en/stable/";
inline const static std::string CHANGELOG_URL = "https://iode.readthedocs.io/en/stable/changes.html";
inline const static std::string PYTHON_API_URL = "https://iode.readthedocs.io/en/stable/api.html";
inline const static std::string ORGANIZATION_NAME = "Federal Planning Bureau (Belgium)";
