from pathlib import Path

def change_c_to_cpp(directory: Path):
    for file in directory.iterdir():
        if file.is_file() and file.suffix == '.c':
            new_file = file.with_suffix('.cpp')
            file.rename(new_file)
            print(f"Renamed: {file} -> {new_file}")

# Example usage:
c_api_dir = Path(__file__).parent

subdir = 'lec'
change_c_to_cpp(c_api_dir / subdir)
