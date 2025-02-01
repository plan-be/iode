from pathlib import Path
from iode import comments, equations, identities, lists, scalars, tables, variables
from iode import SAMPLE_DATA_DIR, view_workspace


global_var = 10


def run_view_workspace():
    comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
    equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
    lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
    scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
    tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
    local_var = [1, 2, 3, 4, 5]
    # will open a viewer showing all IODE databases
    view_workspace()


if __name__ == "__main__":
    try:
        from iode_gui import start
        start()

        run_view_workspace()
    
        run_view_workspace()
    except ImportError:
        pass
