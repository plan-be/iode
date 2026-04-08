#pragma once

#include "api/constants.h"
#include "api/objs/objs.h"
#include "api/objs/identities.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"

KDBVariablesPtr KI_exec(KDBIdentitiesPtr dbi_ptr, KDBVariablesPtr dbv_ptr, int nv, char* vfiles[], 
    KDBScalarsPtr dbs_ptr, int ns, char* sfiles[], Sample* in_smpl);
