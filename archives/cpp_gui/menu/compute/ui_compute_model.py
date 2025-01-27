# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'compute_model.ui'
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
from PySide6.QtWidgets import (QApplication, QDialog, QFrame, QGridLayout,
    QLabel, QPushButton, QSizePolicy, QSpacerItem,
    QWidget)

from text_edit.complete_text_edit import IodeAutoCompleteTextEdit

class Ui_MenuComputeModel(object):
    def setupUi(self, MenuComputeModel):
        if not MenuComputeModel.objectName():
            MenuComputeModel.setObjectName(u"MenuComputeModel")
        MenuComputeModel.resize(600, 200)
        MenuComputeModel.setMinimumSize(QSize(600, 200))
        MenuComputeModel.setMaximumSize(QSize(600, 200))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuComputeModel.setWindowIcon(icon)
        self.gridLayout = QGridLayout(MenuComputeModel)
        self.gridLayout.setObjectName(u"gridLayout")
        self.frame = QFrame(MenuComputeModel)
        self.frame.setObjectName(u"frame")
        self.frame.setFrameShape(QFrame.Panel)
        self.frame.setFrameShadow(QFrame.Raised)
        self.label_info = QLabel(self.frame)
        self.label_info.setObjectName(u"label_info")
        self.label_info.setGeometry(QRect(40, 10, 421, 51))
        self.label_info.setAlignment(Qt.AlignJustify|Qt.AlignVCenter)

        self.gridLayout.addWidget(self.frame, 0, 0, 3, 2)

        self.OKButton = QPushButton(MenuComputeModel)
        self.OKButton.setObjectName(u"OKButton")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.OKButton.sizePolicy().hasHeightForWidth())
        self.OKButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.OKButton, 0, 2, 1, 1)

        self.cancelButton = QPushButton(MenuComputeModel)
        self.cancelButton.setObjectName(u"cancelButton")
        sizePolicy.setHeightForWidth(self.cancelButton.sizePolicy().hasHeightForWidth())
        self.cancelButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.cancelButton, 1, 2, 1, 1)

        self.helpButton = QPushButton(MenuComputeModel)
        self.helpButton.setObjectName(u"helpButton")
        sizePolicy.setHeightForWidth(self.helpButton.sizePolicy().hasHeightForWidth())
        self.helpButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.helpButton, 2, 2, 2, 1)

        self.label_equations_list = QLabel(MenuComputeModel)
        self.label_equations_list.setObjectName(u"label_equations_list")
        self.label_equations_list.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_equations_list, 3, 0, 2, 1)

        self.textEdit_equations_list = IodeAutoCompleteTextEdit(MenuComputeModel)
        self.textEdit_equations_list.setObjectName(u"textEdit_equations_list")

        self.gridLayout.addWidget(self.textEdit_equations_list, 3, 1, 2, 1)

        self.verticalSpacer = QSpacerItem(20, 89, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 4, 2, 1, 1)


        self.retranslateUi(MenuComputeModel)
        self.helpButton.clicked.connect(MenuComputeModel.help)
        self.cancelButton.clicked.connect(MenuComputeModel.reject)
        self.OKButton.clicked.connect(MenuComputeModel.compute)

        QMetaObject.connectSlotsByName(MenuComputeModel)
    # setupUi

    def retranslateUi(self, MenuComputeModel):
        MenuComputeModel.setWindowTitle(QCoreApplication.translate("MenuComputeModel", u"COMPILE MODEL", None))
        self.label_info.setText(QCoreApplication.translate("MenuComputeModel", u"Recompiles the given set of equations using the list defined in the workspace.\n"
"This can be necessary if you use lists in your equation definitions and you load\n"
"another file of lists.", None))
        self.OKButton.setText(QCoreApplication.translate("MenuComputeModel", u"OK", None))
#if QT_CONFIG(shortcut)
        self.OKButton.setShortcut(QCoreApplication.translate("MenuComputeModel", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.cancelButton.setText(QCoreApplication.translate("MenuComputeModel", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.cancelButton.setShortcut(QCoreApplication.translate("MenuComputeModel", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.helpButton.setText(QCoreApplication.translate("MenuComputeModel", u"Help", None))
#if QT_CONFIG(shortcut)
        self.helpButton.setShortcut(QCoreApplication.translate("MenuComputeModel", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label_equations_list.setText(QCoreApplication.translate("MenuComputeModel", u"Equations List\n"
"\n"
"(empty for\n"
"all equations)", None))
    # retranslateUi

