# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'workspace_seasonal_adjustment.ui'
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
from PySide6.QtWidgets import (QApplication, QDialog, QDoubleSpinBox, QGridLayout,
    QLabel, QPushButton, QSizePolicy, QTextEdit,
    QVBoxLayout, QWidget)

from util.widgets.file_chooser import IodeFileChooser

class Ui_MenuWorkspaceSeasonalAdjustment(object):
    def setupUi(self, MenuWorkspaceSeasonalAdjustment):
        if not MenuWorkspaceSeasonalAdjustment.objectName():
            MenuWorkspaceSeasonalAdjustment.setObjectName(u"MenuWorkspaceSeasonalAdjustment")
        MenuWorkspaceSeasonalAdjustment.resize(600, 240)
        MenuWorkspaceSeasonalAdjustment.setMinimumSize(QSize(600, 240))
        MenuWorkspaceSeasonalAdjustment.setMaximumSize(QSize(600, 240))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuWorkspaceSeasonalAdjustment.setWindowIcon(icon)
        self.layoutWidget = QWidget(MenuWorkspaceSeasonalAdjustment)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(10, 20, 471, 201))
        self.gridLayout = QGridLayout(self.layoutWidget)
        self.gridLayout.setObjectName(u"gridLayout")
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.label_input_file = QLabel(self.layoutWidget)
        self.label_input_file.setObjectName(u"label_input_file")

        self.gridLayout.addWidget(self.label_input_file, 0, 0, 1, 1)

        self.label_series = QLabel(self.layoutWidget)
        self.label_series.setObjectName(u"label_series")

        self.gridLayout.addWidget(self.label_series, 1, 0, 1, 1)

        self.textEdit_series = QTextEdit(self.layoutWidget)
        self.textEdit_series.setObjectName(u"textEdit_series")

        self.gridLayout.addWidget(self.textEdit_series, 1, 1, 1, 2)

        self.label_EPS_test = QLabel(self.layoutWidget)
        self.label_EPS_test.setObjectName(u"label_EPS_test")

        self.gridLayout.addWidget(self.label_EPS_test, 2, 0, 1, 1)

        self.doubleSpinBox_EPS_test = QDoubleSpinBox(self.layoutWidget)
        self.doubleSpinBox_EPS_test.setObjectName(u"doubleSpinBox_EPS_test")
        self.doubleSpinBox_EPS_test.setValue(5.000000000000000)

        self.gridLayout.addWidget(self.doubleSpinBox_EPS_test, 2, 1, 1, 1)

        self.label = QLabel(self.layoutWidget)
        self.label.setObjectName(u"label")

        self.gridLayout.addWidget(self.label, 2, 2, 1, 1)

        self.fileChooser_input_file = IodeFileChooser(self.layoutWidget)
        self.fileChooser_input_file.setObjectName(u"fileChooser_input_file")

        self.gridLayout.addWidget(self.fileChooser_input_file, 0, 1, 1, 2)

        self.verticalLayoutWidget = QWidget(MenuWorkspaceSeasonalAdjustment)
        self.verticalLayoutWidget.setObjectName(u"verticalLayoutWidget")
        self.verticalLayoutWidget.setGeometry(QRect(490, 20, 91, 91))
        self.verticalLayout = QVBoxLayout(self.verticalLayoutWidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.pushButton_go = QPushButton(self.verticalLayoutWidget)
        self.pushButton_go.setObjectName(u"pushButton_go")

        self.verticalLayout.addWidget(self.pushButton_go)

        self.pushButton_cancel = QPushButton(self.verticalLayoutWidget)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.verticalLayout.addWidget(self.pushButton_cancel)

        self.pushButton_help = QPushButton(self.verticalLayoutWidget)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.retranslateUi(MenuWorkspaceSeasonalAdjustment)
        self.pushButton_go.clicked.connect(MenuWorkspaceSeasonalAdjustment.seasonal_adjustment)
        self.pushButton_cancel.clicked.connect(MenuWorkspaceSeasonalAdjustment.reject)
        self.pushButton_help.clicked.connect(MenuWorkspaceSeasonalAdjustment.help)

        QMetaObject.connectSlotsByName(MenuWorkspaceSeasonalAdjustment)
    # setupUi

    def retranslateUi(self, MenuWorkspaceSeasonalAdjustment):
        MenuWorkspaceSeasonalAdjustment.setWindowTitle(QCoreApplication.translate("MenuWorkspaceSeasonalAdjustment", u"CENSUS II SEASONAL ADJUSTMENT", None))
        self.label_input_file.setText(QCoreApplication.translate("MenuWorkspaceSeasonalAdjustment", u"Input file", None))
        self.label_series.setText(QCoreApplication.translate("MenuWorkspaceSeasonalAdjustment", u"Series", None))
        self.label_EPS_test.setText(QCoreApplication.translate("MenuWorkspaceSeasonalAdjustment", u"EPS-Test", None))
        self.label.setText("")
        self.pushButton_go.setText(QCoreApplication.translate("MenuWorkspaceSeasonalAdjustment", u"Go", None))
#if QT_CONFIG(shortcut)
        self.pushButton_go.setShortcut(QCoreApplication.translate("MenuWorkspaceSeasonalAdjustment", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuWorkspaceSeasonalAdjustment", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuWorkspaceSeasonalAdjustment", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuWorkspaceSeasonalAdjustment", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuWorkspaceSeasonalAdjustment", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

