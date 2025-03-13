import re
import ast
from shutil import copyfile
from pathlib import Path
from typing import Union, Set, List, Tuple
from Cython.Compiler.TreeFragment import parse_from_strings
from Cython.Compiler.ExprNodes import ConstNode, AttributeNode, NameNode, StringNode, NoneNode
from Cython.Compiler.Nodes import (Node, DefNode, ClassDefNode, PyClassDefNode, CClassDefNode, 
                                   StatListNode, CImportStatNode, FromImportStatNode, FromCImportStatNode, 
                                   CFuncDefNode, CVarDefNode, CArgDeclNode, DecoratorNode, 
                                   SingleAssignmentNode, IfStatNode, TryExceptStatNode)

pyiode_dir = Path(__file__).parent
cython_reserved_keywords = {'cdef', 'cpdef', '&', 'dereference', '@cython'}


# ---- Parse cython pyx file ----

class CythonParser:
    def __init__(self, name: str, file_path: Path):
        self.name = name
        self.file_path = file_path
        self.source_code = []
        self.python_code = []
        self.pyx_code = []
        self.import_from_cython = set()
        self.cython_objs_or_keywords = cython_reserved_keywords
    
    def parse(self):
        # Parse the source code into an AST using the Cython parser
        with open(self.file_path, 'r') as file:
            self.source_code = file.readlines()
        
        tree: StatListNode = parse_from_strings(self.name, ''.join(self.source_code))
        self._parse_list_node(tree.body)

        # Add import from cython file
        if self.import_from_cython:
            last_import_index = 0
            for i, line in enumerate(self.python_code):
                if line.startswith('import') or line.startswith('from'):
                    last_import_index = i
            self.import_from_cython = ', '.join(sorted(list(self.import_from_cython)))
            self.import_from_cython = f"from iode.iode_cython import {self.import_from_cython}\n"
            self.python_code.insert(last_import_index+1, self.import_from_cython)

        print()
        print('cython objs or keywords:', self.cython_objs_or_keywords)

    def cleanup_pyx_file(self):
        with open(self.file_path, 'w') as file:
            file.writelines(self.pyx_code)

    def write_to_python_file(self, file_path: Path):
        with open(file_path, 'w') as file:
            file.writelines(self.python_code)

    def _parse_list_node(self, list_node: StatListNode):
        for node in list_node.stats:
            if isinstance(node, StatListNode):
                # nested list node
                self._parse_list_node(node)
            elif isinstance(node, FromImportStatNode):
                import_code = self._parse_from_import_node(node)
                self.pyx_code.extend(import_code)
                self.python_code.extend(import_code)
                print('import_code:', import_code)
            elif isinstance(node, CImportStatNode):
                cimport_code = self._get_pyx_code_from_node(node)
                self.pyx_code.extend(cimport_code)
                print('cimport_code:', cimport_code)
            elif isinstance(node, FromCImportStatNode):
                cimport_code = self._parse_from_cimport_node(node)
                self.pyx_code.extend(cimport_code)
                print('cimport_code:', cimport_code)
            elif isinstance(node, ClassDefNode):
                if self.python_code[-1] != '\n':
                    self.python_code.append('\n')
                pyx_code, py_code = self._parse_class_node(node)
                self.pyx_code.extend(pyx_code)
                self.python_code.extend(py_code)
            elif isinstance(node, DefNode):
                if self.python_code[-1] != '\n':
                    self.python_code.append('\n')
                pyx_code, py_code = self._parse_def_node(node)
                self.pyx_code.extend(pyx_code)
                self.python_code.extend(py_code)
            elif isinstance(node, (IfStatNode, TryExceptStatNode)):
                node_code = self._get_pyx_code_from_node(node)
                if any('import' in line for line in node_code):
                    self.pyx_code.extend(node_code)
                    self.python_code.extend(node_code)
                    for line in node_code:
                        print('import_code:', [line])
            else:
                print(f'{type(node).__name__}:', node.pos[1])
                pyx_code, py_code = self._extract_cython_and_python_code(node)
                self.pyx_code.extend(pyx_code)
                self.python_code.extend(py_code)

    def _get_pyx_code_from_node(self, node: Node) -> List[str]:
        if not node.pos:
            return ''
        _, start_line, start_col = node.pos
        _, end_line, end_col = node.end_pos()
        return self.source_code[start_line-1:end_line]

    def _extract_cython_and_python_code(self, node: Node) -> Tuple[List[str], List[str]]:
        pyx_code = []
        py_code = []
        node_code = self._get_pyx_code_from_node(node)
        for line in node_code:
            if line.strip() == '' or line.strip().startswith('#'):
                pyx_code.append(line)
                py_code.append(line)
            elif line.strip().startswith('import '):
                pyx_code.append(line)
                py_code.append(line)
                print('import_code:', [line])
            elif self.is_pure_python(line):
                py_code.append(line)
            else:
                pyx_code.append(line)
        return pyx_code, py_code

    def _parse_from_import_node(self, import_node: FromImportStatNode) -> List[str]:
        return self._get_pyx_code_from_node(import_node)

    def _parse_from_cimport_node(self, import_node: FromCImportStatNode) -> List[str]:
        # imported_names  [(pos, name, as_name)]  Names to be imported
        names = {item[2] if item[2] is not None else item[1] 
                 for item in import_node.imported_names}
        self.cython_objs_or_keywords |= names
        return self._get_pyx_code_from_node(import_node)

    def _parse_decorator(self, decorator_nodes: List[DecoratorNode], indent: str) -> List[str]:
        if not decorator_nodes:
            return []
        
        list_decorators = []
        for decorator_node in decorator_nodes:
            decorator = decorator_node.decorator
            if isinstance(decorator, NameNode):
                list_decorators.append(f'{indent}@{decorator.name}\n')
            elif isinstance(decorator, AttributeNode):
                name = decorator.obj.name
                str_decorator = f'{indent}@{name}'
                if decorator.attribute:
                    str_decorator += f'.{decorator.attribute}\n'
                list_decorators.append(str_decorator)
            else:
                raise NotImplementedError(f'Decorators: type {type(decorator).__name__} is not supported yet')
        return list_decorators

    def _parse_func_arg(self, arg_node: CArgDeclNode) -> Tuple[str, str]:
        if arg_node.is_self_arg:
            return 'self', 'self'
        arg_name = arg_node.declarator.name     # CDeclaratorNode
        base_type = arg_node.base_type.name     # CBaseTypeNode
        default = arg_node.default              # ExprNode or None
        annotation = arg_node.annotation        # AnnotationNode or None
        arg = arg_name
        if annotation is not None:
            annotation = annotation.string.value
            arg += f': {annotation}'
        if default is not None:
            unparsed_default = default
            if isinstance(default, StringNode):
                default = f"'{default.value}'"
            elif isinstance(default, NoneNode):
                default = 'None'
            elif isinstance(default, ConstNode):
                default = default.value
            else:
                raise NotImplementedError(f'Default argument: type {type(default).__name__} is not supported yet')
            arg += f'={default}'
        return arg_name, arg

    def _get_docstring(self, doc: str, indent: str) -> List[str]:
        if doc is None:
            return []
        doc = doc.split('\n')
        if not doc[0].strip():
            doc = doc[1:]
        if not doc[-1].strip():
            doc = doc[:-1]
        docstring = [f'{indent}    r"""\n']
        docstring += [f'{line}\n' for line in doc]
        docstring += [f'{indent}    """\n']
        return docstring

    def is_pure_python(self, code: str) -> bool:
        return not any(marker in code for marker in self.cython_objs_or_keywords)

    def _parse_def_node(self, def_node: DefNode, class_name: str = None, super_class: str = None) -> Tuple[List[str], List[str]]:
        func_name = def_node.name
        pyx_func_code = []
        py_func_code = []

        start_line, end_line = def_node.pos[1], def_node.end_pos()[1]
        first_line = self.source_code[start_line-1]
        indent = ' ' * (len(first_line) - len(first_line.lstrip())) 
        print(f'{indent}function:', func_name, ' - lines ', start_line, ' -> ', end_line)
 
        # ---- function decorator(s) ----
        decorators = self._parse_decorator(def_node.decorators, indent)
        if decorators:
            is_property = any('@property' in decorator for decorator in decorators)
            is_setter = any('.setter' in decorator for decorator in decorators)
            py_func_code.extend(decorators)
            if is_property or is_setter:
                decorators = [decorator for decorator in decorators 
                              if '@property' not in decorator and '.setter' not in decorator]
            pyx_func_code.extend(decorators)
        else:
            is_property = False
            is_setter = False

        # ---- function signature ----
        func_name = def_node.name
        # arguments -> list(CArgDeclNode)
        args = dict()
        for arg in def_node.args:
            arg_name, arg_definition = self._parse_func_arg(arg)
            args[arg_name] = arg_definition
        arg_defs = ', '.join(args.values())
        # return_type_annotation -> AnnotationNode or None
        return_type_annotation = def_node.return_type_annotation
        
        if func_name not in ['__cinit__', '__dealloc__']:
            py_func_signature = f'{indent}def {func_name}({arg_defs})'
            if return_type_annotation is not None:
                py_func_signature += f' -> {return_type_annotation.string.value}'
            py_func_signature += ':\n'
            if class_name is not None and class_name in py_func_signature:
                py_func_signature = py_func_signature.replace(class_name, 'Self')
            py_func_code.append(py_func_signature)
        
        if is_property:
            pyx_func_name = f'get_{func_name}'
        elif is_setter:
            pyx_func_name = f'set_{func_name}'
        else:
            pyx_func_name = func_name 
        pyx_func_signature = f'{indent}def {pyx_func_name}({arg_defs})'
        if return_type_annotation is not None:
            pyx_func_signature += f' -> {return_type_annotation.string.value}'
        pyx_func_signature += ':\n'
        pyx_func_code.append(pyx_func_signature)

        # ---- function docstring ----
        # NOTE: skip docstring in pyx file, use docstring in py file
        doc = def_node.doc
        docstring = self._get_docstring(doc, indent)
        if docstring:
            py_func_code.extend(docstring)

        # ---- function body ----
        body_node = def_node.body
        body_code = self._get_pyx_code_from_node(body_node)
        # remove docstring from body_code
        if docstring:
            body_code = body_code[len(docstring):]
        # dump body code to pyx file and/or py file
        if func_name in ['__cinit__', '__dealloc__']:
            print(f'{indent}    cython special method -> keep as it in .pyx file')
            pyx_func_code.extend(body_code)
        elif self.is_pure_python(''.join(body_code)):
            print(f'{indent}    pure python function -> dumped as it in .py file / skipped in .pyx file')
            py_func_code.extend(body_code)
            pyx_func_code = []
        else:
            print(f'{indent}    not pure python function -> calling cython function in .py file / keep as it in .pyx file')
            pyx_func_code.extend(body_code)
            list_args = ', '.join(args.keys())
            maybe_return = 'return ' if return_type_annotation is not None else ''
            if super_class is not None:
                py_func_code.append(f'{indent}    {maybe_return}{super_class}.{pyx_func_name}({list_args})\n')
            else:
                py_func_code.append(f'{indent}    {maybe_return}{pyx_func_name}({list_args})\n')
                self.import_from_cython.add(pyx_func_name)
        
        if len(pyx_func_code) > 0:
            pyx_func_code.append('\n')
        if len(py_func_code) > 0:
            py_func_code.append('\n')
        return pyx_func_code, py_func_code

    def _parse_class_node(self, class_node: ClassDefNode) -> Tuple[List[str], List[str]]:
        class_name = class_node.class_name if isinstance(class_node, CClassDefNode) else class_node.target.name
        pyx_class_code = []
        py_class_code = []

        start_line, end_line = class_node.pos[1], class_node.end_pos()[1]
        prefix = 'cdef class' if isinstance(class_node, CClassDefNode) else 'class'
        first_line = self.source_code[start_line-1]
        indent = ' ' * (len(first_line) - len(first_line.lstrip())) 
        print(f'{indent}{prefix}:', class_name, ' - lines ', start_line, ' -> ', end_line)

        # ---- class decorator(s) ----
        decorators = self._parse_decorator(class_node.decorators, indent)
        if decorators:
            pyx_class_code.extend(decorators)
            decorators = [decorator for decorator in decorators if '@cython' not in decorator]
            py_class_code.extend(decorators)

        # ---- class signature ----
        py_class_signature = f'{indent}class {class_name}'
        if isinstance(class_node, CClassDefNode):
            pyx_class_signature = f'{indent}cdef class {class_name}'
        else:
            pyx_class_signature = py_class_signature
        
        pyx_base_classes = ''
        _bases = class_node.bases                           # ExprNodes.TupleNode or None
        if _bases is not None:
            if len(_bases.args):                            # [NameNode]
                pyx_base_classes = ', '.join(base_class.name for base_class in _bases.args)
        super_class = None
        if isinstance(class_node, CClassDefNode):
            super_class = f'Cython{class_name}'
            self.import_from_cython.add(f'{class_name} as {super_class}')
            py_base_classes = super_class
            if pyx_base_classes:
                py_base_classes += f', {pyx_base_classes}'
        else:
            py_base_classes = pyx_base_classes
        if len(pyx_base_classes):
            pyx_class_signature += f'({pyx_base_classes})'
        if len(py_base_classes):
            py_class_signature += f'({py_base_classes})'
        
        pyx_class_signature += ':\n'
        pyx_class_code.append(pyx_class_signature)

        py_class_signature += ':\n'
        py_class_code.append(py_class_signature)             

        # ---- class docstring ----
        # NOTE: skip docstring in pyx file
        doc = []
        if isinstance(class_node, ClassDefNode):
            doc = class_node.doc
        else:
            doc = class_node.doc_node
            if doc is not None:
                doc = '\n'.join(self._get_pyx_code_from_node(doc))
        docstring = self._get_docstring(doc, indent)
        if docstring:
            py_class_code.extend(docstring)

        # ---- class body ----
        pyx_body_code = []
        py_body_code = []
        for node in class_node.body.stats:
            if isinstance(node, DefNode):
                pyx_method_code, py_method_code = self._parse_def_node(node, class_name, super_class)
                # NOTE: add an empty line between the class variables and the first method
                if len(pyx_method_code) and len(pyx_body_code) and pyx_body_code[-1].strip() != '':
                    pyx_body_code.append('\n')
                pyx_body_code.extend(pyx_method_code)
                # NOTE: add an empty line between the class variables and the first method
                if len(py_method_code) and len(py_body_code) and py_body_code[-1].strip() != '':
                    py_body_code.append('\n')
                py_body_code.extend(py_method_code)
            elif isinstance(node, CVarDefNode):
                # C variable definition or forward/extern function declaration
                for declarator in node.declarators:
                    self.cython_objs_or_keywords.add(declarator.declared_name())
                    print(f'{indent}    C variable definition:', declarator.declared_name())
                pyx_code = self._get_pyx_code_from_node(node)
                pyx_body_code.extend(pyx_code)
            elif isinstance(node, CFuncDefNode):
                # C function definition
                print(f'{indent}    C function definition:', node.declared_name())
                print(f'{indent}        not pure python function -> keep as it in .pyx file')
                self.cython_objs_or_keywords.add(node.declared_name())
                decorators = self._parse_decorator(node.decorators, indent + '    ')
                if decorators:
                    pyx_body_code.extend(decorators)
                pyx_code = self._get_pyx_code_from_node(node) + ['\n']
                pyx_body_code.extend(pyx_code)
            else:
                print(f'{indent}    {type(node).__name__}', node.pos[1])
                pyx_code, py_code = self._extract_cython_and_python_code(node)
                pyx_body_code.extend(pyx_code)
                py_body_code.extend(py_code)
        
        # ---- finish class ----
        if not len(pyx_body_code):
            pyx_class_code = []
        else:
            pyx_class_code.extend(pyx_body_code)
            pyx_class_code.append('\n')
            # NOTE: add an empty line between the (c)import statements and the class definition
            if len(pyx_class_code) and len(self.pyx_code) and self.pyx_code[-1].strip() != '':
                pyx_class_code = ['\n'] + pyx_class_code
        
        if not len(py_body_code):
            py_class_code = []
        else:
            py_class_code.extend(py_body_code)
            py_class_code.append('\n')
            # NOTE: add an empty line between the (c)import statements and the class definition
            if len(py_class_code) and len(self.python_code) and self.python_code[-1].strip() != '':
                py_class_code = ['\n'] + py_class_code
        
        return pyx_class_code, py_class_code


if __name__ == "__main__":
    stem = "variables_database"
    tmp_file = 'source_code.py'
    file_path_py = pyiode_dir / "iode" / "iode_database" / f"{stem}.py"
    file_path_cython = pyiode_dir / "iode_database" / f"{stem}.pyx"

    copyfile(file_path_cython, file_path_cython.with_suffix('.pyx.bak'))

    cython_parser = CythonParser(stem, file_path_cython)
    cython_parser.parse()
    cython_parser.cleanup_pyx_file()
    cython_parser.write_to_python_file(file_path_py)
    
    
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
