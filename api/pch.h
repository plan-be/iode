#pragma once

// Standard C library headers
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

// Standard C++ library headers - Containers
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <array>
#include <bitset>
#include <string>
#include <string_view>

// Standard C++ library headers - Memory
#include <memory>
#include <functional>

// Standard C++ library headers - Algorithms
#include <algorithm>
#include <numeric>
#include <utility>

// Standard C++ library headers - I/O and Strings
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

// Standard C++ library headers - Utilities
#include <variant>
#include <stdexcept>
#include <filesystem>

// Platform-specific headers
#ifdef _WIN32
    #include <Windows.h>
#else
    #include <unistd.h>
    #include <sys/stat.h>
    #include <iconv.h>
#endif

// IODE project headers - these are stable and widely used
#include "api/constants.h"
#include "api/utils/utils.h"
#include "api/utils/logging.h"
