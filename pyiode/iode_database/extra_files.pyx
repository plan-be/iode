from pathlib import Path
from typing import Union, List

from pyiode.iode_database.cpp_api_database cimport load_reference_kdb


def load_extra_files(extra_files: Union[str, Path, List[Union[str, Path]]]) -> List[Path]:
    """
    Load extra file(s) referenced in the generalized sample.
    Maximum 4 files can be passed as argument.
    The file [1] always refers to the current workspace. 
    Extra files are numeroted from 2 to 5.

    Parmeters
    ---------
    extra_files: str or Path or list(str) or list(Path)

    Examples
    --------
    >>> from pathlib import Path
    >>> from iode import SAMPLE_DATA_DIR
    >>> from iode import load_extra_files

    >>> sample_data_dir = Path(SAMPLE_DATA_DIR)
    >>> extra_files = [sample_data_dir / "ref.av", sample_data_dir / "fun.av", 
    ...                sample_data_dir / "fun2.av", sample_data_dir / "a.var"]
    >>> extra_files = load_extra_files(extra_files)
    >>> [Path(filepath).name for filepath in extra_files]
    ['ref.av', 'fun.av', 'fun2.av', 'a.var']
    """
    if isinstance(extra_files, str):
        extra_files = [extra_file.strip() for extra_file in extra_files.split(';')]
    if isinstance(extra_files, Path):
        extra_files = [extra_files]
    if len(extra_files) > 4:
        raise ValueError("The number of extra files cannot exceed 4")
    if not all(isinstance(extra_file, (str, Path)) for extra_file in extra_files):
        raise TypeError("'extra_files' must be a string, a path or a list of strings or paths")
    extra_files = [Path(extra_file) for extra_file in extra_files]
    for i, extra_file in enumerate(extra_files):
        load_reference_kdb(i + 2, IodeType.VARIABLES, str(extra_file).encode())
    return extra_files
