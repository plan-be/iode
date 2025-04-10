from PySide6.QtCore import QAbstractTableModel, QModelIndex, Qt

from iode.compute.estimation import CorrelationMatrix


class CorrelationMatrixModel(QAbstractTableModel):
    """
    QAbstractTableModel subclass that provides a model for a Correlation Matrix.
    """

    def __init__(self, correlation_matrix: CorrelationMatrix, parent=None):
        """
        Initializes the CorrelationMatrixModel with a CorrelationMatrix object.

        :param correlation_matrix: The CorrelationMatrix object to be used for the model.
        :param parent: The parent QObject for this model.
        """
        super().__init__(parent)
        self.correlation_matrix: CorrelationMatrix = correlation_matrix

    def rowCount(self, parent=QModelIndex()) -> int:
        """
        Returns the number of rows in the model.

        :param parent: The parent model index.
        :return: The number of coefficients in the CorrelationMatrix object.
        """
        return len(self.correlation_matrix)

    def columnCount(self, parent=QModelIndex()) -> int:
        """
        Returns the number of columns in the model.

        :param parent: The parent model index.
        :return: The number of coefficients in the CorrelationMatrix object.
        """
        return len(self.correlation_matrix)

    def headerData(self, section: int, orientation: Qt.Orientation, role: Qt.ItemDataRole):
        """
        Returns the data for the given header section and data role.

        :param section: The header section.
        :param orientation: The header orientation.
        :param role: The data role.
        :return: The header data.
        """
        if role != Qt.ItemDataRole.DisplayRole:
            return

        return self.correlation_matrix.name(section)

    def data(self, index: QModelIndex, role: Qt.ItemDataRole=Qt.ItemDataRole.DisplayRole):
        """
        Returns the data for the given model index and data role.

        :param index: The model index.
        :param role: The data role.
        :return: The data.
        """
        if not index.isValid():
            return 

        if index.row() >= self.rowCount() or index.column() >= self.columnCount():
            return 

        if role == Qt.ItemDataRole.TextAlignmentRole:
            return Qt.AlignmentFlag.AlignRight

        if role == Qt.ItemDataRole.DisplayRole:
            return f"{self.correlation_matrix[(index.row(), index.column())]:.6g}"
        
        return 
