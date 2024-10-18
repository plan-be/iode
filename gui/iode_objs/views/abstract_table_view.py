from delegates.base_delegate import BaseDelegate    

from PySide6.QtCore import Qt, Signal
from PySide6.QtWidgets import QTableView, QLineEdit
from PySide6.QtGui import QTextDocument, QShortcut
from PySide6.QtPrintSupport import QPrinter

from iode import IodeTypes


class IodeAbstractTableView(QTableView):
    """
    This class represents a table view for Iode database objects.
    """

    databaseModified = Signal()
    showObjsRequest = Signal(str, list)

    def __init__(self, iode_type: IodeTypes, delegate: BaseDelegate, parent=None):
        """
        Constructor for IodeAbstractTableView.

        :param iode_type: The type of the Iode database.
        :param delegate: The delegate for the table view.
        :param parent: The parent widget.
        """
        super().__init__(parent)

        self.iode_type: IodeTypes = iode_type
        self.delegate: BaseDelegate = delegate
        self.filter_line_edit: QLineEdit = None

        self.section_edited: int = None
        self.object_name_edit: QLineEdit = None

        self.printer = QPrinter()
        self.document = QTextDocument()

        self.delete_shortcut = QShortcut(self)

        self.shortcut_edit_enter = QShortcut(self)
        self.shortcut_edit_return = QShortcut(self)

        self.same_cmt_shortcut = QShortcut(self)
        self.same_eq_shortcut = QShortcut(self)
        self.same_idt_shortcut = QShortcut(self)
        self.same_lst_shortcut = QShortcut(self)
        self.same_tbl_shortcut = QShortcut(self)
        self.vars_from_clec_shortcut = QShortcut(self)
        self.scalars_from_clec_shortcut = QShortcut(self)

        self.related_cmt_shortcut = QShortcut(self)
        self.related_eq_shortcut = QShortcut(self)
        self.related_idt_shortcut = QShortcut(self)
        self.related_lst_shortcut = QShortcut(self)
        self.related_scl_shortcut = QShortcut(self)
        self.related_tbl_shortcut = QShortcut(self)
        self.related_var_shortcut = QShortcut(self)

    @property
    def table_model(self) -> IodeAbstractTableModel:
        return self.model()

    def setup(self):
        """
        This method sets up the table view.
        """
        pass

    def update(self):
        """
        This method updates the table view.
        """
        table_model = self.table_model
        table_model.reset()

    def setFilterLineEdit(self, filter_line_edit: QLineEdit):
        """
        This method sets the filter line edit for the table view.

        :param filter_line_edit: The filter line edit.
        """
        self.filter_line_edit = filter_line_edit

    def preferredHeight(self):
        """
        This method returns the preferred height of the table view.
        """
        height = self.horizontalHeader().height()
        for i in range(self.model().rowCount()):
            height += self.rowHeight(i)
        if self.iode_type == "VARIABLES":
            height += 20
        return height

    def enableDeleteShortcut(self, enable):
        """
        This method enables or disables the delete shortcut.

        :param enable: A boolean value indicating whether to enable or disable the delete shortcut.
        """
        self.delete_shortcut.setEnabled(enable)
