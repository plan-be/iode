from PySide6.QtWidgets import QDialog
from PySide6.QtGui import QKeySequence, QShortcut
from PySide6.QtCore import Qt

from .ui_estimation_coefs import Ui_EstimationCoefsDialog
from iode_gui.iode_objs.models.table_model import ScalarsModel
from iode.compute.estimation import EditAndEstimateEquations


class EstimationCoefsDialog(QDialog):
    """A dialog for displaying estimation coefficients.

    This class sets up a dialog for displaying estimation coefficients.
    It uses a ScalarsModel to handle the coefficients data, and it sets up
    a shortcut for toggling full screen mode.
    """

    def __init__(self, edit_est_eqs: EditAndEstimateEquations, parent=None):
        """Initialize the EstimationCoefsDialog.

        Args:
            edit_est_eqs: An instance of EditAndEstimateEquations.
            parent: The parent widget.
        """
        super().__init__(parent)
        self.ui = Ui_EstimationCoefsDialog()
        self.ui.setupUi(self)

        # Set up the table view
        stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }"
        self.ui.tableView_coefs.setStyleSheet(stylesheet)

        # Create a copy of the estimation scalars database
        scalars_db = edit_est_eqs.scalars_db.copy()
        scalarsModel = ScalarsModel(scalars_db=scalars_db, parent=self)
        self.ui.tableView_coefs.setModel(scalarsModel)

        # Set up the full screen shortcut
        self.setWindowFlags(Qt.WindowType.Window)
        self.full_screen_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_X), self)
        self.full_screen_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.full_screen_shortcut.activated.connect(self.showMaximized)
