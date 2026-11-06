from pathlib import Path
from typing import List
import random
import shutil

data_dir = Path(__file__).absolute().parent / 'data'
test_dir = data_dir / 'test_delete'


def generate_dirs_and_files() -> List[Path]:
    """
    Create a random number of sub-folders, sub-sub-folders and files 
    inside 'test_delete'.

    Returns the list of created folders and files.
    """
    # create the sub-folder 'test_delete' if it does not exists
    test_dir.mkdir(parents=True, exist_ok=True)

    # create sub-folders, sub-sub-folders and files inside 'test_delete'
    created_paths: List[Path] = []
    for i in range(1, 5):
        sub_dir = test_dir / f"sub_folder_{i}"
        sub_dir.mkdir(exist_ok=True)
        created_paths.append(sub_dir)
        for k in range(3):
            file = sub_dir / f"file_{i}_{k}.txt"
            file.write_text(f"content of file {i}_{k}")
            created_paths.append(file)
        for j in range(0, 3):
            sub_sub_dir = sub_dir / f"sub_sub_folder_{j}"
            sub_sub_dir.mkdir(exist_ok=True)
            created_paths.append(sub_sub_dir)
            for k in range(3):
                file = sub_sub_dir / f"file_{i}_{j}_{k}.txt"
                file.write_text(f"content of file {i}_{j}_{k}")
                created_paths.append(file)
    return created_paths


def cleanup():
    """
    Delete the folder 'test_delete' and everything inside.
    """
    # delete the folder 'test_delete' and everything inside.
    if test_dir.exists():
        shutil.rmtree(test_dir)


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Generate or delete the folder 'test_delete' "
                                                 "(with sub-folders and files) in the 'data' directory.")
    parser.add_argument("action", nargs="?", default="generate", choices=["generate", "cleanup"],
                        help="'generate' to create the folders and files, 'cleanup' to delete them "
                             "(default: 'generate')")
    args = parser.parse_args()

    print("data directory:", str(data_dir))
    if args.action == "generate":
        created_paths = generate_dirs_and_files()
        print(f"{len(created_paths)} folders and files created in '{test_dir}'")
    else:
        cleanup()
        print(f"'{test_dir}' deleted")
