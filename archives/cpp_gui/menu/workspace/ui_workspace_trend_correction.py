# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'workspace_trend_correction.ui'
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
from PySide6.QtWidgets import (QApplication, QCheckBox, QDialog, QDoubleSpinBox,
    QGridLayout, QLabel, QPushButton, QSizePolicy,
    QTextEdit, QVBoxLayout, QWidget)

from util.widgets.file_chooser import IodeFileChooser

class Ui_MenuWorkspaceTrendCorrection(object):
    def setupUi(self, MenuWorkspaceTrendCorrection):
        if not MenuWorkspaceTrendCorrection.objectName():
            MenuWorkspaceTrendCorrection.setObjectName(u"MenuWorkspaceTrendCorrection")
        MenuWorkspaceTrendCorrection.resize(600, 300)
        MenuWorkspaceTrendCorrection.setMinimumSize(QSize(600, 300))
        MenuWorkspaceTrendCorrection.setMaximumSize(QSize(600, 300))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuWorkspaceTrendCorrection.setWindowIcon(icon)
        self.layoutWidget = QWidget(MenuWorkspaceTrendCorrection)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(12, 21, 471, 266))
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

        self.gridLayout.addWidget(self.textEdit_series, 1, 1, 1, 3)

        self.label_lambda = QLabel(self.layoutWidget)
        self.label_lambda.setObjectName(u"label_lambda")

        self.gridLayout.addWidget(self.label_lambda, 2, 0, 1, 1)

        self.label = QLabel(self.layoutWidget)
        self.label.setObjectName(u"label")

        self.gridLayout.addWidget(self.label, 2, 3, 1, 1)

        self.label_log = QLabel(self.layoutWidget)
        self.label_log.setObjectName(u"label_log")

        self.gridLayout.addWidget(self.label_log, 3, 0, 1, 1)

        self.label_2 = QLabel(self.layoutWidget)
        self.label_2.setObjectName(u"label_2")

        self.gridLayout.addWidget(self.label_2, 3, 3, 1, 1)

        self.checkBox_log = QCheckBox(self.layoutWidget)
        self.checkBox_log.setObjectName(u"checkBox_log")

        self.gridLayout.addWidget(self.checkBox_log, 3, 1, 1, 2)

        self.fileChooser_input_file = IodeFileChooser(self.layoutWidget)
        self.fileChooser_input_file.setObjectName(u"fileChooser_input_file")

        self.gridLayout.addWidget(self.fileChooser_input_file, 0, 1, 1, 3)

        self.doubleSpinBox_lambda = QDoubleSpinBox(self.layoutWidget)
        self.doubleSpinBox_lambda.setObjectName(u"doubleSpinBox_lambda")
        self.doubleSpinBox_lambda.setMaximum(1000000.000000000000000)
        self.doubleSpinBox_lambda.setValue(100.000000000000000)

        self.gridLayout.addWidget(self.doubleSpinBox_lambda, 2, 1, 1, 2)

        self.verticalLayoutWidget = QWidget(MenuWorkspaceTrendCorrection)
        self.verticalLayoutWidget.setObjectName(u"verticalLayoutWidget")
        self.verticalLayoutWidget.setGeometry(QRect(500, 20, 91, 91))
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


        self.retranslateUi(MenuWorkspaceTrendCorrection)
        self.pushButton_go.clicked.connect(MenuWorkspaceTrendCorrection.trend_correction)
        self.pushButton_cancel.clicked.connect(MenuWorkspaceTrendCorrection.reject)
        self.pushButton_help.clicked.connect(MenuWorkspaceTrendCorrection.help)

        QMetaObject.connectSlotsByName(MenuWorkspaceTrendCorrection)
    # setupUi

    def retranslateUi(self, MenuWorkspaceTrendCorrection):
        MenuWorkspaceTrendCorrection.setWindowTitle(QCoreApplication.translate("MenuWorkspaceTrendCorrection", u"TREND - HODRICK - PRESCOTT FILTER", None))
        self.label_input_file.setText(QCoreApplication.translate("MenuWorkspaceTrendCorrection", u"Input file", None))
        self.label_series.setText(QCoreApplication.translate("MenuWorkspaceTrendCorrection", u"Series", None))
        self.label_lambda.setText(QCoreApplication.translate("MenuWorkspaceTrendCorrection", u"Lambda", None))
        self.label.setText("")
        self.label_log.setText(QCoreApplication.translate("MenuWorkspaceTrendCorrection", u"Log", None))
        self.label_2.setText("")
        self.checkBox_log.setText("")
        self.pushButton_go.setText(QCoreApplication.translate("MenuWorkspaceTrendCorrection", u"Go", None))
#if QT_CONFIG(shortcut)
        self.pushButton_go.setShortcut(QCoreApplication.translate("MenuWorkspaceTrendCorrection", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuWorkspaceTrendCorrection", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuWorkspaceTrendCorrection", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuWorkspaceTrendCorrection", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuWorkspaceTrendCorrection", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

