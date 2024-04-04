from pathlib import Path

SAMPLE_DATA_DIR = str((Path(__file__).parent / "tests" / "data").resolve())

from .iode_python import *
