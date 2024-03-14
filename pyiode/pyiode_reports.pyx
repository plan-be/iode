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
# TODO: add parameters
def report_exec(filename_parms: str):
    '''Execute a report'''
    rc = B_ReportExec(cstr(filename_parms))
    if rc != 0:
        raise  RuntimeError(f"Execution of report {'filename_parms'} has failed. rc = {rc}")


# $ExecReportLine repline
def reportline_exec(repline: Union[str, List[str]]):
    '''Execute report line(s)'''
    
    repline = arg_to_str(repline, sep = '\n')
    rc = B_ReportLine(cstr(repline))
    if rc != 0:
        raise  RuntimeError(f"Execution of report line '{repline}' has failed. rc = {rc}")
