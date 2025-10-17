import itertools
import numpy as np
from pathlib import Path
from iode import variables


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


if __name__ == "__main__":
    current_dir = Path(__file__).parent.resolve()
    filepath = current_dir / "data" / "big.var"

    populate_big_vars()

    save_big_vars_ws(str(filepath))

    filepath = filepath.with_suffix(".av")

    save_big_vars_ws(str(filepath))
