import os
import subprocess
from pathlib import Path


# current directory
current_directory = Path(__file__).parent.resolve()

# Convert all <name>.ui files to ui_<name>.py files
for ui_file in current_directory.glob("**/*.ui"):
    # Get the path of the output .py file
    py_file = ui_file.with_suffix('.py')
    py_file = py_file.with_name(f'ui_{py_file.name}')

    ui_file = str(ui_file.resolve())
    py_file = str(py_file.resolve())

    # Run the PyUIC6 command
    command = ['pyuic6', '-o', py_file, ui_file]
    print(' '.join(command))
    process = subprocess.run(command, shell=True, capture_output=True, check=True)
    process.check_returncode()

    # replace the line "from PyQt6(.xxx) import yyyy" with 
    # "from qtpy(.xxx) import yyy" in the generated .py file
    with open(py_file, 'r', encoding='utf-8') as f:
        content = f.read()
    content = content.replace("from PyQt6", "from qtpy")
    with open(py_file, 'w', encoding='utf-8') as f:
        f.write(content)
