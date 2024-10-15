from PySide6.QtWidgets import QCompleter, QWidget

from iode import IodeTypes


class IodeCompleter(QCompleter):
    def __init__(self, report_commands: bool = True, lec_functions: bool = False, 
        iode_type: IodeTypes = None, parent: QWidget = None):
        super(IodeCompleter, self).__init__(parent)

    def update_iode_objects_list_names(self) -> None:
        pass
