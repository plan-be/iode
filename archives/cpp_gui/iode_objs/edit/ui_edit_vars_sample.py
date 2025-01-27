# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'edit_vars_sample.ui'
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
    QHBoxLayout, QLabel, QPlainTextEdit, QPushButton,
    QSizePolicy, QSpacerItem, QWidget)

from util.widgets.sample_edit import IodeSampleEdit

class Ui_EditIodeSampleDialog(object):
    def setupUi(self, EditIodeSampleDialog):
        if not EditIodeSampleDialog.objectName():
            EditIodeSampleDialog.setObjectName(u"EditIodeSampleDialog")
        EditIodeSampleDialog.resize(449, 229)
        EditIodeSampleDialog.setModal(False)
        self.gridLayout = QGridLayout(EditIodeSampleDialog)
        self.gridLayout.setObjectName(u"gridLayout")
        self.horizontalLayout_3 = QHBoxLayout()
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.horizontalSpacer_4 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout_3.addItem(self.horizontalSpacer_4)

        self.labelTitle = QLabel(EditIodeSampleDialog)
        self.labelTitle.setObjectName(u"labelTitle")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.labelTitle.sizePolicy().hasHeightForWidth())
        self.labelTitle.setSizePolicy(sizePolicy)
        self.labelTitle.setMinimumSize(QSize(150, 0))
        self.labelTitle.setMaximumSize(QSize(150, 16777215))
        font = QFont()
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        self.labelTitle.setFont(font)
        self.labelTitle.setFrameShape(QFrame.StyledPanel)
        self.labelTitle.setFrameShadow(QFrame.Raised)
        self.labelTitle.setMidLineWidth(0)
        self.labelTitle.setAlignment(Qt.AlignCenter)

        self.horizontalLayout_3.addWidget(self.labelTitle)

        self.horizontalSpacer = QSpacerItem(108, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout_3.addItem(self.horizontalSpacer)


        self.gridLayout.addLayout(self.horizontalLayout_3, 0, 0, 1, 1)

        self.saveButton = QPushButton(EditIodeSampleDialog)
        self.saveButton.setObjectName(u"saveButton")
        sizePolicy.setHeightForWidth(self.saveButton.sizePolicy().hasHeightForWidth())
        self.saveButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.saveButton, 0, 1, 1, 1)

        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.horizontalSpacer_5 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout_2.addItem(self.horizontalSpacer_5)

        self.label_from = QLabel(EditIodeSampleDialog)
        self.label_from.setObjectName(u"label_from")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Preferred)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.label_from.sizePolicy().hasHeightForWidth())
        self.label_from.setSizePolicy(sizePolicy1)
        self.label_from.setMinimumSize(QSize(40, 0))
        self.label_from.setMaximumSize(QSize(40, 16777215))

        self.horizontalLayout_2.addWidget(self.label_from)

        self.sampleEdit_from = IodeSampleEdit(EditIodeSampleDialog)
        self.sampleEdit_from.setObjectName(u"sampleEdit_from")
        sizePolicy.setHeightForWidth(self.sampleEdit_from.sizePolicy().hasHeightForWidth())
        self.sampleEdit_from.setSizePolicy(sizePolicy)
        self.sampleEdit_from.setMinimumSize(QSize(120, 0))
        self.sampleEdit_from.setMaximumSize(QSize(120, 16777215))

        self.horizontalLayout_2.addWidget(self.sampleEdit_from)

        self.label_to = QLabel(EditIodeSampleDialog)
        self.label_to.setObjectName(u"label_to")
        sizePolicy1.setHeightForWidth(self.label_to.sizePolicy().hasHeightForWidth())
        self.label_to.setSizePolicy(sizePolicy1)
        self.label_to.setMinimumSize(QSize(20, 0))
        self.label_to.setMaximumSize(QSize(20, 16777215))

        self.horizontalLayout_2.addWidget(self.label_to)

        self.sampleEdit_to = IodeSampleEdit(EditIodeSampleDialog)
        self.sampleEdit_to.setObjectName(u"sampleEdit_to")
        self.sampleEdit_to.setMinimumSize(QSize(120, 0))
        self.sampleEdit_to.setMaximumSize(QSize(120, 16777215))

        self.horizontalLayout_2.addWidget(self.sampleEdit_to)

        self.horizontalSpacer_6 = QSpacerItem(38, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout_2.addItem(self.horizontalSpacer_6)


        self.gridLayout.addLayout(self.horizontalLayout_2, 1, 0, 1, 1)

        self.cancelButton = QPushButton(EditIodeSampleDialog)
        self.cancelButton.setObjectName(u"cancelButton")
        sizePolicy.setHeightForWidth(self.cancelButton.sizePolicy().hasHeightForWidth())
        self.cancelButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.cancelButton, 1, 1, 1, 1)

        self.plainTextEdit = QPlainTextEdit(EditIodeSampleDialog)
        self.plainTextEdit.setObjectName(u"plainTextEdit")
        self.plainTextEdit.setOverwriteMode(True)

        self.gridLayout.addWidget(self.plainTextEdit, 2, 0, 2, 1)

        self.helpButton = QPushButton(EditIodeSampleDialog)
        self.helpButton.setObjectName(u"helpButton")
        sizePolicy.setHeightForWidth(self.helpButton.sizePolicy().hasHeightForWidth())
        self.helpButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.helpButton, 2, 1, 1, 1)

        self.verticalSpacer = QSpacerItem(20, 116, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 3, 1, 1, 1)


        self.retranslateUi(EditIodeSampleDialog)
        self.helpButton.clicked.connect(EditIodeSampleDialog.help)
        self.saveButton.clicked.connect(EditIodeSampleDialog.edit)
        self.cancelButton.clicked.connect(EditIodeSampleDialog.reject)

        QMetaObject.connectSlotsByName(EditIodeSampleDialog)
    # setupUi

    def retranslateUi(self, EditIodeSampleDialog):
        EditIodeSampleDialog.setWindowTitle(QCoreApplication.translate("EditIodeSampleDialog", u"Variables Sample", None))
        self.labelTitle.setText(QCoreApplication.translate("EditIodeSampleDialog", u"Variables Sample", None))
        self.saveButton.setText(QCoreApplication.translate("EditIodeSampleDialog", u"OK", None))
#if QT_CONFIG(shortcut)
        self.saveButton.setShortcut(QCoreApplication.translate("EditIodeSampleDialog", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.label_from.setText(QCoreApplication.translate("EditIodeSampleDialog", u"From", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_from.setToolTip(QCoreApplication.translate("EditIodeSampleDialog", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_from.setInputMask("")
        self.sampleEdit_from.setPlaceholderText("")
        self.label_to.setText(QCoreApplication.translate("EditIodeSampleDialog", u"To", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_to.setToolTip(QCoreApplication.translate("EditIodeSampleDialog", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_to.setInputMask("")
        self.sampleEdit_to.setPlaceholderText("")
        self.cancelButton.setText(QCoreApplication.translate("EditIodeSampleDialog", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.cancelButton.setShortcut(QCoreApplication.translate("EditIodeSampleDialog", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.plainTextEdit.setPlainText(QCoreApplication.translate("EditIodeSampleDialog", u"Sample format : yyyyPppp              \n"
"       - yyyy is the year             \n"
"       - P is the periodicity :       \n"
"           Y=Yearly, Q=Quaterly,      \n"
"           M=Monthly, W=Weekly        \n"
"       - ppp is the period in the year", None))
        self.helpButton.setText(QCoreApplication.translate("EditIodeSampleDialog", u"Help", None))
#if QT_CONFIG(shortcut)
        self.helpButton.setShortcut(QCoreApplication.translate("EditIodeSampleDialog", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

