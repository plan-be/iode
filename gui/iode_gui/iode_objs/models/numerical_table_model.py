from PySide6.QtCore import QObject, Signal
from iode_gui.utils import NAN_REP, MAX_PRECISION_NUMBERS

from typing import List, Any
import numpy as np
from iode import IodeType, is_NA


class IodeNumericalTableModel:
    """
    Handles the number of significant digits to display in any numerical tables.
    """
    no_precision = False

    precision_changed = Signal(int)

    def __init__(self, precision: int=2, format: str='f'):
        self._precision: int = precision
        self._format: str = format
        self._min_precision: int = 0 if format == 'f' else 1

    def value_to_string(self, value: Any):
        """
        Converts the given value to a string based on the current precision and format.
        """
        if isinstance(value, float) and (is_NA(value) or np.isnan(value)):
            return NAN_REP
        if not isinstance(value, (int, float)):
            return NAN_REP
        # If no_precision is True, QLocale.FloatingPointShortest is used.
        # Otherwise, the specified precision is used.
        precision = -1 if self.no_precision else self._precision
        return f"{value:.{precision}{self._format}}"

    def string_to_float(self, value: str) -> float:
        """
        Converts the given string to a float.
        """
        return np.nan if value == NAN_REP or value == "" else float(value)

    @property
    def precision(self) -> int:
        """
        Returns the current precision.
        """
        return self._precision

    @precision.setter
    def precision(self, precision: int):
        """
        Sets the precision, ensuring it is within the valid range.
        """
        if precision < self._min_precision:
            self._precision = self._min_precision
        elif precision > MAX_PRECISION_NUMBERS:
            self._precision = MAX_PRECISION_NUMBERS
        else:
            self._precision = precision
        self.update_display_values()

    @property
    def min_precision(self) -> int:
        """
        Returns the minimum precision.
        """
        return self._min_precision

    @property
    def format(self) -> str:
        """
        Returns the current format.
        """
        return self._format

    @format.setter
    def format(self, format: str):
        """
        Sets the format and updates the minimum precision accordingly.
        """
        self._format = format
        self._min_precision = 0 if format == 'f' else 1
        self.update_display_values()

    def precision_plus(self):
        """
        Increases the precision by one, if possible.
        """
        if self._precision < MAX_PRECISION_NUMBERS:
            self._precision += 1
            self.update_display_values()
            self.precision_changed.emit(self._precision)

    def precision_minus(self):
        """
        Decreases the precision by one, if possible.
        """
        if self._precision > self._min_precision:
            self._precision -= 1
            self.update_display_values()
            self.precision_changed.emit(self._precision)

    def update_display_values(self):
        """
        Updates the display values. This method must be implemented in derived classes.
        """
        raise NotImplementedError()
