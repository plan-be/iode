from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from settings import MixinSettingsDialog
from .ui_workspace_describe import Ui_MenuWorkspaceDescribe


from iode import (comments, equations, identities, lists, 
                  scalars, tables, variables)


class MenuWorkspaceDescribe(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuWorkspaceDescribe()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)
        self.load_settings()

    @Slot()
    def update_descriptions(self):
        try:
            # Extract and verify the descriptions
            comments_description: str = self.ui.lineEdit_comments.text()
            equations_description: str = self.ui.lineEdit_equations.text()
            identities_description: str = self.ui.lineEdit_identities.text()
            lists_description: str = self.ui.lineEdit_lists.text()
            scalars_description: str = self.ui.lineEdit_scalars.text()
            tables_description: str = self.ui.lineEdit_tables.text()
            variables_description: str = self.ui.lineEdit_variables.text()

            # Update the descriptions if they're not empty
            if comments_description:
                comments.description = comments_description
            if equations_description:
                equations.description = equations_description
            if identities_description:
                identities.description = identities_description
            if lists_description:
                lists.description = lists_description
            if scalars_description:
                scalars.description = scalars_description
            if tables_description:
                tables.description = tables_description
            if variables_description:
                variables.description = variables_description

            self.accept()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", "Failed to updated the description "
                                "of the IODE database(s)\n" + str(e))
