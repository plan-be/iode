from pathlib import Path
import shutil
import argparse


root_dir = Path(__file__).parent
pyiode_dist_dir = root_dir / "pyiode" / "dist"
gui_dist_dir = root_dir / "gui" / "dist"

# create a patched version of the wheel file 
def create_patched_version(wheel_path: Path, version: str, path_number: int):
    print(f"Processing wheel file: {wheel_path}")

    # create a new wheel file name with the version and path number
    prefix, suffix = wheel_path.stem.split(f"-{version}-")
    new_wheel_stem = f"{prefix}-{version}-{path_number}-{suffix}"
    new_wheel_path = wheel_path.with_stem(new_wheel_stem)

    # copy the original wheel file to the new location with the new name
    shutil.copy(wheel_path, new_wheel_path)
    
    print(f"Created patched wheel: {new_wheel_path}")

def pyiode_patch_version(version: str, path_number: int):
    for wheel_file in pyiode_dist_dir.glob(f"iode-{version}-*.whl"):
        create_patched_version(wheel_file, version, path_number)

def gui_patch_version(version: str, path_number: int):
    for wheel_file in gui_dist_dir.glob(f"iode_gui-{version}-*.whl"):
        create_patched_version(wheel_file, version, path_number)


if __name__ == "__main__":
    # parse the version and path number from the command line arguments
    parser = argparse.ArgumentParser(description="Create patched wheel files for a " \
                                    "specific version and path number.")
    parser.add_argument("folder", type=str, choices=["pyiode", "gui"], help="The folder " \
                        "to patch: 'pyiode' or 'gui'.")
    parser.add_argument("version", type=str, help="The version of the wheel files to " \
                        "patch (e.g., 7.0.6).")
    parser.add_argument("path_number", type=int, help="The path number to include in " \
                        "the patched wheel file name (e.g., 1).")
    args = parser.parse_args()

    if args.folder == "pyiode":
        pyiode_patch_version(args.version, args.path_number)
    elif args.folder == "gui":
        gui_patch_version(args.version, args.path_number)
    else:
        print("Invalid folder name. Please specify 'pyiode' or 'gui'.")
