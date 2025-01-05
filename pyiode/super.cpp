#pragma once
#include "api/iode.h"
#include "cpp_api/iode_cpp_api.h"
#include "super.h"


void python_assign_super()
{
    kerror_super   = c_kerror_super;
    kwarning_super = c_kwarning_super;
    kmsg_super     = c_kmsg_super;
    kconfirm_super = c_kconfirm_super;
    kpanic_super   = c_kpanic_super;

	SB_PrintObjDef_super    = c_PrintObjDef_super;
	SB_ViewPrintGr_super    = c_ViewPrintGr_super;
	SB_ViewPrintTbl_super   = c_ViewPrintTbl_super;
	SB_ViewByTbl_super      = c_ViewByTbl_super;
	SB_DataSearch_super     = c_DataSearch_super;
	SB_DataDuplicate_super  = c_DataDuplicate_super;
	SB_DataList_super       = c_DataList_super;
	SB_DataCompare_super    = c_DataCompare_super;
	SB_DataCalcLst_super    = c_DataCalcLst_super;
	SB_DataListSort_super   = c_DataListSort_super;
	SB_DataEditGraph_super  = c_DataEditGraph_super;
	SB_DataScan_super       = c_DataScan_super;
	SB_StatUnitRoot_super   = c_StatUnitRoot_super;
	SB_WsLoad_super         = c_WsLoad_super;
	SB_WsSave_super         = c_WsSave_super;
	SB_WsMerge_super        = c_WsMerge_super;
	SB_WsDescr_super        = c_WsDescr_super;
	SB_WsCopy_super         = c_WsCopy_super;
	SB_WsClear_super        = c_WsClear_super;
	SB_WsSample_super       = c_WsSample_super;
	SB_WsExtrapolate_super  = c_WsExtrapolate_super;
	SB_WsHtoL_super         = c_WsHtoL_super;
	SB_WsLtoH_super         = c_WsLtoH_super;
	SB_WsSeasonAdj_super    = c_WsSeasonAdj_super;
	SB_WsTrend_super        = c_WsTrend_super;
	SB_ModelSimulate_super  = c_ModelSimulate_super;
	SB_ModelCompile_super   = c_ModelCompile_super;
	SB_IdtExecute_super     = c_IdtExecute_super;
	SB_EqsEstimate_super    = c_EqsEstimate_super;
	SB_XodeRuleImport_super = c_XodeRuleImport_super;

	T_view_tbl_super        = c_ViewTable_super;
}
