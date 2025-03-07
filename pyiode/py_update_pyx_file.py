import re
from pathlib import Path

_func_signature_pattern = re.compile(r'^\s*(def|cdef|cpdef)\s+[a-zA-Z_]\w*\s*\(.*$')


def is_func_signature(line):
    return _func_signature_pattern.match(line) is not None


def remove_docstrings_from_pyx_file(file_path: Path):
    content = []
    docstring = False
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            if '"""' in line:
                docstring = not docstring
            print("is docstring: ", docstring, " -> line: ", line)
            if not docstring and '"""' not in line:
                content.append(line)
    with open(file_path, 'w') as file:
        file.write(''.join(content))


def remove_cython_imports(file_path: Path):
    content = []
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            if not 'cimport' in line:
                content.append(line)
    with open(file_path, 'w') as file:
        file.write(''.join(content))


def remove_cython_funcs(file_path: Path):
    content = []
    with open(file_path, 'r') as file:
        lines = file.readlines()
        func_content = []
        cython_func = False
        for line in lines:
            new_func = is_func_signature(line)
            if new_func:
                property = func_content.pop() if len(func_content) and '@' in func_content[-1] else None
                if not cython_func:
                    content.extend(func_content)
                func_content = []
                if property:
                    content.append(property)
                cython_func = False
            func_content.append(line)
            if not new_func and ('cdef' in line or 'cpdef' in line):
                cython_func = True
    with open(file_path, 'w') as file:
        file.write(''.join(content))


if __name__ == "__main__":
    current_dir = Path(__file__).parent
    file_path = current_dir / "iode" / "iode_database" / "variables_database.py"
    remove_cython_imports(file_path)
    remove_cython_funcs(file_path)
