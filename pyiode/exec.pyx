#  IODE EXTENSION FOR PYTHON
#  =========================
# 
#     @header4iode
#  
#  IODE identity execution
#  -----------------------
# 
#   idt_execute(sample: Optional[Union[str, List[str]]] = None, idt_list: Optional[Union[str, List[str]]] = None, var_files: Optional[Union[str, List[str]]] = None, scl_files: Optional[Union[str, List[str]]] = None, trace: bool = False) | Execute a list of identities on a given sample

import warnings

cdef extern from "api/iode.h": 
    cdef int    IodeExecuteIdts(char *smpl, char *idt_list, char *var_files, char *scl_files, int trace)


def idt_execute(sample: Optional[Union[str, List[str]]] = None, 
                idt_list: Optional[Union[str, List[str]]] = None, 
                var_files: Optional[Union[str, List[str]]] = None, 
                scl_files: Optional[Union[str, List[str]]] = None,
                trace: bool = False):
    r'''
    Execute a list of identities
    
    Parameters
    ----------
    sample: Optional[Union[str, List[str]]] = None
            range of period on which the identities must be calculated
            if sample is None or empty, the WS sample is used 
            
    idt_list: Optional[Union[str, List[str]]] = None
            list of identities to execute
            if empty, all identities are executed
            
    var_files: Optional[Union[str, List[str]]] = None
            list of files the needed variables must be read from. "WS" means current current Variable WS.
            if empty, only the current KV_WS is used
            
    scl_files: Optional[Union[str, List[str]]] = None
            list of files the needed scalars must be read from. "WS" means current current WS.
            if empty, only the current workspace is used
                        
    trace: bool = False
            optional trace indicating the source of the variables and scalars. For example:
            
            Execution of identities
            
            Parameters
                Execution sample : 1960Y1:2015Y1
            
                Variables loaded
                    From WS : AOUC AOUC_ COTRES DEBT DTF DTH EFMY EFXY EX FLG FLGR GAP2 GAP_ GOSF HF  
                    
                Scalars loaded
                    From ../data/fun : gamma gamma2 gamma3 gamma4 knf3
    '''
    warnings.warn("idt_execute() is deprecated. " + 
        "Please use the new syntax:\nidt_db = Identities()\nidt_db.execute(identities, from_period, " + 
        "to_period, var_files, scalar_files, trace)", DeprecationWarning)
    idt_db = Identities()
    from_period, to_period = sample.split(';')
    idt_db.execute(idt_list, from_period, to_period, var_files, scl_files, trace)
