# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
#  IODE report functions
#  ---------------------
#   data_update_var(varname, values, operation = "L", per_from = None)
#   report(filename_parms)
#   reportline(repline)


# $ExecReport filename_parms
def report(filename_parms:str):
    return(B_ReportExec(cstr(filename_parms)))

# $ExecReportLine repline
def reportline(repline:str):
   return(B_ReportLine(cstr(repline)))


