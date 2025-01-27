# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'add_object.ui'
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
from PySide6.QtWidgets import (QApplication, QDialog, QFrame, QHBoxLayout,
    QLabel, QLineEdit, QPushButton, QSizePolicy,
    QSpacerItem, QVBoxLayout, QWidget)

class Ui_AddObjectDialog(object):
    def setupUi(self, AddObjectDialog):
        if not AddObjectDialog.objectName():
            AddObjectDialog.setObjectName(u"AddObjectDialog")
        AddObjectDialog.setWindowModality(Qt.ApplicationModal)
        AddObjectDialog.resize(599, 182)
        AddObjectDialog.setModal(True)
        self.layoutWidget = QWidget(AddObjectDialog)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(11, 11, 571, 161))
        self.verticalLayout = QVBoxLayout(self.layoutWidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.label_title = QLabel(self.layoutWidget)
        self.label_title.setObjectName(u"label_title")
        palette = QPalette()
        brush = QBrush(QColor(0, 120, 215, 255))
        brush.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.Highlight, brush)
        brush1 = QBrush(QColor(240, 240, 240, 255))
        brush1.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Inactive, QPalette.Highlight, brush1)
        palette.setBrush(QPalette.Disabled, QPalette.Highlight, brush)
        self.label_title.setPalette(palette)
        font = QFont()
        font.setPointSize(12)
        font.setBold(False)
        font.setStrikeOut(False)
        self.label_title.setFont(font)
        self.label_title.setAutoFillBackground(False)
        self.label_title.setFrameShape(QFrame.StyledPanel)
        self.label_title.setFrameShadow(QFrame.Raised)
        self.label_title.setLineWidth(2)
        self.label_title.setTextFormat(Qt.AutoText)
        self.label_title.setAlignment(Qt.AlignCenter)
        self.label_title.setWordWrap(False)

        self.verticalLayout.addWidget(self.label_title)

        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.label_name = QLabel(self.layoutWidget)
        self.label_name.setObjectName(u"label_name")

        self.horizontalLayout.addWidget(self.label_name)

        self.lineEdit_name = QLineEdit(self.layoutWidget)
        self.lineEdit_name.setObjectName(u"lineEdit_name")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.lineEdit_name.sizePolicy().hasHeightForWidth())
        self.lineEdit_name.setSizePolicy(sizePolicy)

        self.horizontalLayout.addWidget(self.lineEdit_name)

        self.spacer = QSpacerItem(318, 31, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout.addItem(self.spacer)


        self.verticalLayout.addLayout(self.horizontalLayout)

        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.label_definition = QLabel(self.layoutWidget)
        self.label_definition.setObjectName(u"label_definition")

        self.horizontalLayout_2.addWidget(self.label_definition)

        self.lineEdit_definition = QLineEdit(self.layoutWidget)
        self.lineEdit_definition.setObjectName(u"lineEdit_definition")

        self.horizontalLayout_2.addWidget(self.lineEdit_definition)


        self.verticalLayout.addLayout(self.horizontalLayout_2)

        self.horizontalLayout_3 = QHBoxLayout()
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout_3.addItem(self.horizontalSpacer)

        self.okButton = QPushButton(self.layoutWidget)
        self.okButton.setObjectName(u"okButton")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.okButton.sizePolicy().hasHeightForWidth())
        self.okButton.setSizePolicy(sizePolicy1)

        self.horizontalLayout_3.addWidget(self.okButton)

        self.cancelButton = QPushButton(self.layoutWidget)
        self.cancelButton.setObjectName(u"cancelButton")
        sizePolicy1.setHeightForWidth(self.cancelButton.sizePolicy().hasHeightForWidth())
        self.cancelButton.setSizePolicy(sizePolicy1)

        self.horizontalLayout_3.addWidget(self.cancelButton)

        self.helpButton = QPushButton(self.layoutWidget)
        self.helpButton.setObjectName(u"helpButton")
        sizePolicy1.setHeightForWidth(self.helpButton.sizePolicy().hasHeightForWidth())
        self.helpButton.setSizePolicy(sizePolicy1)

        self.horizontalLayout_3.addWidget(self.helpButton)

        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout_3.addItem(self.horizontalSpacer_2)


        self.verticalLayout.addLayout(self.horizontalLayout_3)


        self.retranslateUi(AddObjectDialog)
        self.cancelButton.clicked.connect(AddObjectDialog.reject)
        self.okButton.clicked.connect(AddObjectDialog.add)
        self.helpButton.clicked.connect(AddObjectDialog.help)

        QMetaObject.connectSlotsByName(AddObjectDialog)
    # setupUi

    def retranslateUi(self, AddObjectDialog):
        AddObjectDialog.setWindowTitle(QCoreApplication.translate("AddObjectDialog", u"Add IODE Object", None))
        self.label_title.setText(QCoreApplication.translate("AddObjectDialog", u"Add Iode Object", None))
        self.label_name.setText(QCoreApplication.translate("AddObjectDialog", u"Name", None))
        self.label_definition.setText(QCoreApplication.translate("AddObjectDialog", u"Definition", None))
        self.okButton.setText(QCoreApplication.translate("AddObjectDialog", u"Save", None))
#if QT_CONFIG(shortcut)
        self.okButton.setShortcut(QCoreApplication.translate("AddObjectDialog", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.cancelButton.setText(QCoreApplication.translate("AddObjectDialog", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.cancelButton.setShortcut(QCoreApplication.translate("AddObjectDialog", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.helpButton.setText(QCoreApplication.translate("AddObjectDialog", u"Help", None))
#if QT_CONFIG(shortcut)
        self.helpButton.setShortcut(QCoreApplication.translate("AddObjectDialog", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

