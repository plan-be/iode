# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
#  IODE report functions
#  ---------------------
#   report_exec(filename_parms: str)                | Execute a report with optional arguments
#   reportline_exec(repline: Union[str, List[str]]) | Execute report line(s)

from pyiode_reports cimport B_ReportExec, B_ReportLine

# $ExecReport filename_parms
# TODO: (ald) add Parameters, Returns and Examples section 
def report_exec(filename_parms: str):
    '''Execute a report'''
    rc = B_ReportExec(_cstr(filename_parms))
    if rc != 0:
        raise  RuntimeError(f"Execution of report {'filename_parms'} has failed. rc = {rc}")


# TODO: (ald) add Parameters, Returns and Examples section 
# $ExecReportLine repline
def reportline_exec(repline: Union[str, List[str]]):
    '''Execute report line(s)'''
    
    repline = _arg_to_str(repline, sep = '\n')
    rc = B_ReportLine(_cstr(repline))
    if rc != 0:
        raise  RuntimeError(f"Execution of report line '{repline}' has failed. rc = {rc}")
