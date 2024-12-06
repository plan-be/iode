
def iode_error(level: int, message: str):
    c_error_super(level, message.encode('utf-8'))

def iode_warning(message: str):
    c_warning_super(message.encode('utf-8'))

def iode_msg(message: str):
    c_msg_super(message.encode('utf-8'))

def iode_confirm(message: str):
    return c_confirm_super(message.encode('utf-8'))

def iode_panic():
    c_panic_super()
