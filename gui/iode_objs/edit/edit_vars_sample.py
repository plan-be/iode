from PySide6.QtWidgets import QDialog


class EditIodeSampleDialog(QDialog):
    def __init__(self, parent=None, sample=None):
        super(EditIodeSampleDialog, self).__init__(parent)