# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'sort_expand_list.ui'
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
    QVBoxLayout, QWidget)

from text_edit.complete_line_edit import IodeAutoCompleteLineEdit

class Ui_MenuDataSortExpandList(object):
    def setupUi(self, MenuDataSortExpandList):
        if not MenuDataSortExpandList.objectName():
            MenuDataSortExpandList.setObjectName(u"MenuDataSortExpandList")
        MenuDataSortExpandList.resize(470, 120)
        MenuDataSortExpandList.setMinimumSize(QSize(470, 120))
        MenuDataSortExpandList.setMaximumSize(QSize(470, 120))
        self.gridLayout = QGridLayout(MenuDataSortExpandList)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_sort = QLabel(MenuDataSortExpandList)
        self.label_sort.setObjectName(u"label_sort")

        self.gridLayout.addWidget(self.label_sort, 0, 0, 1, 1)

        self.lineEdit_sort = IodeAutoCompleteLineEdit(MenuDataSortExpandList)
        self.lineEdit_sort.setObjectName(u"lineEdit_sort")

        self.gridLayout.addWidget(self.lineEdit_sort, 0, 1, 1, 1)

        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.pushButton_sort = QPushButton(MenuDataSortExpandList)
        self.pushButton_sort.setObjectName(u"pushButton_sort")

        self.verticalLayout.addWidget(self.pushButton_sort)

        self.pushButton_cancel = QPushButton(MenuDataSortExpandList)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.verticalLayout.addWidget(self.pushButton_cancel)

        self.pushButton_help = QPushButton(MenuDataSortExpandList)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.gridLayout.addLayout(self.verticalLayout, 0, 2, 3, 1)

        self.label_save = QLabel(MenuDataSortExpandList)
        self.label_save.setObjectName(u"label_save")

        self.gridLayout.addWidget(self.label_save, 1, 0, 1, 1)

        self.lineEdit_save = QLineEdit(MenuDataSortExpandList)
        self.lineEdit_save.setObjectName(u"lineEdit_save")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.lineEdit_save.sizePolicy().hasHeightForWidth())
        self.lineEdit_save.setSizePolicy(sizePolicy)
        self.lineEdit_save.setMinimumSize(QSize(270, 20))
        self.lineEdit_save.setMaximumSize(QSize(10000, 20))

        self.gridLayout.addWidget(self.lineEdit_save, 1, 1, 1, 1)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 2, 1, 1, 1)


        self.retranslateUi(MenuDataSortExpandList)
        self.pushButton_sort.clicked.connect(MenuDataSortExpandList.sort)
        self.pushButton_cancel.clicked.connect(MenuDataSortExpandList.reject)
        self.pushButton_help.clicked.connect(MenuDataSortExpandList.help)

        QMetaObject.connectSlotsByName(MenuDataSortExpandList)
    # setupUi

    def retranslateUi(self, MenuDataSortExpandList):
        MenuDataSortExpandList.setWindowTitle(QCoreApplication.translate("MenuDataSortExpandList", u"SORT LIST", None))
        self.label_sort.setText(QCoreApplication.translate("MenuDataSortExpandList", u"Sort", None))
        self.pushButton_sort.setText(QCoreApplication.translate("MenuDataSortExpandList", u"OK", None))
#if QT_CONFIG(shortcut)
        self.pushButton_sort.setShortcut(QCoreApplication.translate("MenuDataSortExpandList", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuDataSortExpandList", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuDataSortExpandList", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuDataSortExpandList", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuDataSortExpandList", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label_save.setText(QCoreApplication.translate("MenuDataSortExpandList", u"Save", None))
    # retranslateUi

