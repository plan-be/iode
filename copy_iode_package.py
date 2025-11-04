# utility script to update iode package files *.py and *.pyi without having to recompile 
# (pip wheel) and reinstall (pip ) the package. Run this script after making changes to 
# the iode package source code
import shutil
from pathlib import Path
import iode


def copy_iode_package(source_dir: Path, target_dir: Path) -> None: 
    """
    Copy the iode package files from source_dir to target_dir.

    Parameters
    ----------
    source_dir : Path
        Path to the source iode package directory.
    target_dir : Path
        Path to the target iode package directory.
    """
    if not source_dir.exists():
        raise FileNotFoundError(f"Source directory '{source_dir}' does not exist.")
    
    if not target_dir.exists():
        raise FileNotFoundError(f"Target directory '{target_dir}' should already not exist.")
    
    shutil.copytree(source_dir, target_dir, 
                    ignore=shutil.ignore_patterns('__pycache__', '*.pyc', '*.pyo', '*.pyd', '*.in'), 
                    dirs_exist_ok=True)
    print(f"Copied iode package from '{source_dir}' to '{target_dir}'.")


if __name__ == "__main__":
    source_directory = Path(__file__).parent / "pyiode" / "iode"
    target_directory = Path(iode.__file__).parent
    copy_iode_package(source_directory, target_directory)
    print("Update complete.")
