#pragma once

#include "api/constants.h"
#include "api/objs/objs.h"
#include "api/objs/identities.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"

CKDBVariables* KI_exec(CKDBIdentities* dbi, CKDBVariables* dbv, int nv, char* vfiles[], 
    CKDBScalars* dbs, int ns, char* sfiles[], Sample* in_smpl);
