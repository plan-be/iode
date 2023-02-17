# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
#  IODE report functions
#  ---------------------
#   report_exec(filename_parms)     | Execute a report
#   reportline_exec(repline)        | Execute a report line


# $ExecReport filename_parms
def report_exec(filename_parms:str):
    '''Execute a report'''
    return(B_ReportExec(cstr(filename_parms)))

# $ExecReportLine repline
def reportline(repline:str):
    '''Execute a report line'''
    return(B_ReportLine(cstr(repline)))


