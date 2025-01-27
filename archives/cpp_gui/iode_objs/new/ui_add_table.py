# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'add_table.ui'
##
## Created by: Qt User Interface Compiler version 6.8.0
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QApplication, QCheckBox, QDialog, QFrame,
    QGridLayout, QLabel, QLineEdit, QPushButton,
    QSizePolicy, QSpacerItem, QSpinBox, QWidget)

from text_edit.complete_text_edit import IodeAutoCompleteTextEdit

class Ui_AddTableDialog(object):
    def setupUi(self, AddTableDialog):
        if not AddTableDialog.objectName():
            AddTableDialog.setObjectName(u"AddTableDialog")
        AddTableDialog.resize(600, 418)
        AddTableDialog.setMinimumSize(QSize(600, 0))
        self.gridLayout = QGridLayout(AddTableDialog)
        self.gridLayout.setObjectName(u"gridLayout")
        self.horizontalSpacer_2 = QSpacerItem(243, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_2, 8, 8, 1, 3)

        self.verticalSpacer = QSpacerItem(20, 174, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 7, 0, 1, 3)

        self.spinBox_nb_columns = QSpinBox(AddTableDialog)
        self.spinBox_nb_columns.setObjectName(u"spinBox_nb_columns")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.spinBox_nb_columns.sizePolicy().hasHeightForWidth())
        self.spinBox_nb_columns.setSizePolicy(sizePolicy)
        self.spinBox_nb_columns.setMinimum(2)
        self.spinBox_nb_columns.setMaximum(10)

        self.gridLayout.addWidget(self.spinBox_nb_columns, 1, 9, 1, 1)

        self.label_table_title = QLabel(AddTableDialog)
        self.label_table_title.setObjectName(u"label_table_title")

        self.gridLayout.addWidget(self.label_table_title, 5, 0, 1, 1)

        self.label_auto_table = QLabel(AddTableDialog)
        self.label_auto_table.setObjectName(u"label_auto_table")
        palette = QPalette()
        brush = QBrush(QColor(0, 0, 0, 255))
        brush.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.Text, brush)
        brush1 = QBrush(QColor(0, 120, 215, 255))
        brush1.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.Highlight, brush1)
        palette.setBrush(QPalette.Inactive, QPalette.Text, brush)
        brush2 = QBrush(QColor(240, 240, 240, 255))
        brush2.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Inactive, QPalette.Highlight, brush2)
        brush3 = QBrush(QColor(120, 120, 120, 255))
        brush3.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Disabled, QPalette.Text, brush3)
        palette.setBrush(QPalette.Disabled, QPalette.Highlight, brush1)
        self.label_auto_table.setPalette(palette)
        font = QFont()
        font.setPointSize(9)
        font.setBold(False)
        font.setStrikeOut(False)
        self.label_auto_table.setFont(font)
        self.label_auto_table.setAutoFillBackground(False)
        self.label_auto_table.setFrameShape(QFrame.Panel)
        self.label_auto_table.setFrameShadow(QFrame.Raised)
        self.label_auto_table.setLineWidth(1)
        self.label_auto_table.setTextFormat(Qt.AutoText)
        self.label_auto_table.setAlignment(Qt.AlignCenter)
        self.label_auto_table.setWordWrap(True)

        self.gridLayout.addWidget(self.label_auto_table, 2, 0, 3, 11)

        self.horizontalSpacer = QSpacerItem(147, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 1, 10, 1, 1)

        self.lineEdit_table_title = QLineEdit(AddTableDialog)
        self.lineEdit_table_title.setObjectName(u"lineEdit_table_title")

        self.gridLayout.addWidget(self.lineEdit_table_title, 5, 1, 1, 10)

        self.spacerItem = QSpacerItem(20, 277, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.spacerItem, 4, 11, 5, 1)

        self.checkBox_files = QCheckBox(AddTableDialog)
        self.checkBox_files.setObjectName(u"checkBox_files")
        sizePolicy.setHeightForWidth(self.checkBox_files.sizePolicy().hasHeightForWidth())
        self.checkBox_files.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.checkBox_files, 8, 5, 1, 1)

        self.label_lec = QLabel(AddTableDialog)
        self.label_lec.setObjectName(u"label_lec")
        sizePolicy.setHeightForWidth(self.label_lec.sizePolicy().hasHeightForWidth())
        self.label_lec.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.label_lec, 6, 0, 1, 4)

        self.textEdit_lec = IodeAutoCompleteTextEdit(AddTableDialog)
        self.textEdit_lec.setObjectName(u"textEdit_lec")

        self.gridLayout.addWidget(self.textEdit_lec, 6, 4, 2, 7)

        self.checkBox_date = QCheckBox(AddTableDialog)
        self.checkBox_date.setObjectName(u"checkBox_date")
        sizePolicy.setHeightForWidth(self.checkBox_date.sizePolicy().hasHeightForWidth())
        self.checkBox_date.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.checkBox_date, 8, 6, 1, 2)

        self.helpButton = QPushButton(AddTableDialog)
        self.helpButton.setObjectName(u"helpButton")
        sizePolicy.setHeightForWidth(self.helpButton.sizePolicy().hasHeightForWidth())
        self.helpButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.helpButton, 3, 11, 1, 1)

        self.checkBox_mode = QCheckBox(AddTableDialog)
        self.checkBox_mode.setObjectName(u"checkBox_mode")
        sizePolicy.setHeightForWidth(self.checkBox_mode.sizePolicy().hasHeightForWidth())
        self.checkBox_mode.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.checkBox_mode, 8, 3, 1, 2)

        self.label_footnotes = QLabel(AddTableDialog)
        self.label_footnotes.setObjectName(u"label_footnotes")
        sizePolicy.setHeightForWidth(self.label_footnotes.sizePolicy().hasHeightForWidth())
        self.label_footnotes.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.label_footnotes, 8, 0, 1, 3)

        self.saveButton = QPushButton(AddTableDialog)
        self.saveButton.setObjectName(u"saveButton")
        sizePolicy.setHeightForWidth(self.saveButton.sizePolicy().hasHeightForWidth())
        self.saveButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.saveButton, 1, 11, 1, 1)

        self.label_nb_columns = QLabel(AddTableDialog)
        self.label_nb_columns.setObjectName(u"label_nb_columns")
        sizePolicy.setHeightForWidth(self.label_nb_columns.sizePolicy().hasHeightForWidth())
        self.label_nb_columns.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.label_nb_columns, 1, 7, 1, 2)

        self.label_title = QLabel(AddTableDialog)
        self.label_title.setObjectName(u"label_title")
        palette1 = QPalette()
        palette1.setBrush(QPalette.Active, QPalette.Highlight, brush1)
        palette1.setBrush(QPalette.Inactive, QPalette.Highlight, brush2)
        palette1.setBrush(QPalette.Disabled, QPalette.Highlight, brush1)
        self.label_title.setPalette(palette1)
        font1 = QFont()
        font1.setPointSize(12)
        font1.setBold(False)
        font1.setStrikeOut(False)
        self.label_title.setFont(font1)
        self.label_title.setAutoFillBackground(False)
        self.label_title.setFrameShape(QFrame.StyledPanel)
        self.label_title.setFrameShadow(QFrame.Raised)
        self.label_title.setLineWidth(2)
        self.label_title.setTextFormat(Qt.AutoText)
        self.label_title.setAlignment(Qt.AlignCenter)
        self.label_title.setWordWrap(False)

        self.gridLayout.addWidget(self.label_title, 0, 0, 1, 12)

        self.label_name = QLabel(AddTableDialog)
        self.label_name.setObjectName(u"label_name")
        sizePolicy.setHeightForWidth(self.label_name.sizePolicy().hasHeightForWidth())
        self.label_name.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.label_name, 1, 0, 1, 2)

        self.label = QLabel(AddTableDialog)
        self.label.setObjectName(u"label")
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.label, 1, 6, 1, 1)

        self.cancelButton = QPushButton(AddTableDialog)
        self.cancelButton.setObjectName(u"cancelButton")
        sizePolicy.setHeightForWidth(self.cancelButton.sizePolicy().hasHeightForWidth())
        self.cancelButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.cancelButton, 2, 11, 1, 1)

        self.lineEdit_name = QLineEdit(AddTableDialog)
        self.lineEdit_name.setObjectName(u"lineEdit_name")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.lineEdit_name.sizePolicy().hasHeightForWidth())
        self.lineEdit_name.setSizePolicy(sizePolicy1)

        self.gridLayout.addWidget(self.lineEdit_name, 1, 2, 1, 4)


        self.retranslateUi(AddTableDialog)
        self.saveButton.clicked.connect(AddTableDialog.add)
        self.cancelButton.clicked.connect(AddTableDialog.reject)
        self.helpButton.clicked.connect(AddTableDialog.help)

        QMetaObject.connectSlotsByName(AddTableDialog)
    # setupUi

    def retranslateUi(self, AddTableDialog):
        AddTableDialog.setWindowTitle(QCoreApplication.translate("AddTableDialog", u"Table", None))
        self.label_table_title.setText(QCoreApplication.translate("AddTableDialog", u"Title", None))
        self.label_auto_table.setText(QCoreApplication.translate("AddTableDialog", u"<html><head/><body><p align=\"center\"><span style=\" text-decoration: underline;\">Auto Table</span></p><p>To automatically create a table, enter LEC forms (separator = ';'), the title and the selected footnotes</p></body></html>", None))
        self.checkBox_files.setText(QCoreApplication.translate("AddTableDialog", u"Files", None))
        self.label_lec.setText(QCoreApplication.translate("AddTableDialog", u"LEC formulas", None))
#if QT_CONFIG(tooltip)
        self.textEdit_lec.setToolTip(QCoreApplication.translate("AddTableDialog", u"<html><head/><body><p>LEC formulas separated by sem-colons ';'</p></body></html>", None))
#endif // QT_CONFIG(tooltip)
        self.checkBox_date.setText(QCoreApplication.translate("AddTableDialog", u"Date", None))
        self.helpButton.setText(QCoreApplication.translate("AddTableDialog", u"Help", None))
#if QT_CONFIG(shortcut)
        self.helpButton.setShortcut(QCoreApplication.translate("AddTableDialog", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.checkBox_mode.setText(QCoreApplication.translate("AddTableDialog", u"Mode", None))
        self.label_footnotes.setText(QCoreApplication.translate("AddTableDialog", u"Footnotes", None))
        self.saveButton.setText(QCoreApplication.translate("AddTableDialog", u"Save", None))
#if QT_CONFIG(shortcut)
        self.saveButton.setShortcut(QCoreApplication.translate("AddTableDialog", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.label_nb_columns.setText(QCoreApplication.translate("AddTableDialog", u"Number of columns", None))
        self.label_title.setText(QCoreApplication.translate("AddTableDialog", u"Add Table", None))
        self.label_name.setText(QCoreApplication.translate("AddTableDialog", u"Name", None))
        self.label.setText("")
        self.cancelButton.setText(QCoreApplication.translate("AddTableDialog", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.cancelButton.setShortcut(QCoreApplication.translate("AddTableDialog", u"Esc", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

