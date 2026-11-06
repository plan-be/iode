import shutil
from pathlib import Path


def check_replace_allowed(src_path: Path, dst_path: Path):
    """
    Check that the destination (if it exists) can be replaced by the source.
    Replacing a directory by a file or a file by a directory is not allowed.

    :param src_path: path of the file or directory to move or copy.
    :param dst_path: path of the destination file or directory.
    :raises ValueError: if a directory would be replaced by a file or a file by a directory.
    """
    if dst_path.exists() and src_path.is_dir() != dst_path.is_dir():
        src_kind = "directory" if src_path.is_dir() else "file"
        dst_kind = "directory" if dst_path.is_dir() else "file"
        raise ValueError(f"Cannot replace the {dst_kind} '{dst_path}' by the {src_kind} '{src_path}'")


def move_file_or_dir(src_path: Path, dst_path: Path):
    """
    Move (or rename) a file or a directory. If the destination already exists, it is overwritten.

    If both the source and the destination are directories, the content of the source
    directory is merged into the destination directory (same behavior as when copying
    a directory with shutil.copytree(..., dirs_exist_ok=True)).

    Replacing a directory by a file or a file by a directory is not allowed (raises a ValueError).

    :param src_path: path of the file or directory to move.
    :param dst_path: new path of the file or directory.
    """
    src_path = src_path.resolve()
    dst_path = dst_path.resolve()
    # moving an item onto itself -> nothing to do
    if src_path == dst_path:
        return
    if src_path.is_dir() and src_path in dst_path.parents:
        raise ValueError(f"Cannot move the directory '{src_path}' into one of its own sub-directories")

    # do not replace a directory by a file or a file by a directory
    check_replace_allowed(src_path, dst_path)

    if dst_path.exists() and src_path.is_dir() and dst_path.is_dir():
        # merge the content of the source directory into the destination directory
        shutil.copytree(src_path, dst_path, dirs_exist_ok=True)
        shutil.rmtree(src_path)
        return

    # Rename this file or directory to the given target. If target points to an
    # existing file or empty directory, it will be unconditionally replaced.
    # https://docs.python.org/3.14/library/pathlib.html#pathlib.Path.replace
    src_path.replace(dst_path)
