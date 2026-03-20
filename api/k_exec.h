#pragma once

#include "api/constants.h"
#include "api/objs/objs.h"
#include "api/objs/identities.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"

std::shared_ptr<KDBVariables> KI_exec(std::shared_ptr<KDBIdentities> dbi, std::shared_ptr<KDBVariables> dbv, int nv, char* vfiles[], 
    std::shared_ptr<KDBScalars> dbs, int ns, char* sfiles[], Sample* in_smpl);
