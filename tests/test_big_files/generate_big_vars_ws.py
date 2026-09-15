import itertools
import numpy as np
from pathlib import Path
from iode import variables
from iode import skip_message       # iode version >= 7.0.8
# from iode import suppress_msgs    # iode version < 7.0.8


def populate_big_vars():
    letters = [chr(i) for i in range(65, 91)]   # 'A' to 'Z'
    names = [''.join(pair) for pair in itertools.product(letters, repeat=3)]
    names = [f"{a}{i}" for a in names for i in range(0, 10)]
    variables.sample = "1990Y1:2060Y1"
    for name in names:
        variables[name] = np.random.normal(1000, 500, size=71)
    print(f"Generated {len(names)} variables")
    print(f"sample '{variables.sample}'")


def save_big_vars_ws(filepath: str):
    variables.save(filepath)
    print(f"Saved big vars workspace to '{filepath}'")

def load_big_vars_ws(filepath: str):
    variables.clear()
    variables.load(filepath)
    print(f"Loaded big vars workspace from '{filepath}'")

if __name__ == "__main__":
    current_dir = Path(__file__).parent.resolve()

    # iode version < 7.0.8
    # suppress_msgs()
    # iode version >= 7.0.8
    skip_message(True)

    populate_big_vars()

    data_dir = current_dir / "data"
    data_dir.mkdir(exist_ok=True)

    filepath = data_dir / "big.var"
    if not filepath.exists():
        save_big_vars_ws(str(filepath))

    if filepath.exists():
        load_big_vars_ws(str(filepath))

    filepath = filepath.with_suffix(".av")
    if not filepath.exists():
        save_big_vars_ws(str(filepath))

    if filepath.exists():
        load_big_vars_ws(str(filepath))
