from PySide6.QtCore import Slot
from PySide6.QtWidgets import QDialog

from iode_gui.plot.plot import PlotDialog

from typing import Union, List, Dict, Any
import numpy as np
from iode import VarsMode, TableGraphType, TableGraphGrid


class PlotVariablesDialog(PlotDialog):
    def __init__(self, x_data: Union[List[float], np.ndarray],
                 y_data: Dict[str, Any],
                 chart_type: TableGraphType = TableGraphType.LINE, 
                 grid: TableGraphGrid = TableGraphGrid.MAJOR,  
                 var_mode: VarsMode = VarsMode.LEVEL,
                 log_scale: bool = False, 
                 y_min: float=None, y_max: float=None, 
                 title: str=None, parent=None): 
        super().__init__(x_data, y_data, chart_type, grid, log_scale, y_min, y_max, title, parent)

        self.v_variable_modes = list(VarsMode)
        v_variable_modes_names = [item.name.replace("_" , " ").title() for item in self.v_variable_modes]
        # self.comboBox_x_axis_type.addItems(v_variable_modes_names)
        
        self._var_mode = var_mode
        index = self.v_variable_modes.index(var_mode) 
        # self.comboBox_x_axis_type.setCurrentIndex(index)
