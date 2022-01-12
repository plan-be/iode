#pragma once

/* ************************************************ *
 * copied/pasted from the /cmd/k_errorv.c directory *
 * ************************************************ */

#define _IODEVARG_
#include "../api/iode.h"

void kerror(int level, char* fmt, ...) {}

void kmsg(char* fmt, ...) {}
