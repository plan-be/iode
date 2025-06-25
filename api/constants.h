#pragma once

// ALD 23/11/2023
#include "version.h"

#define K_VERSION  "1.0"
#define OK_MAX_NAME  10
#define K_MAX_NAME   20  /* IODE64K */

// ALD 18/11/2022
#define I_DEFAULT_FILENAME  "ws"

// JMP 23/5/2019 (from b_rep.c for b_proc.c)
#define LINELENGTH 102400

#define OK_MAX_FILE  64
#define K_MAX_FILE  512
#define K_MAX_DESC  64
//#define K_CHUNCK    1024 /* JMP 11-03-07 */
#define K_CHUNCK    (64 * 1024)
#define LMAGIC      32
