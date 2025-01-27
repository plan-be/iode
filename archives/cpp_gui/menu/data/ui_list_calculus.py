# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'list_calculus.ui'
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
from PySide6.QtWidgets import (QApplication, QComboBox, QDialog, QFrame,
    QGridLayout, QGroupBox, QLabel, QLineEdit,
    QPushButton, QSizePolicy, QSpacerItem, QTextEdit,
    QVBoxLayout, QWidget)

from text_edit.complete_line_edit import IodeAutoCompleteLineEdit

class Ui_MenuDataListCalculus(object):
    def setupUi(self, MenuDataListCalculus):
        if not MenuDataListCalculus.objectName():
            MenuDataListCalculus.setObjectName(u"MenuDataListCalculus")
        MenuDataListCalculus.resize(468, 286)
        self.gridLayout = QGridLayout(MenuDataListCalculus)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_sort = QLabel(MenuDataListCalculus)
        self.label_sort.setObjectName(u"label_sort")

        self.gridLayout.addWidget(self.label_sort, 0, 0, 1, 1)

        self.lineEdit_list1 = IodeAutoCompleteLineEdit(MenuDataListCalculus)
        self.lineEdit_list1.setObjectName(u"lineEdit_list1")

        self.gridLayout.addWidget(self.lineEdit_list1, 0, 1, 1, 1)

        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.pushButton_exec = QPushButton(MenuDataListCalculus)
        self.pushButton_exec.setObjectName(u"pushButton_exec")

        self.verticalLayout.addWidget(self.pushButton_exec)

        self.pushButton_cancel = QPushButton(MenuDataListCalculus)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.verticalLayout.addWidget(self.pushButton_cancel)

        self.pushButton_help = QPushButton(MenuDataListCalculus)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.gridLayout.addLayout(self.verticalLayout, 0, 2, 4, 1)

        self.comboBox_operator = QComboBox(MenuDataListCalculus)
        self.comboBox_operator.setObjectName(u"comboBox_operator")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.comboBox_operator.sizePolicy().hasHeightForWidth())
        self.comboBox_operator.setSizePolicy(sizePolicy)
        self.comboBox_operator.setMinimumSize(QSize(40, 20))
        self.comboBox_operator.setMaximumSize(QSize(40, 20))

        self.gridLayout.addWidget(self.comboBox_operator, 1, 0, 1, 1)

        self.lineEdit_list2 = IodeAutoCompleteLineEdit(MenuDataListCalculus)
        self.lineEdit_list2.setObjectName(u"lineEdit_list2")

        self.gridLayout.addWidget(self.lineEdit_list2, 1, 1, 1, 1)

        self.line = QFrame(MenuDataListCalculus)
        self.line.setObjectName(u"line")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.line.sizePolicy().hasHeightForWidth())
        self.line.setSizePolicy(sizePolicy1)
        self.line.setFrameShape(QFrame.Shape.HLine)
        self.line.setFrameShadow(QFrame.Shadow.Sunken)

        self.gridLayout.addWidget(self.line, 2, 0, 1, 2)

        self.label = QLabel(MenuDataListCalculus)
        self.label.setObjectName(u"label")
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)
        self.label.setMinimumSize(QSize(40, 20))
        self.label.setMaximumSize(QSize(40, 20))
        self.label.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label, 3, 0, 1, 1)

        self.lineEdit_list_res = QLineEdit(MenuDataListCalculus)
        self.lineEdit_list_res.setObjectName(u"lineEdit_list_res")
        sizePolicy1.setHeightForWidth(self.lineEdit_list_res.sizePolicy().hasHeightForWidth())
        self.lineEdit_list_res.setSizePolicy(sizePolicy1)
        self.lineEdit_list_res.setMinimumSize(QSize(270, 20))
        self.lineEdit_list_res.setMaximumSize(QSize(10000, 20))
        self.lineEdit_list_res.setReadOnly(False)

        self.gridLayout.addWidget(self.lineEdit_list_res, 3, 1, 1, 1)

        self.groupBox = QGroupBox(MenuDataListCalculus)
        self.groupBox.setObjectName(u"groupBox")
        self.groupBox.setFlat(False)
        self.label_count_text = QLabel(self.groupBox)
        self.label_count_text.setObjectName(u"label_count_text")
        self.label_count_text.setGeometry(QRect(10, 30, 49, 16))
        self.label_count = QLabel(self.groupBox)
        self.label_count.setObjectName(u"label_count")
        self.label_count.setGeometry(QRect(70, 30, 271, 16))
        sizePolicy2 = QSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Preferred)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.label_count.sizePolicy().hasHeightForWidth())
        self.label_count.setSizePolicy(sizePolicy2)
        self.textEdit_res = QTextEdit(self.groupBox)
        self.textEdit_res.setObjectName(u"textEdit_res")
        self.textEdit_res.setGeometry(QRect(10, 50, 351, 91))

        self.gridLayout.addWidget(self.groupBox, 5, 0, 1, 2)

        self.verticalSpacer = QSpacerItem(74, 147, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 5, 2, 1, 1)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 4, 0, 1, 3)


        self.retranslateUi(MenuDataListCalculus)
        self.pushButton_exec.clicked.connect(MenuDataListCalculus.calculus)
        self.pushButton_cancel.clicked.connect(MenuDataListCalculus.reject)
        self.pushButton_help.clicked.connect(MenuDataListCalculus.help)

        QMetaObject.connectSlotsByName(MenuDataListCalculus)
    # setupUi

    def retranslateUi(self, MenuDataListCalculus):
        MenuDataListCalculus.setWindowTitle(QCoreApplication.translate("MenuDataListCalculus", u"LIST CALCULUS", None))
        self.label_sort.setText("")
        self.pushButton_exec.setText(QCoreApplication.translate("MenuDataListCalculus", u"Exec", None))
#if QT_CONFIG(shortcut)
        self.pushButton_exec.setShortcut(QCoreApplication.translate("MenuDataListCalculus", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuDataListCalculus", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuDataListCalculus", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuDataListCalculus", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuDataListCalculus", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label.setText(QCoreApplication.translate("MenuDataListCalculus", u"=", None))
        self.lineEdit_list_res.setText(QCoreApplication.translate("MenuDataListCalculus", u"_RES", None))
        self.groupBox.setTitle(QCoreApplication.translate("MenuDataListCalculus", u"Results", None))
        self.label_count_text.setText(QCoreApplication.translate("MenuDataListCalculus", u"Count", None))
        self.label_count.setText(QCoreApplication.translate("MenuDataListCalculus", u"0", None))
    # retranslateUi

