import re
import ast
from shutil import copyfile
from pathlib import Path
from typing import Union, Set, List
from Cython.Compiler.TreeFragment import parse_from_strings
from Cython.Compiler.ExprNodes import ConstNode
from Cython.Compiler.Nodes import (Node, DefNode, PyClassDefNode, CClassDefNode, StatListNode, 
                                   FromImportStatNode, FromCImportStatNode, CVarDefNode, 
                                   CArgDeclNode, SingleAssignmentNode, IfStatNode)


tag_cdef_func = 'tagged_function'
cython_reserved_keywords = {'cdef', 'cpdef', '&', '@cython'}

_func_signature_pattern = re.compile(r'^\s*(def|cdef|cpdef)\s+[a-zA-Z_][\w\s]*\(.*$')
c_cast_pattern = re.compile(r'<(\w+)>')

def is_function_signature(line: str) -> bool:
    return _func_signature_pattern.match(line) is not None

def contains_cython_cast(line: str) -> bool:
    return line.strip() != '<BLANKLINE>' and c_cast_pattern.search(line) is not None


# ---- cython files ----

def cython_remove_docstrings(file_path: Path):
    content = []
    docstring = False
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            if '"""' in line:
                docstring = not docstring
            if not docstring and '"""' not in line:
                content.append(line)
    with open(file_path, 'w') as file:
        file.write(''.join(content))


def cython_rename_class_property(file_path: Path):
    content = []
    class_names = dict()
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for i, line in enumerate(lines):
            # rename classes
            if line.startswith('cdef class ') and not line.startswith('cdef class Cython'):
                py_class_name = line.split('class')[1].split('(')[0].strip()
                class_names[py_class_name] = 'Cython' + py_class_name
            # rename properties
            new_func = is_function_signature(line)
            if new_func:
                _, signature = line.split('def ')
                function_name, _ = signature.split('(')
                # if getter
                if lines[i - 1].strip() == '@property':
                    content.pop()
                    line = line.replace(function_name, 'get_' + function_name)
                # if setter
                if '@' in lines[i - 1] and '.setter' in lines[i - 1]:
                    content.pop()
                    line = line.replace(function_name, 'set_' + function_name)
            content.append(line)
    content = ''.join(content)
    for py_class_name, cython_class_name in class_names.items():
        content = re.sub(rf'\b{py_class_name}\b', cython_class_name, content)
        content = content.replace(f'cimport {cython_class_name}', f'cimport {py_class_name}')
    with open(file_path, 'w') as file:
        file.write(content)


def get_list_cimport_items(file_path: Path) -> Set[str]:
    cimport_items = set()
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            if 'cimport' in line:
                items = line.split('cimport')[1].strip()
                if 'as' in items:
                    item = items.split('as')[1].strip()
                    cimport_items.add(item)
                else:
                    for item in items.split(','):
                        cimport_items.add(item.strip())
    return cimport_items


# ---- Python scripts ----

def rename_cython_class_to_py_class(file_path: Path):
    content = []
    cython_class_name = None
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            if 'cdef class' in line:
                py_class_name = line.split(' ')[2].split('(')[0]
                cython_class_name = 'Cython' + py_class_name
                line = f"class {py_class_name}({cython_class_name}):\n"
            else:
                if cython_class_name is not None:
                    line = line.replace(cython_class_name, py_class_name)
            content.append(line)
    with open(file_path, 'w') as file:
        file.write(''.join(content))


def tagging_cdef_func(file_path: Path):
    content = []
    cython_func_pattern = re.compile(r'^\s*(cdef|cpdef)\s+\w+\s+\w+\(.*$')
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            if cython_func_pattern.match(line):
                indent = ' ' * (len(line) - len(line.lstrip()))
                line = indent + f'def {tag_cdef_func}(self):\n'
            content.append(line)
    with open(file_path, 'w') as file:
        file.write(''.join(content))


def remove_cython_code(file_path: Path):
    content = []
    with open(file_path, 'r') as file:
        docstring = False
        lines = file.readlines()
        for line in lines:
            if '"""' in line:
                docstring = not docstring
            if docstring or line.strip().startswith('#') or is_function_signature(line):
                pass
            elif any(item in line for item in cython_items) or contains_cython_cast(line):
                print(f"skipped line '{line.strip()}'")
                indent = ' ' * (len(line) - len(line.lstrip()))
                line = indent + 'pass # << cython code >> \n'
            content.append(line)
    with open(file_path, 'w') as file:
        file.write(''.join(content))

# ---- Parse cython pyx file ----

class CythonParser:
    def __init__(self, name: str, file_path: Path):
        self.name = name
        self.file_path = file_path
        self.source_code = []
        self.python_code = []
        self.cimports = set()
    
    def parse(self):
        # Parse the source code into an AST using the Cython parser
        with open(self.file_path, 'r') as file:
            self.source_code = file.readlines()
        
        tree: StatListNode = parse_from_strings(self.name, ''.join(self.source_code))
        for stat_node in tree.body.stats:
            if isinstance(stat_node, FromImportStatNode):
                self._parse_from_import_node(stat_node)
            elif isinstance(stat_node, FromCImportStatNode):
                self._parse_from_cimport_node(stat_node)
            elif isinstance(stat_node, PyClassDefNode):
                self._parse_class_node(stat_node)
            elif isinstance(stat_node, CClassDefNode):
                # self._parse_cdef_class_node(stat_node)
                pass
            elif isinstance(stat_node, DefNode):
                self._parse_def_node(stat_node)
                pass
            else:
                print(type(stat_node).__name__, stat_node.pos[1:])
        
        print()
        print('cimports:', self.cimports)
        print()
        print('---------- python code ----------')
        print(''.join(self.python_code))

    def _get_source_code(self, node: Node) -> Union[str, List[str]]:
        if not node.pos:
            return ''
        _, start_line, start_col = node.pos
        _, end_line, end_col = node.end_pos()
        return self.source_code[start_line-1:end_line]

    def _parse_from_import_node(self, import_node: FromImportStatNode):
        code = self._get_source_code(import_node)
        self.python_code.extend(code)

    def _parse_from_cimport_node(self, import_node: FromCImportStatNode):
        # imported_names  [(pos, name, as_name)]  Names to be imported
        names = {item[2] if item[2] is not None else item[1] 
                 for item in import_node.imported_names}
        self.cimports |= names

    def _parse_func_arg(self, arg_node: CArgDeclNode) -> str:
        if arg_node.is_self_arg:
            return 'self'
        arg_name = arg_node.declarator.name     # CDeclaratorNode
        base_type = arg_node.base_type.name     # CBaseTypeNode
        default = arg_node.default              # ExprNode or None
        annotation = arg_node.annotation        # AnnotationNode or None
        arg = arg_name
        if annotation is not None:
            annotation = annotation.string.value
            arg += f': {annotation}'
        if default is not None:
            default = default.value if isinstance(default, ConstNode) else str(default.constant_result)
            arg += f'={default}'
        return arg

    def is_pure_python(self, code: str) -> bool:
        cython_markers = cython_reserved_keywords | self.cimports
        return not any(marker in code for marker in cython_markers)

    def _parse_def_node(self, def_node: DefNode) -> str:
        func_name = def_node.name

        start_line, end_line = def_node.pos[1], def_node.end_pos()[1]
        print('function:', func_name, ' - lines ', start_line, ' -> ', end_line)
        self.python_code.append('\n')

        # ---- function docstring ----
        docstring = def_node.doc
        print('docstring:', docstring)

        # ---- function decorator(s) ----
        if def_node.decorators:
            for decorator in def_node.decorators:
                print('decorator:', decorator)
        
        # ---- function signature ----
        func_name = def_node.name
        return_type_annotation = def_node.return_type_annotation
        args = ', '.join(self._parse_func_arg(arg) for arg in def_node.args)
        func_signature = f'def {func_name}({args})'
        if return_type_annotation is not None:
            func_signature += f' -> {return_type_annotation}'
        func_signature += ':\n'
        self.python_code.append(func_signature)

        # ---- function body ----
        body_node = def_node.body
        if hasattr(body_node, 'stats'):
            node_types = {type(node).__name__ for node in body_node.stats}
        else:
            node_types = {type(body_node).__name__}
        print('\body -> node types:', node_types)
        body_code = self._get_source_code(body_node)
        if self.is_pure_python(''.join(body_code)):
            print('\tpure python function -> dumped as it')
            self.python_code.extend(body_code)
        else:
            print('\tnot pure python function')

    def _parse_class_node(self, class_node: PyClassDefNode):
        print('Python class:', class_node.name)
        print(class_node.child_attrs)
        for node in class_node.body.stats:
            if isinstance(node, DefNode):
                self._parse_def_node(node)
            else:
                print('\t', type(node).__name__, node.child_attrs)

    def _parse_cdef_class_node(self, class_node: CClassDefNode):
        print('Cython class')
        print(class_node.child_attrs)
        # for node in class_node.body.stats:
        #     print('\t', type(node).__name__, node.child_attrs)


if __name__ == "__main__":
    pyiode_dir = Path(__file__).parent
    stem = "variables_database"
    tmp_file = 'source_code.py'
    file_path_py = pyiode_dir / "iode" / "iode_database" / f"{stem}.py"
    file_path_cython = pyiode_dir / "iode_database" / f"{stem}.pyx"

    cython_parser = CythonParser(stem, file_path_cython)
    cython_parser.parse()

    """
    # ---- cython file -> remove docstrings and rename functions as __cython_...
    copyfile(file_path_cython, tmp_file)
    cython_remove_docstrings(file_path_cython)
    cython_rename_class_property(file_path_cython)

    # ---- python file -> call cython functions and remove cimports and c(p)defs
    cython_items = get_list_cimport_items(tmp_file)
    cython_items = cython_items.union({"cdef", "cpdef", "&", "@cython"})
    print('cython_items:', cython_items, '\n')

    if file_path_py.exists():
        file_path_py.unlink()
    copyfile(tmp_file, file_path_py)
    rename_cython_class_to_py_class(file_path_py)
    tagging_cdef_func(file_path_py)
    remove_cython_code(file_path_py)
    
    answer = input(f"Please check for possible errors in the python script "
                   f"{file_path_py.name} and press y/yes to continue")
    if answer.lower()[0] == 'y':
        pass
    
    """

    """
    c_cast_replacement = r'cast_\1'

    source_code = ''
    with open(file_path_py, 'r') as file:
        lines = file.readlines()
        for line in lines:
            cython_marker = None
            if line.strip().startswith('cdef'):
                cython_marker = 'cdef'
            if line.strip().startswith('cpdef'):
                cython_marker = 'cpdef'
            if cython_marker:
                if is_function_signature(line):
                    indent, signature = line.split(cython_marker)
                    signature = signature.strip()
                    returned_type_and_func_name, args = signature.split('(')
                    returned_type, function_name = returned_type_and_func_name.split()
                    line = indent + 'def cython_func_' + function_name + '(self):\n'
                else:
                    line = line.replace(cython_marker, '# ' + cython_marker)
            source_code += line
        source_code = source_code.replace('&', 'ampersand_')
        source_code = re.sub(c_cast_pattern, c_cast_replacement, source_code)

    with open('source_code.py', 'w') as file:
        file.write(source_code)

    tree = ast.parse(source_code)
    for node in ast.walk(tree):
        if isinstance(node, ast.FunctionDef):
            print("function name: ", node.name)
            print("function args: ", node.args)
            print("function body: ", node.body)"
    """
