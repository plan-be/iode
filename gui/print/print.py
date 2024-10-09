from PyQt6.QtWidgets import QMessageBox
from PyQt6.QtGui import QTextDocument
from PyQt6.QtPrintSupport import QPrintPreviewDialog

import os
from iode import TableLang, IodePrint
from typing import List, Union, Optional


class IodeGuiPrint(IodePrint):
    """
    This class provides methods to set the print destination, number of decimals, language, and table files.
    """

    def __init__(self, nb_decimals: int, language: TableLang, gsample: Optional[str] = None, 
                 files: Optional[List[str]] = None) -> None:
        super().__init__(nb_decimals, language, gsample, files)

    def _print_tbl_or_var_to_file(self, is_table: bool, output_file: str, format: str, names: Union[str, List[str]]) -> None:
        """
        Prints tables or variables to a file.

        :param is_table: A boolean indicating whether to print tables or variables.
        :param output_file: The output file path.
        :param format: The output file format.
        :param names: The names of the tables or variables to print.
        """
        success: bool = self.print_tbl_or_var_to_file(is_table, output_file, format, names)
        if success:
            obj_type = "tables" if is_table else "variables"
            if not isinstance(names, str):
                names = ', '.join(names)
            QMessageBox.information(None, "SUCCESS", f"succeeded to export {obj_type} {names} to the file {output_file}")

    def _print_tbl_or_var(self, is_table: bool, dialog: QPrintPreviewDialog, document: QTextDocument, 
                          names: Union[str, List[str]]) -> None:
        """
        Print tables or variables.

        :param is_table: A boolean value indicating whether to print tables or variables.
        :param dialog: A QPrintPreviewDialog object.
        :param document: A QTextDocument object.
        :param names: A string containing the names.
        """
        try:
            output_file = self.print_tbl_or_var(self, is_table, names)

            document.clear()

            # Load the content of the generated HTML file and then removed the file
            with open(output_file, 'r') as file:
                document.setHtml(file.read())
            os.remove(output_file)

            # Print the document
            dialog.exec()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    def print_table_to_file(self, output_file: str, format: str, names: Union[str, List[str]]) -> None:
        """
        Print table to a file.

        Args:
            output_file (str): The output file path.
            format (str): The format of the output file.
            names (str): The names of the variables.

        Note:
            See C_ViewPrintGrTblVar() in sb_view.c from the old GUI.
        """
        self._print_tbl_or_var_to_file(True, output_file, format, names)

    def print_table(self, dialog: QPrintPreviewDialog, document: QTextDocument, names: Union[str, List[str]]) -> None:
        """
        Print table to a QTextDocument.

        Args:
            dialog (QPrintPreviewDialog): The print preview dialog.
            document (QTextDocument): The text document to print to.
            names (str): The names of the variables.

        Note:
            See C_ViewPrintGrTblVar() in sb_view.c from the old GUI.
        """
        self._print_tbl_or_var(True, dialog, document, names)

    def print_variable_to_file(self, output_file: str, format: str, names: Union[str, List[str]]) -> None:
        """
        Print variable to a file.

        Args:
            output_file (str): The output file path.
            format (str): The format of the output file.
            names (str): The names of the variables.

        Note:
            See C_ViewPrintGrTblVar() in sb_view.c from the old GUI.
        """
        self._print_tbl_or_var_to_file(False, output_file, format, names)

    def print_variable(self, dialog: QPrintPreviewDialog, document: QTextDocument, names: Union[str, List[str]]) -> None:
        """
        Print variable to a QTextDocument.

        Args:
            dialog (QPrintPreviewDialog): The print preview dialog.
            document (QTextDocument): The text document to print to.
            names (str): The names of the variables.

        Note:
            See C_ViewPrintGrTblVar() in sb_view.c from the old GUI.
        """
        self._print_tbl_or_var(False, dialog, document, names)
