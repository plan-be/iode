from PySide6.QtCore import Slot
from PySide6.QtWidgets import (QDialog, QVBoxLayout, QHBoxLayout, QLabel, QComboBox, 
                               QCheckBox, QLineEdit, QPushButton)
from PySide6.QtGui import QDoubleValidator

from typing import Union, List, Dict, Any
import numpy as np
import pandas as pd
from matplotlib.backends.backend_qtagg import FigureCanvas
from matplotlib.backends.backend_qtagg import NavigationToolbar2QT as NavigationToolbar
from matplotlib.backends.qt_compat import QtWidgets
from matplotlib.figure import Figure

from iode import TableGraphType, TableGraphGrid


class PlotDialog(QDialog):
    def __init__(self, x_data: Union[List[float], np.ndarray],
                 y_data: Dict[str, Any],
                 chart_type: TableGraphType = TableGraphType.LINE, 
                 grid: TableGraphGrid = TableGraphGrid.MAJOR,  
                 log_scale: bool = False, 
                 y_min: float=None, y_max: float=None, 
                 title: str=None, parent=None):
        super().__init__(parent)
        self.x_data = np.asarray(x_data)
        self.y_data = {key: np.asarray(value) for key, value in y_data.items()} 
        self.title = title

        self.v_chart_types = list(TableGraphType)
        v_chart_type_names = [chart_type.name.title() for chart_type in self.v_chart_types]

        self.v_chart_grid = list(TableGraphGrid)
        v_chart_grid_names = [axis_ticks.name.title() for axis_ticks in self.v_chart_grid]
        
        self.resize(800, 600)

        # Main layout
        layout = QVBoxLayout(self)

        # Matplotlib Figure
        self.figure = Figure()
        self.canvas = FigureCanvas(self.figure)
        self.toolbar = NavigationToolbar(self.canvas, self)
        layout.addWidget(self.toolbar)
        layout.addWidget(self.canvas)

        # Control panel
        controls_layout = QHBoxLayout()
        
        # Graph type combobox
        controls_layout.addWidget(QLabel("Graph Type:"))
        self.graph_type_combo = QComboBox()
        self.graph_type_combo.addItems(v_chart_type_names)
        self.graph_type_combo.setCurrentIndex(self.v_chart_types.index(chart_type))
        self.graph_type_combo.currentIndexChanged.connect(self.update_graph)
        controls_layout.addWidget(self.graph_type_combo)

        # Grid combobox
        controls_layout.addWidget(QLabel("Grid:"))
        self.grid_combo = QComboBox()
        self.grid_combo.addItems(v_chart_grid_names)
        self.grid_combo.setCurrentIndex(self.v_chart_grid.index(grid))
        self.grid_combo.currentIndexChanged.connect(self.update_grid)
        controls_layout.addWidget(self.grid_combo)

        # Log scale checkbox
        self.log_scale_checkbox = QCheckBox("Log Scale (Y Axis)")
        self.log_scale_checkbox.setChecked(log_scale)
        self.log_scale_checkbox.stateChanged.connect(self.update_scale)
        controls_layout.addWidget(self.log_scale_checkbox)

        # Y-axis range inputs
        controls_layout.addWidget(QLabel("Y Min:"))
        self.y_min_input = QLineEdit()
        self.y_min_input.setPlaceholderText("Min")
        self.y_min_input.editingFinished.connect(self.update_y_limits)
        self.y_min_input.setValidator(QDoubleValidator(self.y_min_input))
        if y_min:
            self.y_min_input.setText(str(y_min))
        controls_layout.addWidget(self.y_min_input)

        controls_layout.addWidget(QLabel("Y Max:"))
        self.y_max_input = QLineEdit()
        self.y_max_input.setPlaceholderText("Max")
        self.y_max_input.editingFinished.connect(self.update_y_limits)
        self.y_max_input.setValidator(QDoubleValidator(self.y_max_input))
        if y_max:
            self.y_max_input.setText(str(y_max))
        controls_layout.addWidget(self.y_max_input)

        # Apply button
        self.apply_button = QPushButton("Apply")
        self.apply_button.clicked.connect(self.update_graph)
        controls_layout.addWidget(self.apply_button)

        layout.addLayout(controls_layout)

        # Initialize graph
        self.update_graph()

    def update_graph(self):
        index_graph_type = self.graph_type_combo.currentIndex()
        
        self.figure.clear()
        ax = self.figure.add_subplot(111)

        graph_type: TableGraphType = self.v_chart_types[index_graph_type]
        for label, y in self.y_data.items():
            if graph_type == TableGraphType.LINE:
                ax.plot(self.x_data, y, label=label)
            elif graph_type == TableGraphType.SCATTER:
                ax.scatter(self.x_data, y, label=label)
            elif graph_type == TableGraphType.BAR:
                ax.bar(self.x_data, y, width=0.1, label=label)
        ax.set_xlabel('periods')
        ax.set_ylabel('values')
        if self.title:
            ax.set_title(self.title)

        y_min = self.y_min_input.text()
        y_max = self.y_max_input.text()

        try:
            y_min = float(y_min) if y_min else None
            y_max = float(y_max) if y_max else None
            ax.set_ylim(y_min, y_max)
        except ValueError:
            pass  # Ignore invalid input

        ax.legend()
        self.update_grid()
        self.update_scale()
        self.canvas.draw()

    def update_scale(self):
        ax = self.figure.gca()
        index_graph_type = self.graph_type_combo.currentIndex()
        graph_type: TableGraphType = self.v_chart_types[index_graph_type]
        log_scale = self.log_scale_checkbox.isChecked()

        if log_scale and graph_type != TableGraphType.BAR:
            ax.set_yscale("log")
        else:
            ax.set_yscale("linear")

        self.canvas.draw()

    def update_grid(self):
        ax = self.figure.gca()
        index_grid = self.grid_combo.currentIndex()
        grid: TableGraphGrid = self.v_chart_grid[index_grid]

        if grid == TableGraphGrid.NONE:
            ax.grid(False)
        elif grid == TableGraphGrid.MINOR:
            ax.grid(True, which='minor', linestyle=':', linewidth=0.5)
        elif grid == TableGraphGrid.MAJOR:
            ax.grid(True, which='major', linestyle='-', linewidth=0.75)

        self.canvas.draw()

    def update_y_limits(self):
        self.update_graph()
