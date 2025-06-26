from pathlib import Path

def convert_oem_to_utf8(input_path, output_path, oem_encoding="cp850"):
    """
    Convert a file from OEM code page (e.g., MS-DOS cp850) to UTF-8.

    Parameters
    ----------
    input_path : str
        Path to the input file encoded in OEM code page.
    output_path : str
        Path to the output file to be written in UTF-8.
    oem_encoding : str, optional
        The OEM code page encoding (default is 'cp850').
    """
    with open(input_path, "r", encoding=oem_encoding) as fin:
        content = fin.read()
    print(f"Converting {input_path} from {oem_encoding} to UTF-8...")
    with open(output_path, "w", encoding="utf-8") as fout:
        fout.write(content)


current_dir = Path(__file__).parent
for file in current_dir.glob("**/*.h"):
    convert_oem_to_utf8(file.name, file.name)
for file in current_dir.glob("**/*.c"):
    convert_oem_to_utf8(file.name, file.name)
