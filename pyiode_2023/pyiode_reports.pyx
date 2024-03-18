# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
#  IODE report functions
#  ---------------------
#   report_exec(filename: str, params: Optional[Union[str, List[str]]] = None) | Execute a report with optional parameters
#   reportline_exec(repline: Union[str, List[str]])                            | Execute one or more report line(s)


# $ExecReport filename params
# TODO: if some parameters contain spaces or ", quotes must be escaped and the params have to be enclosed in double quotes.
def report_exec(filename: str, params: Optional[Union[str, List[str]]] = None):
    '''Execute a report'''
    
    filename_parms = filename + ' ' + arg_to_str(params, sep = ' ')
    
    rc = B_ReportExec(cstr(filename_parms))
    if rc != 0:
        raise  RuntimeError(f"Execution of report {'filename_parms'} has failed. rc = {rc}")


# $ExecReportLine repline
def reportline_exec(repline: Union[str, List[str]]):
    '''Execute a report line'''
    
    repline = arg_to_str(repline, sep = '\n')
    rc = B_ReportLine(cstr(repline))
    if rc != 0:
        raise  RuntimeError(f"Execution of report line '{repline}' has failed. rc = {rc}")


