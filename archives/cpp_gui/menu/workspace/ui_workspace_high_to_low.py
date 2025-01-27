# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'workspace_high_to_low.ui'
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
from PySide6.QtWidgets import (QApplication, QDialog, QGridLayout, QLabel,
    QPushButton, QSizePolicy, QTextEdit, QVBoxLayout,
    QWidget)

from util.widgets.file_chooser import IodeFileChooser

class Ui_MenuWorkspaceHighToLow(object):
    def setupUi(self, MenuWorkspaceHighToLow):
        if not MenuWorkspaceHighToLow.objectName():
            MenuWorkspaceHighToLow.setObjectName(u"MenuWorkspaceHighToLow")
        MenuWorkspaceHighToLow.resize(600, 380)
        MenuWorkspaceHighToLow.setMinimumSize(QSize(600, 380))
        MenuWorkspaceHighToLow.setMaximumSize(QSize(600, 380))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuWorkspaceHighToLow.setWindowIcon(icon)
        MenuWorkspaceHighToLow.setModal(False)
        self.layoutWidget = QWidget(MenuWorkspaceHighToLow)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(10, 20, 471, 341))
        self.gridLayout = QGridLayout(self.layoutWidget)
        self.gridLayout.setObjectName(u"gridLayout")
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.label_input_file = QLabel(self.layoutWidget)
        self.label_input_file.setObjectName(u"label_input_file")

        self.gridLayout.addWidget(self.label_input_file, 0, 0, 1, 1)

        self.fileChooser_input_file = IodeFileChooser(self.layoutWidget)
        self.fileChooser_input_file.setObjectName(u"fileChooser_input_file")

        self.gridLayout.addWidget(self.fileChooser_input_file, 0, 1, 1, 1)

        self.label_method = QLabel(self.layoutWidget)
        self.label_method.setObjectName(u"label_method")
        self.label_method.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_method, 1, 0, 1, 1)

        self.label_list_variables = QLabel(self.layoutWidget)
        self.label_list_variables.setObjectName(u"label_list_variables")
        self.label_list_variables.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_list_variables, 1, 1, 1, 1)

        self.label_last_obsvervation = QLabel(self.layoutWidget)
        self.label_last_obsvervation.setObjectName(u"label_last_obsvervation")

        self.gridLayout.addWidget(self.label_last_obsvervation, 2, 0, 1, 1)

        self.textEdit_last_observation = QTextEdit(self.layoutWidget)
        self.textEdit_last_observation.setObjectName(u"textEdit_last_observation")

        self.gridLayout.addWidget(self.textEdit_last_observation, 2, 1, 1, 1)

        self.label_mean = QLabel(self.layoutWidget)
        self.label_mean.setObjectName(u"label_mean")

        self.gridLayout.addWidget(self.label_mean, 3, 0, 1, 1)

        self.textEdit_mean = QTextEdit(self.layoutWidget)
        self.textEdit_mean.setObjectName(u"textEdit_mean")

        self.gridLayout.addWidget(self.textEdit_mean, 3, 1, 1, 1)

        self.label_sum = QLabel(self.layoutWidget)
        self.label_sum.setObjectName(u"label_sum")

        self.gridLayout.addWidget(self.label_sum, 4, 0, 1, 1)

        self.textEdit_sum = QTextEdit(self.layoutWidget)
        self.textEdit_sum.setObjectName(u"textEdit_sum")

        self.gridLayout.addWidget(self.textEdit_sum, 4, 1, 1, 1)

        self.verticalLayoutWidget = QWidget(MenuWorkspaceHighToLow)
        self.verticalLayoutWidget.setObjectName(u"verticalLayoutWidget")
        self.verticalLayoutWidget.setGeometry(QRect(500, 20, 91, 91))
        self.verticalLayout = QVBoxLayout(self.verticalLayoutWidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.pushButton_htol = QPushButton(self.verticalLayoutWidget)
        self.pushButton_htol.setObjectName(u"pushButton_htol")

        self.verticalLayout.addWidget(self.pushButton_htol)

        self.pushButton_cancel = QPushButton(self.verticalLayoutWidget)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.verticalLayout.addWidget(self.pushButton_cancel)

        self.pushButton_help = QPushButton(self.verticalLayoutWidget)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.retranslateUi(MenuWorkspaceHighToLow)
        self.pushButton_cancel.clicked.connect(MenuWorkspaceHighToLow.reject)
        self.pushButton_help.clicked.connect(MenuWorkspaceHighToLow.help)
        self.pushButton_htol.clicked.connect(MenuWorkspaceHighToLow.slot_high_to_low)

        QMetaObject.connectSlotsByName(MenuWorkspaceHighToLow)
    # setupUi

    def retranslateUi(self, MenuWorkspaceHighToLow):
        MenuWorkspaceHighToLow.setWindowTitle(QCoreApplication.translate("MenuWorkspaceHighToLow", u"HIGH TO LOW", None))
        self.label_input_file.setText(QCoreApplication.translate("MenuWorkspaceHighToLow", u"Input file", None))
        self.label_method.setText(QCoreApplication.translate("MenuWorkspaceHighToLow", u"Method", None))
        self.label_list_variables.setText(QCoreApplication.translate("MenuWorkspaceHighToLow", u"List of Variables", None))
        self.label_last_obsvervation.setText(QCoreApplication.translate("MenuWorkspaceHighToLow", u"Last Observation", None))
        self.label_mean.setText(QCoreApplication.translate("MenuWorkspaceHighToLow", u"Mean", None))
        self.label_sum.setText(QCoreApplication.translate("MenuWorkspaceHighToLow", u"Sum", None))
        self.pushButton_htol.setText(QCoreApplication.translate("MenuWorkspaceHighToLow", u"OK", None))
#if QT_CONFIG(shortcut)
        self.pushButton_htol.setShortcut(QCoreApplication.translate("MenuWorkspaceHighToLow", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuWorkspaceHighToLow", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuWorkspaceHighToLow", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuWorkspaceHighToLow", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuWorkspaceHighToLow", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

