#pragma once

#include "api/pch.h"
#include "api/objs/objs.h"
#include "api/objs/identities.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"

inline std::set<std::string> idt_exec_loaded_vars;

KDBVariablesPtr KI_exec(KDBIdentitiesPtr dbi_ptr, KDBVariablesPtr dbv_ptr, int nv, char* vfiles[], 
    KDBScalarsPtr dbs_ptr, int ns, char* sfiles[], Sample* in_smpl);
