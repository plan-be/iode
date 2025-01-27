# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'duplicate.ui'
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
    QSpacerItem, QVBoxLayout, QWidget)

from text_edit.complete_line_edit import IodeAutoCompleteLineEdit

class Ui_MenuDataDuplicateObj(object):
    def setupUi(self, MenuDataDuplicateObj):
        if not MenuDataDuplicateObj.objectName():
            MenuDataDuplicateObj.setObjectName(u"MenuDataDuplicateObj")
        MenuDataDuplicateObj.resize(406, 109)
        self.gridLayout = QGridLayout(MenuDataDuplicateObj)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_iode_types = QLabel(MenuDataDuplicateObj)
        self.label_iode_types.setObjectName(u"label_iode_types")
        self.label_iode_types.setMinimumSize(QSize(70, 20))
        self.label_iode_types.setMaximumSize(QSize(70, 20))
        self.label_iode_types.setAlignment(Qt.AlignLeading|Qt.AlignLeft|Qt.AlignVCenter)

        self.gridLayout.addWidget(self.label_iode_types, 0, 0, 1, 1)

        self.comboBox_iode_types = QComboBox(MenuDataDuplicateObj)
        self.comboBox_iode_types.setObjectName(u"comboBox_iode_types")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.comboBox_iode_types.sizePolicy().hasHeightForWidth())
        self.comboBox_iode_types.setSizePolicy(sizePolicy)
        self.comboBox_iode_types.setMinimumSize(QSize(150, 20))
        self.comboBox_iode_types.setMaximumSize(QSize(150, 20))

        self.gridLayout.addWidget(self.comboBox_iode_types, 0, 1, 1, 1)

        self.horizontalSpacer = QSpacerItem(70, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 0, 2, 1, 1)

        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.pushButton_duplicate = QPushButton(MenuDataDuplicateObj)
        self.pushButton_duplicate.setObjectName(u"pushButton_duplicate")

        self.verticalLayout.addWidget(self.pushButton_duplicate)

        self.pushButton_cancel = QPushButton(MenuDataDuplicateObj)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.verticalLayout.addWidget(self.pushButton_cancel)

        self.pushButton_help = QPushButton(MenuDataDuplicateObj)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.gridLayout.addLayout(self.verticalLayout, 0, 3, 3, 1)

        self.label_obj_name = QLabel(MenuDataDuplicateObj)
        self.label_obj_name.setObjectName(u"label_obj_name")

        self.gridLayout.addWidget(self.label_obj_name, 1, 0, 1, 1)

        self.lineEdit_obj_name = IodeAutoCompleteLineEdit(MenuDataDuplicateObj)
        self.lineEdit_obj_name.setObjectName(u"lineEdit_obj_name")

        self.gridLayout.addWidget(self.lineEdit_obj_name, 1, 1, 1, 2)

        self.label_dup_obj_name = QLabel(MenuDataDuplicateObj)
        self.label_dup_obj_name.setObjectName(u"label_dup_obj_name")

        self.gridLayout.addWidget(self.label_dup_obj_name, 2, 0, 1, 1)

        self.lineEdit_dup_obj_name = QLineEdit(MenuDataDuplicateObj)
        self.lineEdit_dup_obj_name.setObjectName(u"lineEdit_dup_obj_name")

        self.gridLayout.addWidget(self.lineEdit_dup_obj_name, 2, 1, 1, 2)


        self.retranslateUi(MenuDataDuplicateObj)
        self.pushButton_duplicate.clicked.connect(MenuDataDuplicateObj.duplicate)
        self.pushButton_cancel.clicked.connect(MenuDataDuplicateObj.reject)
        self.pushButton_help.clicked.connect(MenuDataDuplicateObj.help)

        QMetaObject.connectSlotsByName(MenuDataDuplicateObj)
    # setupUi

    def retranslateUi(self, MenuDataDuplicateObj):
        MenuDataDuplicateObj.setWindowTitle(QCoreApplication.translate("MenuDataDuplicateObj", u"DUPLICATE OBJECTS IN WS", None))
        self.label_iode_types.setText(QCoreApplication.translate("MenuDataDuplicateObj", u"Object Type", None))
        self.pushButton_duplicate.setText(QCoreApplication.translate("MenuDataDuplicateObj", u"Duplicate", None))
#if QT_CONFIG(shortcut)
        self.pushButton_duplicate.setShortcut(QCoreApplication.translate("MenuDataDuplicateObj", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuDataDuplicateObj", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuDataDuplicateObj", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuDataDuplicateObj", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuDataDuplicateObj", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label_obj_name.setText(QCoreApplication.translate("MenuDataDuplicateObj", u"Duplicate", None))
        self.label_dup_obj_name.setText(QCoreApplication.translate("MenuDataDuplicateObj", u"To", None))
    # retranslateUi

