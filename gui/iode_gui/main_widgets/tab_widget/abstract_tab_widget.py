from qtpy.QtWidgets import QTabWidget

from typing import List            
from iode import IodeType


class AbstractIodeTabWidget(QTabWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
    
    def show_database_subset(self, iode_type: IodeType, obj_names: List[str] = None):
        raise NotImplementedError("show_database_subset() method must be implemented in subclass")
