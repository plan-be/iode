# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'edit_table.ui'
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
from PySide6.QtWidgets import (QAbstractItemView, QApplication, QComboBox, QDialog,
    QGridLayout, QHeaderView, QLabel, QLineEdit,
    QPushButton, QSizePolicy, QSpacerItem, QVBoxLayout,
    QWidget)

from iode_objs.edit.edit_table_view import EditTableView

class Ui_EditTableDialog(object):
    def setupUi(self, EditTableDialog):
        if not EditTableDialog.objectName():
            EditTableDialog.setObjectName(u"EditTableDialog")
        EditTableDialog.resize(621, 342)
        EditTableDialog.setSizeGripEnabled(False)
        self.gridLayout = QGridLayout(EditTableDialog)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_name = QLabel(EditTableDialog)
        self.label_name.setObjectName(u"label_name")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_name.sizePolicy().hasHeightForWidth())
        self.label_name.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.label_name, 0, 0, 1, 1)

        self.lineEdit_name = QLineEdit(EditTableDialog)
        self.lineEdit_name.setObjectName(u"lineEdit_name")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.lineEdit_name.sizePolicy().hasHeightForWidth())
        self.lineEdit_name.setSizePolicy(sizePolicy1)
        self.lineEdit_name.setReadOnly(False)

        self.gridLayout.addWidget(self.lineEdit_name, 0, 1, 1, 2)

        self.horizontalSpacer_2 = QSpacerItem(305, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_2, 0, 3, 1, 2)

        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.saveButton = QPushButton(EditTableDialog)
        self.saveButton.setObjectName(u"saveButton")
        sizePolicy2 = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.saveButton.sizePolicy().hasHeightForWidth())
        self.saveButton.setSizePolicy(sizePolicy2)

        self.verticalLayout.addWidget(self.saveButton)

        self.pushButton_graph = QPushButton(EditTableDialog)
        self.pushButton_graph.setObjectName(u"pushButton_graph")

        self.verticalLayout.addWidget(self.pushButton_graph)

        self.cancelButton = QPushButton(EditTableDialog)
        self.cancelButton.setObjectName(u"cancelButton")
        sizePolicy2.setHeightForWidth(self.cancelButton.sizePolicy().hasHeightForWidth())
        self.cancelButton.setSizePolicy(sizePolicy2)

        self.verticalLayout.addWidget(self.cancelButton)

        self.helpButton = QPushButton(EditTableDialog)
        self.helpButton.setObjectName(u"helpButton")
        sizePolicy2.setHeightForWidth(self.helpButton.sizePolicy().hasHeightForWidth())
        self.helpButton.setSizePolicy(sizePolicy2)

        self.verticalLayout.addWidget(self.helpButton)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer)


        self.gridLayout.addLayout(self.verticalLayout, 0, 5, 2, 1)

        self.tableView = EditTableView(EditTableDialog)
        self.tableView.setObjectName(u"tableView")
        sizePolicy3 = QSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        sizePolicy3.setHorizontalStretch(0)
        sizePolicy3.setVerticalStretch(0)
        sizePolicy3.setHeightForWidth(self.tableView.sizePolicy().hasHeightForWidth())
        self.tableView.setSizePolicy(sizePolicy3)
        self.tableView.setAlternatingRowColors(True)
        self.tableView.setSelectionMode(QAbstractItemView.SingleSelection)
        self.tableView.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.tableView.setGridStyle(Qt.DotLine)
        self.tableView.setSortingEnabled(True)
        self.tableView.setCornerButtonEnabled(False)
        self.tableView.horizontalHeader().setVisible(True)
        self.tableView.horizontalHeader().setMinimumSectionSize(40)
        self.tableView.horizontalHeader().setStretchLastSection(True)
        self.tableView.verticalHeader().setMinimumSectionSize(3)
        self.tableView.verticalHeader().setStretchLastSection(True)

        self.gridLayout.addWidget(self.tableView, 1, 0, 1, 5)

        self.pushButton_insert = QPushButton(EditTableDialog)
        self.pushButton_insert.setObjectName(u"pushButton_insert")
        sizePolicy2.setHeightForWidth(self.pushButton_insert.sizePolicy().hasHeightForWidth())
        self.pushButton_insert.setSizePolicy(sizePolicy2)
        self.pushButton_insert.setMinimumSize(QSize(80, 0))
        self.pushButton_insert.setMaximumSize(QSize(80, 16777215))

        self.gridLayout.addWidget(self.pushButton_insert, 2, 0, 1, 2)

        self.comboBox_insert_line_type = QComboBox(EditTableDialog)
        self.comboBox_insert_line_type.setObjectName(u"comboBox_insert_line_type")
        sizePolicy2.setHeightForWidth(self.comboBox_insert_line_type.sizePolicy().hasHeightForWidth())
        self.comboBox_insert_line_type.setSizePolicy(sizePolicy2)
        self.comboBox_insert_line_type.setMinimumSize(QSize(120, 0))
        self.comboBox_insert_line_type.setMaximumSize(QSize(120, 16777215))

        self.gridLayout.addWidget(self.comboBox_insert_line_type, 2, 2, 1, 1)

        self.comboBox_insert_where = QComboBox(EditTableDialog)
        self.comboBox_insert_where.setObjectName(u"comboBox_insert_where")

        self.gridLayout.addWidget(self.comboBox_insert_where, 2, 3, 1, 1)

        self.horizontalSpacer = QSpacerItem(171, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 2, 4, 1, 1)


        self.retranslateUi(EditTableDialog)
        self.saveButton.clicked.connect(EditTableDialog.edit)
        self.cancelButton.clicked.connect(EditTableDialog.reject)
        self.helpButton.clicked.connect(EditTableDialog.help)
        self.pushButton_insert.clicked.connect(EditTableDialog.insert_line)
        self.pushButton_graph.clicked.connect(EditTableDialog.plot)

        QMetaObject.connectSlotsByName(EditTableDialog)
    # setupUi

    def retranslateUi(self, EditTableDialog):
        EditTableDialog.setWindowTitle(QCoreApplication.translate("EditTableDialog", u"Dialog", None))
        self.label_name.setText(QCoreApplication.translate("EditTableDialog", u"Name", None))
        self.saveButton.setText(QCoreApplication.translate("EditTableDialog", u"Save", None))
#if QT_CONFIG(shortcut)
        self.saveButton.setShortcut(QCoreApplication.translate("EditTableDialog", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_graph.setText(QCoreApplication.translate("EditTableDialog", u"Graph", None))
#if QT_CONFIG(shortcut)
        self.pushButton_graph.setShortcut(QCoreApplication.translate("EditTableDialog", u"F8", None))
#endif // QT_CONFIG(shortcut)
        self.cancelButton.setText(QCoreApplication.translate("EditTableDialog", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.cancelButton.setShortcut(QCoreApplication.translate("EditTableDialog", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.helpButton.setText(QCoreApplication.translate("EditTableDialog", u"Help", None))
#if QT_CONFIG(shortcut)
        self.helpButton.setShortcut(QCoreApplication.translate("EditTableDialog", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_insert.setText(QCoreApplication.translate("EditTableDialog", u"Insert", None))
#if QT_CONFIG(shortcut)
        self.pushButton_insert.setShortcut(QCoreApplication.translate("EditTableDialog", u"Ins", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

