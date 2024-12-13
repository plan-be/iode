#include "py_super.h"


void c_gui_assign_super()
{
	IODE_assign_super_API();
	kerror_super = error_super_GUI;
	kwarning_super = warning_super_GUI;
	kmsg_super = msg_super_GUI;
	kconfirm_super = confirm_super_GUI;
	kpanic_super = panic_super_GUI;

	SB_PrintObjDef_super    = PrintObjDef_super_GUI;
	SB_ViewPrintGr_super    = ViewPrintGr_super_GUI;
	SB_ViewPrintTbl_super   = ViewPrintTbl_super_GUI;
	SB_ViewByTbl_super      = ViewByTbl_super_GUI;
	SB_DataSearch_super     = DataSearch_super_GUI;
	SB_DataDuplicate_super  = DataDuplicate_super_GUI;
	SB_DataList_super       = DataList_super_GUI;
	SB_DataCompare_super    = DataCompare_super_GUI;
	SB_DataCalcLst_super    = DataCalcLst_super_GUI;
	SB_DataListSort_super   = DataListSort_super_GUI;
	SB_DataEditGraph_super  = DataEditGraph_super_GUI;
	SB_DataScan_super       = DataScan_super_GUI;
	SB_StatUnitRoot_super   = StatUnitRoot_super_GUI;
	SB_WsLoad_super         = WsLoad_super_GUI;
	SB_WsSave_super         = WsSave_super_GUI;
	SB_WsMerge_super        = WsMerge_super_GUI;
	SB_WsDescr_super        = WsDescr_super_GUI;
	SB_WsCopy_super         = WsCopy_super_GUI;
	SB_WsClear_super        = WsClear_super_GUI;
	SB_WsSample_super       = WsSample_super_GUI;
	SB_WsExtrapolate_super  = WsExtrapolate_super_GUI;
	SB_WsHtoL_super         = WsHtoL_super_GUI;
	SB_WsLtoH_super         = WsLtoH_super_GUI;
	SB_WsSeasonAdj_super    = WsSeasonAdj_super_GUI;
	SB_WsTrend_super        = WsTrend_super_GUI;
	SB_ModelSimulate_super  = ModelSimulate_super_GUI;
	SB_ModelCompile_super   = ModelCompile_super_GUI;
	SB_IdtExecute_super     = IdtExecute_super_GUI;
	SB_EqsEstimate_super    = EqsEstimate_super_GUI;
	SB_XodeRuleImport_super = XodeRuleImport_super_GUI;

	T_view_tbl_super        = view_tbl_super_GUI;
}
