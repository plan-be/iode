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
    command = ['pyside6-uic', '-o', py_file, ui_file, '--absolute-imports']
    print(' '.join(command))
    process = subprocess.run(command, shell=True, capture_output=True, check=True)
    process.check_returncode()

# Run the PyRCC6 command
resource_filename = "iode_resource"
qrc_resource_file = current_directory / 'iode_gui' / 'resources' / f'{resource_filename}.qrc'
if not qrc_resource_file.exists():
    raise FileNotFoundError(f"Resource file not found: {qrc_resource_file}")
qrc_resource_file = str(qrc_resource_file.resolve())

py_resource_file = current_directory / 'iode_gui' / f'{resource_filename}_rc.py'
py_resource_file = str(py_resource_file.resolve())

command = ['pyside6-rcc', '-o', py_resource_file, qrc_resource_file]
print(' '.join(command))
process = subprocess.run(command, shell=True, capture_output=True, check=True)
process.check_returncode()
