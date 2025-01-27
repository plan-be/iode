# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'unit_root.ui'
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
    QLineEdit, QPushButton, QSizePolicy, QSpacerItem,
    QSpinBox, QTextEdit, QWidget)

class Ui_UnitRootDialog(object):
    def setupUi(self, UnitRootDialog):
        if not UnitRootDialog.objectName():
            UnitRootDialog.setObjectName(u"UnitRootDialog")
        UnitRootDialog.setWindowModality(Qt.ApplicationModal)
        UnitRootDialog.resize(621, 475)
        self.gridLayout = QGridLayout(UnitRootDialog)
        self.gridLayout.setObjectName(u"gridLayout")
        self.pushButton_help = QPushButton(UnitRootDialog)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.gridLayout.addWidget(self.pushButton_help, 2, 3, 1, 1)

        self.horizontalSpacer = QSpacerItem(389, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 1, 2, 1, 1)

        self.textEdit_results = QTextEdit(UnitRootDialog)
        self.textEdit_results.setObjectName(u"textEdit_results")
        self.textEdit_results.setReadOnly(True)

        self.gridLayout.addWidget(self.textEdit_results, 2, 0, 3, 3)

        self.label_order = QLabel(UnitRootDialog)
        self.label_order.setObjectName(u"label_order")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_order.sizePolicy().hasHeightForWidth())
        self.label_order.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.label_order, 1, 0, 1, 1)

        self.lineEdit_series = QLineEdit(UnitRootDialog)
        self.lineEdit_series.setObjectName(u"lineEdit_series")

        self.gridLayout.addWidget(self.lineEdit_series, 0, 1, 1, 2)

        self.label_series = QLabel(UnitRootDialog)
        self.label_series.setObjectName(u"label_series")
        sizePolicy.setHeightForWidth(self.label_series.sizePolicy().hasHeightForWidth())
        self.label_series.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.label_series, 0, 0, 1, 1)

        self.pushButton_quit = QPushButton(UnitRootDialog)
        self.pushButton_quit.setObjectName(u"pushButton_quit")

        self.gridLayout.addWidget(self.pushButton_quit, 1, 3, 1, 1)

        self.pushButton_analyse = QPushButton(UnitRootDialog)
        self.pushButton_analyse.setObjectName(u"pushButton_analyse")

        self.gridLayout.addWidget(self.pushButton_analyse, 0, 3, 1, 1)

        self.spinBox_order = QSpinBox(UnitRootDialog)
        self.spinBox_order.setObjectName(u"spinBox_order")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Preferred)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.spinBox_order.sizePolicy().hasHeightForWidth())
        self.spinBox_order.setSizePolicy(sizePolicy1)
        self.spinBox_order.setMinimumSize(QSize(80, 0))
        self.spinBox_order.setMinimum(0)
        self.spinBox_order.setMaximum(10)
        self.spinBox_order.setValue(0)

        self.gridLayout.addWidget(self.spinBox_order, 1, 1, 1, 1)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 3, 3, 1, 1)


        self.retranslateUi(UnitRootDialog)
        self.pushButton_analyse.clicked.connect(UnitRootDialog.analyse)
        self.pushButton_quit.clicked.connect(UnitRootDialog.reject)
        self.pushButton_help.clicked.connect(UnitRootDialog.help)

        QMetaObject.connectSlotsByName(UnitRootDialog)
    # setupUi

    def retranslateUi(self, UnitRootDialog):
        UnitRootDialog.setWindowTitle(QCoreApplication.translate("UnitRootDialog", u"Unit Root", None))
        self.pushButton_help.setText(QCoreApplication.translate("UnitRootDialog", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("UnitRootDialog", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label_order.setText(QCoreApplication.translate("UnitRootDialog", u"Order  ", None))
        self.lineEdit_series.setPlaceholderText(QCoreApplication.translate("UnitRootDialog", u"Equation name or LEC expression", None))
        self.label_series.setText(QCoreApplication.translate("UnitRootDialog", u"Series  ", None))
        self.pushButton_quit.setText(QCoreApplication.translate("UnitRootDialog", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_quit.setShortcut(QCoreApplication.translate("UnitRootDialog", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_analyse.setText(QCoreApplication.translate("UnitRootDialog", u"Analyse", None))
#if QT_CONFIG(shortcut)
        self.pushButton_analyse.setShortcut(QCoreApplication.translate("UnitRootDialog", u"F10", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

