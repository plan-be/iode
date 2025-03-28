﻿// cpp_api.h : Header file for your target.

#pragma once

#include "cpp_api/common.h"
#include "cpp_api/utils/utils.h"
#include "utils/super.h"
#include "report/report.h"
#include "report/rep_list.h"
#include "cpp_api/time/period.h"
#include "cpp_api/time/sample.h"
#include "cpp_api/objects/equation.h"
#include "cpp_api/objects/identity.h"
#include "cpp_api/objects/table.h"
#include "cpp_api/objects/scalar.h"
#include "cpp_api/KDB/kdb_global.h"
#include "cpp_api/KDB/kdb_reference.h"
#include "cpp_api/KDB/kdb_comments.h"
#include "cpp_api/KDB/kdb_equations.h"
#include "cpp_api/KDB/kdb_identities.h"
#include "cpp_api/KDB/kdb_lists.h"
#include "cpp_api/KDB/kdb_scalars.h"
#include "cpp_api/KDB/kdb_tables.h"
#include "cpp_api/KDB/kdb_variables.h"
#include "cpp_api/lec/lec.h"
#include "compute/simulation.h"
#include "compute/estimation.h"
#include "computed_table/computed_table.h"
#include "computed_table/computed_table_graph.h"

void cpp_iode_init();
