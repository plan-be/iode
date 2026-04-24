from pathlib import Path

def compare_debug_files(file1: Path, file2: Path) -> bool:
    success = True
    with file1.open() as f1, file2.open() as f2:
        lines1 = f1.readlines()
        lines2 = f2.readlines()
        
        if len(lines1) != len(lines2):
            print(f"Files have different number of lines: {len(lines1)} vs {len(lines2)}")
            return False
        
        for i, (line1, line2) in enumerate(zip(lines1, lines2)):
            if line1 != line2:
                print(f"Difference at line {i+1}:\nFile 1: {line1}\nFile 2: {line2}")
                success = False
                
    return success


if __name__ == "__main__":
    dir_1 = Path("C:/usr/Projects/iode-1")
    dir_2 = Path("C:/usr/Projects/iode_copy")

    file_1 = dir_1 / "debug.txt"
    file_2 = dir_2 / "debug.txt"
    
    if compare_debug_files(file_1, file_2):
        print("Files are identical.")
    else:
        print("Files differ.")
