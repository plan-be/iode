from pathlib import Path
import qtpy
from iode import comments, equations, identities, lists, scalars, tables, variables
from iode import SAMPLE_DATA_DIR, view_workspace


class MyClass:
    def __init__(self, value: str):
        self.value = value


def my_function():
    print("This is an imported function")


global_list = [1, 2, 3, 4, 5]
global_instance = MyClass("global instance of MyClass")


def run_view_workspace():
    comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
    equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
    lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
    scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
    tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
    local_list = [6, 7, 8, 9, 10]
    local_instance = MyClass("local instance of MyClass")
    
    view_workspace()
    
    print("local_list:", local_list)
    print("local_instance:", local_instance.value)


if __name__ == "__main__":
    print("Starting IODE GUI...")
    print("qtpy version:", qtpy.__version__)
    print("Qt version:", qtpy.QT_VERSION)
    print("Qt API name:", qtpy.API_NAME)

    run_view_workspace()
    
    print("global_list:", global_list)
    print("global_instance:", global_instance.value)
    print(variables["A*", "1960Y1:1965Y1"])
