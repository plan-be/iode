# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'dyn_adjustment.ui'
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
from PySide6.QtWidgets import (QApplication, QComboBox, QDialog, QGridLayout,
    QLabel, QLineEdit, QPushButton, QSizePolicy,
    QTextEdit, QVBoxLayout, QWidget)

class Ui_DynAdjustmentDialog(object):
    def setupUi(self, DynAdjustmentDialog):
        if not DynAdjustmentDialog.objectName():
            DynAdjustmentDialog.setObjectName(u"DynAdjustmentDialog")
        DynAdjustmentDialog.resize(623, 241)
        self.layoutWidget = QWidget(DynAdjustmentDialog)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(10, 10, 501, 211))
        self.gridLayout = QGridLayout(self.layoutWidget)
        self.gridLayout.setObjectName(u"gridLayout")
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.label_equation = QLabel(self.layoutWidget)
        self.label_equation.setObjectName(u"label_equation")

        self.gridLayout.addWidget(self.label_equation, 0, 0, 1, 1)

        self.textEdit_equation = QTextEdit(self.layoutWidget)
        self.textEdit_equation.setObjectName(u"textEdit_equation")
        self.textEdit_equation.setReadOnly(True)

        self.gridLayout.addWidget(self.textEdit_equation, 0, 1, 1, 1)

        self.label_method = QLabel(self.layoutWidget)
        self.label_method.setObjectName(u"label_method")

        self.gridLayout.addWidget(self.label_method, 1, 0, 1, 1)

        self.comboBox_method = QComboBox(self.layoutWidget)
        self.comboBox_method.setObjectName(u"comboBox_method")

        self.gridLayout.addWidget(self.comboBox_method, 1, 1, 1, 1)

        self.label_coefs = QLabel(self.layoutWidget)
        self.label_coefs.setObjectName(u"label_coefs")

        self.gridLayout.addWidget(self.label_coefs, 2, 0, 1, 1)

        self.lineEdit_coef1 = QLineEdit(self.layoutWidget)
        self.lineEdit_coef1.setObjectName(u"lineEdit_coef1")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.lineEdit_coef1.sizePolicy().hasHeightForWidth())
        self.lineEdit_coef1.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.lineEdit_coef1, 2, 1, 1, 1)

        self.lineEdit_coef2 = QLineEdit(self.layoutWidget)
        self.lineEdit_coef2.setObjectName(u"lineEdit_coef2")
        sizePolicy.setHeightForWidth(self.lineEdit_coef2.sizePolicy().hasHeightForWidth())
        self.lineEdit_coef2.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.lineEdit_coef2, 3, 1, 1, 1)

        self.layoutWidget1 = QWidget(DynAdjustmentDialog)
        self.layoutWidget1.setObjectName(u"layoutWidget1")
        self.layoutWidget1.setGeometry(QRect(530, 10, 77, 101))
        self.verticalLayout = QVBoxLayout(self.layoutWidget1)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.pushButton_adjust = QPushButton(self.layoutWidget1)
        self.pushButton_adjust.setObjectName(u"pushButton_adjust")

        self.verticalLayout.addWidget(self.pushButton_adjust)

        self.pushButton_quit = QPushButton(self.layoutWidget1)
        self.pushButton_quit.setObjectName(u"pushButton_quit")

        self.verticalLayout.addWidget(self.pushButton_quit)

        self.pushButton_help = QPushButton(self.layoutWidget1)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.retranslateUi(DynAdjustmentDialog)
        self.pushButton_adjust.clicked.connect(DynAdjustmentDialog.adjust)
        self.pushButton_help.clicked.connect(DynAdjustmentDialog.help)
        self.pushButton_quit.clicked.connect(DynAdjustmentDialog.reject)

        QMetaObject.connectSlotsByName(DynAdjustmentDialog)
    # setupUi

    def retranslateUi(self, DynAdjustmentDialog):
        DynAdjustmentDialog.setWindowTitle(QCoreApplication.translate("DynAdjustmentDialog", u"Dynamic Adjustment", None))
        self.label_equation.setText(QCoreApplication.translate("DynAdjustmentDialog", u"Equation", None))
        self.label_method.setText(QCoreApplication.translate("DynAdjustmentDialog", u"Method", None))
        self.label_coefs.setText(QCoreApplication.translate("DynAdjustmentDialog", u"Coefficients", None))
        self.lineEdit_coef1.setText(QCoreApplication.translate("DynAdjustmentDialog", u"c1", None))
        self.lineEdit_coef2.setText(QCoreApplication.translate("DynAdjustmentDialog", u"c2", None))
        self.pushButton_adjust.setText(QCoreApplication.translate("DynAdjustmentDialog", u"Adjust", None))
#if QT_CONFIG(shortcut)
        self.pushButton_adjust.setShortcut(QCoreApplication.translate("DynAdjustmentDialog", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_quit.setText(QCoreApplication.translate("DynAdjustmentDialog", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_quit.setShortcut(QCoreApplication.translate("DynAdjustmentDialog", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("DynAdjustmentDialog", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("DynAdjustmentDialog", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

