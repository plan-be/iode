# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'tab_report.ui'
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
from PySide6.QtWidgets import (QApplication, QComboBox, QGridLayout, QLineEdit,
    QPushButton, QSizePolicy, QSpacerItem, QSpinBox,
    QSplitter, QWidget)

from text_edit.report_editor import IodeReportEditor

class Ui_ReportWidget(object):
    def setupUi(self, ReportWidget):
        if not ReportWidget.objectName():
            ReportWidget.setObjectName(u"ReportWidget")
        ReportWidget.resize(684, 500)
        self.gridLayout = QGridLayout(ReportWidget)
        self.gridLayout.setObjectName(u"gridLayout")
        self.lineEdit_parameters = QLineEdit(ReportWidget)
        self.lineEdit_parameters.setObjectName(u"lineEdit_parameters")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.lineEdit_parameters.sizePolicy().hasHeightForWidth())
        self.lineEdit_parameters.setSizePolicy(sizePolicy)
        self.lineEdit_parameters.setMinimumSize(QSize(240, 0))

        self.gridLayout.addWidget(self.lineEdit_parameters, 0, 0, 1, 1)

        self.comboBox_language = QComboBox(ReportWidget)
        self.comboBox_language.setObjectName(u"comboBox_language")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.comboBox_language.sizePolicy().hasHeightForWidth())
        self.comboBox_language.setSizePolicy(sizePolicy1)
        self.comboBox_language.setMinimumSize(QSize(100, 0))

        self.gridLayout.addWidget(self.comboBox_language, 0, 1, 1, 1)

        self.spinBox_nbDecimals = QSpinBox(ReportWidget)
        self.spinBox_nbDecimals.setObjectName(u"spinBox_nbDecimals")
        sizePolicy1.setHeightForWidth(self.spinBox_nbDecimals.sizePolicy().hasHeightForWidth())
        self.spinBox_nbDecimals.setSizePolicy(sizePolicy1)
        self.spinBox_nbDecimals.setMinimumSize(QSize(40, 0))
        self.spinBox_nbDecimals.setMaximum(10)
        self.spinBox_nbDecimals.setValue(2)

        self.gridLayout.addWidget(self.spinBox_nbDecimals, 0, 2, 1, 1)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 0, 3, 1, 1)

        self.pushButton_print = QPushButton(ReportWidget)
        self.pushButton_print.setObjectName(u"pushButton_print")
        sizePolicy1.setHeightForWidth(self.pushButton_print.sizePolicy().hasHeightForWidth())
        self.pushButton_print.setSizePolicy(sizePolicy1)
        self.pushButton_print.setMinimumSize(QSize(60, 0))
        self.pushButton_print.setMaximumSize(QSize(60, 16777215))

        self.gridLayout.addWidget(self.pushButton_print, 0, 4, 1, 1)

        self.pushButton_run = QPushButton(ReportWidget)
        self.pushButton_run.setObjectName(u"pushButton_run")
        sizePolicy1.setHeightForWidth(self.pushButton_run.sizePolicy().hasHeightForWidth())
        self.pushButton_run.setSizePolicy(sizePolicy1)
        self.pushButton_run.setMinimumSize(QSize(60, 0))
        self.pushButton_run.setMaximumSize(QSize(60, 16777215))

        self.gridLayout.addWidget(self.pushButton_run, 0, 5, 1, 1)

        self.splitter = QSplitter(ReportWidget)
        self.splitter.setObjectName(u"splitter")
        self.splitter.setOrientation(Qt.Horizontal)
        self.editor = IodeReportEditor(self.splitter)
        self.editor.setObjectName(u"editor")
        self.splitter.addWidget(self.editor)
        self.editor_2 = IodeReportEditor(self.splitter)
        self.editor_2.setObjectName(u"editor_2")
        self.splitter.addWidget(self.editor_2)

        self.gridLayout.addWidget(self.splitter, 1, 0, 1, 6)


        self.retranslateUi(ReportWidget)
        self.pushButton_print.clicked.connect(ReportWidget.print)
        self.pushButton_run.clicked.connect(ReportWidget.run)
        self.lineEdit_parameters.returnPressed.connect(ReportWidget.run)

        QMetaObject.connectSlotsByName(ReportWidget)
    # setupUi

    def retranslateUi(self, ReportWidget):
        ReportWidget.setWindowTitle(QCoreApplication.translate("ReportWidget", u"Form", None))
        self.lineEdit_parameters.setPlaceholderText(QCoreApplication.translate("ReportWidget", u"report parameters here", None))
        self.pushButton_print.setText(QCoreApplication.translate("ReportWidget", u"Print", None))
#if QT_CONFIG(shortcut)
        self.pushButton_print.setShortcut(QCoreApplication.translate("ReportWidget", u"Ctrl+P", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_run.setText(QCoreApplication.translate("ReportWidget", u"Run", None))
#if QT_CONFIG(shortcut)
        self.pushButton_run.setShortcut(QCoreApplication.translate("ReportWidget", u"Ctrl+R", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

