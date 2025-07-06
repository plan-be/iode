#pragma once

#include "api/constants.h"

#define IODE_VERSION "IODE Modeling Software 7.1.0 - (c) 1990-2025 Federal Planning Bureau - Brussels"
#define IODE_VERSION_MAJOR 7
#define IODE_VERSION_MINOR 1
#define IODE_VERSION_PATCH 0

#ifndef ARCH
    #ifdef __GNUC__
        #define ARCH    "AMD64"
    #else
        #define ARCH    "INTEL86 DOS"
    #endif
#endif

const static char* IODE_WEBSITE = "https://iode.readthedocs.io/en/stable/";
const static char* CHANGELOG_URL = "https://iode.readthedocs.io/en/stable/changes.html";
const static char* PYTHON_API_URL = "https://iode.readthedocs.io/en/stable/api.html";
const static char* ORGANIZATION_NAME = "Federal Planning Bureau (Belgium)";
