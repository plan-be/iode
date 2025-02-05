import sys
import warnings
from iode.iode_cython import register_super_function
from iode.iode_cython import skip_pause
from iode.iode_cython import skip_msg_box


__all__ = ['skip_pause', 'skip_msg_box']


@register_super_function('error')
def py_error(level: int, msg: str):
    if level > 0:
        sys.exit(level)
    else:
        raise RuntimeError(msg)

@register_super_function('warning')
def py_warning(msg: str):
    warnings.warn(msg, RuntimeWarning)

@register_super_function('message')
def py_message(msg: str):
    print(msg)

@register_super_function('confirm')
def py_confirm(msg: str) -> bool:
    yes = "oyj1"
    answer = input(f'{msg} [y/n] ').lower()
    if len(answer) > 1:
        answer = answer[0]
    return answer in yes

@register_super_function('panic')
def py_panic(msg: str):
    print("Swap Panic!")
    print("Attempt to save data in ws.* files.")
    print("To restart iode with the last data, type :")
    print("    iode -ws")
    sys.exit(2)

@register_super_function('pause')
def py_pause():
    input("Press Enter to continue...")

@register_super_function('msgbox')
def py_kmsgbox(title: str, msg: str) -> int:
    print(f'{title}: {msg}')
    return 1

@register_super_function('PrintObjDef')
def PrintObjDef_super() -> int:
    warnings.warn("GUI function for 'PrintObjDef' not found")
    return 0

@register_super_function('ViewPrintGr')
def ViewPrintGr_super() -> int:
    warnings.warn("GUI function for 'ViewPrintGr' not found")
    return 0

@register_super_function('ViewPrintTbl')
def ViewPrintTbl_super() -> int:
    warnings.warn("GUI function for 'ViewPrintTbl' not found")
    return 0

@register_super_function('ViewByTbl')
def ViewByTbl_super() -> int:
    warnings.warn("GUI function for 'ViewByTbl' not found")
    return 0

@register_super_function('DataSearch')
def DataSearch_super() -> int:
    warnings.warn("GUI function for 'DataSearch' not found")
    return 0

@register_super_function('DataDuplicate')
def DataDuplicate_super() -> int:
    warnings.warn("GUI function for 'DataDuplicate' not found")
    return 0

@register_super_function('DataList')
def DataList_super() -> int:
    warnings.warn("GUI function for 'DataList' not found")
    return 0

@register_super_function('DataCompare')
def DataCompare_super() -> int:
    warnings.warn("GUI function for 'DataCompare' not found")
    return 0

@register_super_function('DataCalcLst')
def DataCalcLst_super() -> int:
    warnings.warn("GUI function for 'DataCalcLst' not found")
    return 0

@register_super_function('DataListSort')
def DataListSort_super() -> int:
    warnings.warn("GUI function for 'DataListSort' not found")
    return 0

@register_super_function('DataEditGraph')
def DataEditGraph_super() -> int:
    warnings.warn("GUI function for 'DataEditGraph' not found")
    return 0

@register_super_function('DataScan')
def DataScan_super() -> int:
    warnings.warn("GUI function for 'DataScan' not found")
    return 0

@register_super_function('StatUnitRoot')
def StatUnitRoot_super() -> int:
    warnings.warn("GUI function for 'StatUnitRoot' not found")
    return 0

@register_super_function('WsLoad')
def WsLoad_super() -> int:
    warnings.warn("GUI function for 'WsLoad' not found")
    return 0

@register_super_function('WsSave')
def WsSave_super() -> int:
    warnings.warn("GUI function for 'WsSave' not found")
    return 0

@register_super_function('WsMerge')
def WsMerge_super() -> int:
    warnings.warn("GUI function for 'WsMerge' not found")
    return 0

@register_super_function('WsDescr')
def WsDescr_super() -> int:
    warnings.warn("GUI function for 'WsDescr' not found")
    return 0

@register_super_function('WsCopy')
def WsCopy_super() -> int:
    warnings.warn("GUI function for 'WsCopy' not found")
    return 0

@register_super_function('WsClear')
def WsClear_super() -> int:
    warnings.warn("GUI function for 'WsClear' not found")
    return 0

@register_super_function('WsSample')
def WsSample_super() -> int:
    warnings.warn("GUI function for 'WsSample' not found")
    return 0

@register_super_function('WsExtrapolate')
def WsExtrapolate_super() -> int:
    warnings.warn("GUI function for 'WsExtrapolate' not found")
    return 0

@register_super_function('WsHtoL')
def WsHtoL_super() -> int:
    warnings.warn("GUI function for 'WsHtoL' not found")
    return 0

@register_super_function('WsLtoH')
def WsLtoH_super() -> int:
    warnings.warn("GUI function for 'WsLtoH' not found")
    return 0

@register_super_function('WsSeasonAdj')
def WsSeasonAdj_super() -> int:
    warnings.warn("GUI function for 'WsSeasonAdj' not found")
    return 0

@register_super_function('WsTrend')
def WsTrend_super() -> int:
    warnings.warn("GUI function for 'WsTrend' not found")
    return 0

@register_super_function('ModelSimulate')
def ModelSimulate_super() -> int:
    warnings.warn("GUI function for 'ModelSimulate' not found")
    return 0

@register_super_function('ModelCompile')
def ModelCompile_super() -> int:
    warnings.warn("GUI function for 'ModelCompile' not found")
    return 0

@register_super_function('IdtExecute')
def IdtExecute_super() -> int:
    warnings.warn("GUI function for 'IdtExecute' not found")
    return 0

@register_super_function('EqsEstimate')
def EqsEstimate_super() -> int:
    warnings.warn("GUI function for 'EqsEstimate' not found")
    return 0

@register_super_function('XodeRuleImport')
def XodeRuleImport_super() -> int:
    warnings.warn("GUI function for 'XodeRuleImport' not found")
    return 0

@register_super_function('ViewTable')
def ViewTable_super(table_name: str, generalized_sample: str, nb_decimals: int) -> int:
    warnings.warn("GUI function for 'ViewTable' not found")
    return 0
