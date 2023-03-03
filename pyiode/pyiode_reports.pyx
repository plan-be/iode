# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
#  IODE report functions
#  ---------------------
#   report_exec(filename_parms: str) -> int     | Execute a report
#   reportline_exec(repline: str) -> int        | Execute a report line


# $ExecReport filename_parms
def report_exec(filename_parms: str) -> int:
    '''Execute a report'''
    rc = B_ReportExec(cstr(filename_parms))
    if rc != 0:
        raise  RuntimeError(f"Execution of report {'filename_parms'} has failed. rc = {rc}")
    else:
        return 0


# $ExecReportLine repline
def reportline(repline: str) -> int:
    '''Execute a report line'''
    rc = B_ReportLine(cstr(repline))
    if rc != 0:
        raise  RuntimeError(f"Execution of report line '{repline}' has failed. rc = {rc}")
    else:
        return 0


