#pragma once

#include "api/constants.h"
#include "api/objs/objs.h"
#include "api/objs/identities.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"

KDBVariables* KI_exec(KDBIdentities* dbi, KDBVariables* dbv, int nv, char* vfiles[], 
    KDBScalars* dbs, int ns, char* sfiles[], Sample* in_smpl);
