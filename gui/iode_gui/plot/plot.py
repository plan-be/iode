from PySide6.QtCore import Slot
from PySide6.QtWidgets import (QDialog, QVBoxLayout, QHBoxLayout, QLabel, QComboBox, 
                               QCheckBox, QLineEdit, QPushButton, QMessageBox)
from PySide6.QtGui import QDoubleValidator

from typing import Union, List, Tuple, Dict, Any, Sequence
import numpy as np
from matplotlib.backends.backend_qtagg import FigureCanvas
from matplotlib.backends.backend_qtagg import NavigationToolbar2QT as NavigationToolbar
from matplotlib.backends.qt_compat import QtWidgets
from matplotlib.figure import Figure
from matplotlib.axes import Axes

from iode import TableGraphType, TableGraphGrid


class PlotDialog(QDialog):
    def __init__(self, chart_type: TableGraphType = TableGraphType.LINE, 
                 grid: TableGraphGrid = TableGraphGrid.MAJOR,  
                 log_scale: bool = False, 
                 y_min: float=None, y_max: float=None, 
                 title: str=None, parent=None):
        super().__init__(parent)
        self.series: Dict[str, Tuple[np.ndarray, np.ndarray]] = dict()
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
        self.graph_type_combo.currentIndexChanged.connect(self.plot)
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
        self.apply_button.clicked.connect(self.plot)
        controls_layout.addWidget(self.apply_button)

        layout.addLayout(controls_layout)

        # Initialize graph
        self.reset_graph()

    def reset_graph(self) -> Axes:
        self.figure.clear()
        ax = self.figure.add_subplot(111)
        ax.set_xlabel('periods')
        ax.set_ylabel('values')
        if self.title:
            ax.set_title(self.title)
        return ax

    def add_series(self, label: str, x_data: Sequence[float], y_data: Sequence[float]):
        try:
            x_data, y_data = np.asarray(x_data), np.asarray(y_data)
            if x_data.ndim != 1:
                raise ValueError("x_data must be 1-dimensional")
            if y_data.ndim != 1:
                raise ValueError("y_data must be 1-dimensional")
            if len(x_data) != len(y_data):
                raise ValueError("x_data and y_data must have the same length")
            self.series[label] = (np.asarray(x_data), np.asarray(y_data))
        except Exception as e:
            QMessageBox.warning(None, "WARNING", f"Error adding series {label}: {e}")

    @Slot()
    def plot(self):
        ax = self.reset_graph()
        index_graph_type = self.graph_type_combo.currentIndex()

        graph_type: TableGraphType = self.v_chart_types[index_graph_type]
        for label, (x, y) in self.series.items():
            if graph_type == TableGraphType.LINE:
                ax.plot(x, y, label=label)
            elif graph_type == TableGraphType.SCATTER:
                ax.scatter(x, y, label=label)
            elif graph_type == TableGraphType.BAR:
                ax.bar(x, y, width=0.1, label=label)
        ax.legend()

        self.update_y_limits(False)
        self.update_grid(False)
        self.update_scale(False)
        self.canvas.draw()

    @Slot()
    def update_scale(self, draw: bool=True):
        ax = self.figure.gca()
        index_graph_type = self.graph_type_combo.currentIndex()
        graph_type: TableGraphType = self.v_chart_types[index_graph_type]
        log_scale = self.log_scale_checkbox.isChecked()

        if log_scale and graph_type != TableGraphType.BAR:
            ax.set_yscale("log")
        else:
            ax.set_yscale("linear")

        if draw:
            self.canvas.draw()

    @Slot()
    def update_grid(self, draw: bool=True):
        ax = self.figure.gca()
        index_grid = self.grid_combo.currentIndex()
        grid: TableGraphGrid = self.v_chart_grid[index_grid]

        if grid == TableGraphGrid.NONE:
            ax.grid(False)
        elif grid == TableGraphGrid.MINOR:
            ax.grid(True, which='minor', linestyle=':', linewidth=0.5)
        elif grid == TableGraphGrid.MAJOR:
            ax.grid(True, which='major', linestyle='-', linewidth=0.75)

        if draw:
            self.canvas.draw()

    @Slot()
    def update_y_limits(self, draw: bool=True):
        ax = self.figure.gca()
        y_min = self.y_min_input.text()
        y_max = self.y_max_input.text()

        try:
            y_min = float(y_min) if y_min else None
            y_max = float(y_max) if y_max else None
            ax.set_ylim(y_min, y_max)
        except ValueError:
            pass  # Ignore invalid input

        if draw:
            self.canvas.draw()
