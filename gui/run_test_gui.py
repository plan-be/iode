from pathlib import Path
from iode import comments, equations, identities, lists, scalars, tables, variables
from iode import SAMPLE_DATA_DIR
from iode_gui import main, open_application


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
    project_dir = Path('.').resolve()
    open_application(project_dir, called_from_python_script=True, depth=1)


if __name__ == "__main__":
    # run_view_workspace()
    main()
