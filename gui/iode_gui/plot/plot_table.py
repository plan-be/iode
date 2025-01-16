from iode_gui.plot.plot import PlotDialog

from typing import Union, List, Dict, Any
from iode import TableGraphType, TableGraphGrid
from iode.util import ComputedTable


class PlotTableDialog(PlotDialog):
    def __init__(self, computed_table: ComputedTable, 
                 chart_type: TableGraphType = TableGraphType.LINE, 
                 grid: TableGraphGrid = TableGraphGrid.MAJOR,
                 log_scale: bool = False, 
                 y_min: float=None, y_max: float=None, 
                 title: str=None, parent=None):
        super().__init__(chart_type, grid, log_scale, y_min, y_max, title, parent)
        
        for row in range(computed_table.nb_lines):
            for op_files in range(computed_table.nb_operations_between_files):
                series_name = computed_table.plotting_series_name(row, op_files)
                x, y = computed_table.plotting_series_values(row, op_files)
                self.add_series(series_name, x, y)
