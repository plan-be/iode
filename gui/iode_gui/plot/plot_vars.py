from iode_gui.plot.plot import PlotDialog

from typing import Union, List, Dict, Any
from iode import VarsMode, TableGraphType, TableGraphGrid, Period, Sample, Variables


class PlotVariablesDialog(PlotDialog):
    def __init__(self, variables_database: Variables, 
                 variables_names: Union[str, List[str]], 
                 first_period: Union[str, Period] = None, 
                 last_period: Union[str, Period] = None, 
                 chart_type: TableGraphType = TableGraphType.LINE, 
                 grid: TableGraphGrid = TableGraphGrid.MAJOR,  
                 var_mode: VarsMode = VarsMode.LEVEL,
                 log_scale: bool = False, 
                 y_min: float=None, y_max: float=None, 
                 title: str=None, parent=None): 
        super().__init__(chart_type, grid, log_scale, y_min, y_max, title, parent)

        self.variables_database = variables_database
        if isinstance(variables_names, str):
            self.variables_names = self.variables_database.get_names(variables_names)
        else:
            self.variables_names = variables_names
        self.variables_database.mode = var_mode

        vars_sample = self.variables_database.sample
        if first_period is None:
            first_period = vars_sample.start
        if last_period is None:
            last_period = vars_sample.end
        
        # raise an error if first_period or/and last_period is/are invalid
        plotting_sample = Sample(first_period, last_period)
        
        first_period = plotting_sample.start
        if first_period < vars_sample.start:
            raise RuntimeError(f"First period {first_period} is before the first period "
                               f"of the variables database {vars_sample.start}")
        
        last_period = plotting_sample.end
        if last_period > vars_sample.end:
            raise RuntimeError(f"Last period {last_period} is after the last period "
                               f"of the variables database {vars_sample.end}")

        if first_period == last_period:
            raise RuntimeError("Please select more than 1 period to plot")
        
        periods_filter = f"{first_period}:{last_period}"
        x = plotting_sample.get_period_list(astype=float)
        for variable_name in self.variables_names:
            # NOTE: need to call flatten() since to_ndarray() returns an array of dimensions
            #       (nb_variables, n_periods) even if nb_variables == 1
            y = self.variables_database[variable_name, periods_filter].to_ndarray().flatten()
            self.add_series(variable_name, x, y)
