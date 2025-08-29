from pathlib import Path
from typing import Union, List

from iode.util import enable_msgs, suppress_msgs
from iode.iode_cython import cython_load_extra_files, cython_reset_extra_files


def load_extra_files(extra_files: Union[str, Path, List[Union[str, Path]]], quiet: bool=False) -> List[Path]:
    r"""
    Load extra file(s) referenced in generalized samples.
    Maximum 4 files can be passed as argument.
    The file [1] always refers to the current workspace. 
    Extra files are numerated from 2 to 5.

    Parameters
    ----------
    extra_files: str or Path or list(str) or list(Path)
        The extra files to load. Can be a single file, a list of files, 
        or a string with file paths separated by semicolons.
    quiet: bool, optional
        If True, suppresses the log messages during the loading of files. 
        Default is False.
            
    Examples
    --------
    >>> from pathlib import Path
    >>> from iode import SAMPLE_DATA_DIR
    >>> from iode import load_extra_files

    >>> sample_data_dir = Path(SAMPLE_DATA_DIR)
    >>> extra_files = [sample_data_dir / "ref.av", sample_data_dir / "fun.av", 
    ...                sample_data_dir / "fun2.av", sample_data_dir / "a.var"]
    >>> extra_files = load_extra_files(extra_files)         # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    Loading ...\ref.av
    ...
    394 objects loaded
    Loading ...\fun.av
    ...
    394 objects loaded
    Loading ...\fun2.av
    ...
    394 objects loaded
    Loading ...\a.var
    433 objects loaded
    >>> extra_files = load_extra_files(extra_files, quiet=True)
    >>> [Path(filepath).name for filepath in extra_files]
    ['ref.av', 'fun.av', 'fun2.av', 'a.var']
    """
    reset_extra_files()

    if isinstance(extra_files, str):
        extra_files = [extra_file.strip() for extra_file in extra_files.split(';')]
    if isinstance(extra_files, Path):
        extra_files = [extra_files]
    if len(extra_files) > 4:
        raise ValueError("The number of extra files cannot exceed 4")
    if not all(isinstance(extra_file, (str, Path)) for extra_file in extra_files):
        raise TypeError("'extra_files' must be a string, a path or a list of strings or paths")
    extra_files = [Path(extra_file) for extra_file in extra_files]

    if quiet:
        suppress_msgs()
    
    try:
        extra_files = cython_load_extra_files(extra_files)
        if quiet:
            enable_msgs()
        return extra_files
    except Exception as e:
        if quiet:
            enable_msgs()
        raise e


def reset_extra_files():
    r"""
    reset extra files referenced in generalized samples.

    Parameters
    ----------
    extra_files: str or Path or list(str) or list(Path)

    Examples
    --------
    >>> from pathlib import Path
    >>> from iode import SAMPLE_DATA_DIR
    >>> from iode import load_extra_files, reset_extra_files

    >>> sample_data_dir = Path(SAMPLE_DATA_DIR)
    >>> extra_files = [sample_data_dir / "ref.av", sample_data_dir / "fun.av", 
    ...                sample_data_dir / "fun2.av", sample_data_dir / "a.var"]
    >>> extra_files = load_extra_files(extra_files, quiet=True)
    >>> [Path(filepath).name for filepath in extra_files]
    ['ref.av', 'fun.av', 'fun2.av', 'a.var']

    >>> reset_extra_files()
    >>> load_extra_files([])
    []
    """
    cython_reset_extra_files()
