# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'add_scalar.ui'
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
from PySide6.QtWidgets import (QApplication, QDialog, QDoubleSpinBox, QFrame,
    QHBoxLayout, QLabel, QLineEdit, QPushButton,
    QSizePolicy, QSpacerItem, QVBoxLayout, QWidget)

class Ui_AddScalarDialog(object):
    def setupUi(self, AddScalarDialog):
        if not AddScalarDialog.objectName():
            AddScalarDialog.setObjectName(u"AddScalarDialog")
        AddScalarDialog.resize(485, 161)
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(AddScalarDialog.sizePolicy().hasHeightForWidth())
        AddScalarDialog.setSizePolicy(sizePolicy)
        self.layoutWidget = QWidget(AddScalarDialog)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(11, 11, 461, 141))
        self.verticalLayout = QVBoxLayout(self.layoutWidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.titleLabel = QLabel(self.layoutWidget)
        self.titleLabel.setObjectName(u"titleLabel")
        palette = QPalette()
        brush = QBrush(QColor(0, 120, 215, 255))
        brush.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.Highlight, brush)
        brush1 = QBrush(QColor(240, 240, 240, 255))
        brush1.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Inactive, QPalette.Highlight, brush1)
        palette.setBrush(QPalette.Disabled, QPalette.Highlight, brush)
        self.titleLabel.setPalette(palette)
        font = QFont()
        font.setPointSize(12)
        font.setBold(False)
        font.setStrikeOut(False)
        self.titleLabel.setFont(font)
        self.titleLabel.setAutoFillBackground(False)
        self.titleLabel.setFrameShape(QFrame.StyledPanel)
        self.titleLabel.setFrameShadow(QFrame.Raised)
        self.titleLabel.setLineWidth(2)
        self.titleLabel.setTextFormat(Qt.AutoText)
        self.titleLabel.setAlignment(Qt.AlignCenter)
        self.titleLabel.setWordWrap(False)

        self.verticalLayout.addWidget(self.titleLabel)

        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.label_name = QLabel(self.layoutWidget)
        self.label_name.setObjectName(u"label_name")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Preferred)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.label_name.sizePolicy().hasHeightForWidth())
        self.label_name.setSizePolicy(sizePolicy1)

        self.horizontalLayout.addWidget(self.label_name)

        self.lineEdit_name = QLineEdit(self.layoutWidget)
        self.lineEdit_name.setObjectName(u"lineEdit_name")
        sizePolicy2 = QSizePolicy(QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Fixed)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.lineEdit_name.sizePolicy().hasHeightForWidth())
        self.lineEdit_name.setSizePolicy(sizePolicy2)

        self.horizontalLayout.addWidget(self.lineEdit_name)

        self.label_value = QLabel(self.layoutWidget)
        self.label_value.setObjectName(u"label_value")
        sizePolicy1.setHeightForWidth(self.label_value.sizePolicy().hasHeightForWidth())
        self.label_value.setSizePolicy(sizePolicy1)

        self.horizontalLayout.addWidget(self.label_value)

        self.lineEdit_value = QLineEdit(self.layoutWidget)
        self.lineEdit_value.setObjectName(u"lineEdit_value")
        sizePolicy2.setHeightForWidth(self.lineEdit_value.sizePolicy().hasHeightForWidth())
        self.lineEdit_value.setSizePolicy(sizePolicy2)

        self.horizontalLayout.addWidget(self.lineEdit_value)

        self.label_relax = QLabel(self.layoutWidget)
        self.label_relax.setObjectName(u"label_relax")
        sizePolicy1.setHeightForWidth(self.label_relax.sizePolicy().hasHeightForWidth())
        self.label_relax.setSizePolicy(sizePolicy1)

        self.horizontalLayout.addWidget(self.label_relax)

        self.doubleSpinBox_relax = QDoubleSpinBox(self.layoutWidget)
        self.doubleSpinBox_relax.setObjectName(u"doubleSpinBox_relax")
        sizePolicy3 = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Fixed)
        sizePolicy3.setHorizontalStretch(0)
        sizePolicy3.setVerticalStretch(0)
        sizePolicy3.setHeightForWidth(self.doubleSpinBox_relax.sizePolicy().hasHeightForWidth())
        self.doubleSpinBox_relax.setSizePolicy(sizePolicy3)
        self.doubleSpinBox_relax.setMinimumSize(QSize(60, 0))
        self.doubleSpinBox_relax.setMaximum(1.000000000000000)
        self.doubleSpinBox_relax.setSingleStep(0.010000000000000)
        self.doubleSpinBox_relax.setValue(1.000000000000000)

        self.horizontalLayout.addWidget(self.doubleSpinBox_relax)


        self.verticalLayout.addLayout(self.horizontalLayout)

        self.horizontalLayout_3 = QHBoxLayout()
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout_3.addItem(self.horizontalSpacer)

        self.okButton = QPushButton(self.layoutWidget)
        self.okButton.setObjectName(u"okButton")
        sizePolicy.setHeightForWidth(self.okButton.sizePolicy().hasHeightForWidth())
        self.okButton.setSizePolicy(sizePolicy)

        self.horizontalLayout_3.addWidget(self.okButton)

        self.cancelButton = QPushButton(self.layoutWidget)
        self.cancelButton.setObjectName(u"cancelButton")
        sizePolicy.setHeightForWidth(self.cancelButton.sizePolicy().hasHeightForWidth())
        self.cancelButton.setSizePolicy(sizePolicy)

        self.horizontalLayout_3.addWidget(self.cancelButton)

        self.helpButton = QPushButton(self.layoutWidget)
        self.helpButton.setObjectName(u"helpButton")
        sizePolicy.setHeightForWidth(self.helpButton.sizePolicy().hasHeightForWidth())
        self.helpButton.setSizePolicy(sizePolicy)

        self.horizontalLayout_3.addWidget(self.helpButton)

        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout_3.addItem(self.horizontalSpacer_2)


        self.verticalLayout.addLayout(self.horizontalLayout_3)


        self.retranslateUi(AddScalarDialog)
        self.okButton.clicked.connect(AddScalarDialog.add)
        self.cancelButton.clicked.connect(AddScalarDialog.reject)
        self.helpButton.clicked.connect(AddScalarDialog.help)

        QMetaObject.connectSlotsByName(AddScalarDialog)
    # setupUi

    def retranslateUi(self, AddScalarDialog):
        AddScalarDialog.setWindowTitle(QCoreApplication.translate("AddScalarDialog", u"Add Scalar", None))
        self.titleLabel.setText(QCoreApplication.translate("AddScalarDialog", u"Add Scalar", None))
        self.label_name.setText(QCoreApplication.translate("AddScalarDialog", u"Name", None))
        self.label_value.setText(QCoreApplication.translate("AddScalarDialog", u"Value", None))
        self.label_relax.setText(QCoreApplication.translate("AddScalarDialog", u"Relax", None))
        self.okButton.setText(QCoreApplication.translate("AddScalarDialog", u"Save", None))
#if QT_CONFIG(shortcut)
        self.okButton.setShortcut(QCoreApplication.translate("AddScalarDialog", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.cancelButton.setText(QCoreApplication.translate("AddScalarDialog", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.cancelButton.setShortcut(QCoreApplication.translate("AddScalarDialog", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.helpButton.setText(QCoreApplication.translate("AddScalarDialog", u"Help", None))
#if QT_CONFIG(shortcut)
        self.helpButton.setShortcut(QCoreApplication.translate("AddScalarDialog", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

