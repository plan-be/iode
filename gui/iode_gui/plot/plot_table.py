from iode_gui.plot.plot import PlotDialog

from typing import Union, List, Dict, Any
from iode import TableGraphType, TableGraphGrid
from iode.computed_table.computed_table import ComputedTable


class PlotTableDialog(PlotDialog):
    def __init__(self, computed_table: ComputedTable, 
                 chart_type: TableGraphType = TableGraphType.LINE, 
                 grid: TableGraphGrid = TableGraphGrid.MAJOR,
                 log_scale: bool = False, 
                 y_min: float=None, y_max: float=None, 
                 title: str=None, parent=None):
        super().__init__(chart_type, grid, log_scale, y_min, y_max, title, parent)
        
        series = computed_table.plot_data
        for series_name, (x, y) in series.items():
            self.add_series(series_name, x, y)
