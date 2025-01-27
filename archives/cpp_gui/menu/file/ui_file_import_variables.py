# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'file_import_variables.ui'
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
    QVBoxLayout, QWidget)

from util.widgets.file_chooser import IodeFileChooser
from util.widgets.sample_edit import IodeSampleEdit

class Ui_MenuFileImportVariables(object):
    def setupUi(self, MenuFileImportVariables):
        if not MenuFileImportVariables.objectName():
            MenuFileImportVariables.setObjectName(u"MenuFileImportVariables")
        MenuFileImportVariables.setWindowModality(Qt.ApplicationModal)
        MenuFileImportVariables.resize(600, 280)
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(MenuFileImportVariables.sizePolicy().hasHeightForWidth())
        MenuFileImportVariables.setSizePolicy(sizePolicy)
        MenuFileImportVariables.setMinimumSize(QSize(600, 280))
        MenuFileImportVariables.setMaximumSize(QSize(600, 280))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuFileImportVariables.setWindowIcon(icon)
        self.frame_top = QFrame(MenuFileImportVariables)
        self.frame_top.setObjectName(u"frame_top")
        self.frame_top.setGeometry(QRect(10, 10, 481, 91))
        self.frame_top.setFrameShape(QFrame.Box)
        self.frame_top.setFrameShadow(QFrame.Raised)
        self.layoutWidget = QWidget(self.frame_top)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(10, 10, 461, 71))
        self.gridLayout = QGridLayout(self.layoutWidget)
        self.gridLayout.setObjectName(u"gridLayout")
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.fileChooser_input_file = IodeFileChooser(self.layoutWidget)
        self.fileChooser_input_file.setObjectName(u"fileChooser_input_file")

        self.gridLayout.addWidget(self.fileChooser_input_file, 0, 1, 1, 2)

        self.label_rule_file = QLabel(self.layoutWidget)
        self.label_rule_file.setObjectName(u"label_rule_file")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Preferred)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.label_rule_file.sizePolicy().hasHeightForWidth())
        self.label_rule_file.setSizePolicy(sizePolicy1)

        self.gridLayout.addWidget(self.label_rule_file, 1, 0, 1, 1)

        self.label_input_file = QLabel(self.layoutWidget)
        self.label_input_file.setObjectName(u"label_input_file")
        sizePolicy1.setHeightForWidth(self.label_input_file.sizePolicy().hasHeightForWidth())
        self.label_input_file.setSizePolicy(sizePolicy1)

        self.gridLayout.addWidget(self.label_input_file, 0, 0, 1, 1)

        self.fileChooser_rule_file = IodeFileChooser(self.layoutWidget)
        self.fileChooser_rule_file.setObjectName(u"fileChooser_rule_file")

        self.gridLayout.addWidget(self.fileChooser_rule_file, 1, 1, 1, 2)

        self.frame_middle = QFrame(MenuFileImportVariables)
        self.frame_middle.setObjectName(u"frame_middle")
        self.frame_middle.setGeometry(QRect(10, 100, 481, 101))
        self.frame_middle.setFrameShape(QFrame.Box)
        self.frame_middle.setFrameShadow(QFrame.Raised)
        self.layoutWidget1 = QWidget(self.frame_middle)
        self.layoutWidget1.setObjectName(u"layoutWidget1")
        self.layoutWidget1.setGeometry(QRect(10, 10, 461, 81))
        self.gridLayout_2 = QGridLayout(self.layoutWidget1)
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.gridLayout_2.setContentsMargins(0, 0, 0, 0)
        self.sampleEdit_sample_to = IodeSampleEdit(self.layoutWidget1)
        self.sampleEdit_sample_to.setObjectName(u"sampleEdit_sample_to")

        self.gridLayout_2.addWidget(self.sampleEdit_sample_to, 1, 5, 1, 2)

        self.label_sample_to = QLabel(self.layoutWidget1)
        self.label_sample_to.setObjectName(u"label_sample_to")

        self.gridLayout_2.addWidget(self.label_sample_to, 1, 4, 1, 1)

        self.horizontalSpacer_2 = QSpacerItem(148, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_2.addItem(self.horizontalSpacer_2, 1, 7, 1, 1)

        self.sampleEdit_sample_from = IodeSampleEdit(self.layoutWidget1)
        self.sampleEdit_sample_from.setObjectName(u"sampleEdit_sample_from")

        self.gridLayout_2.addWidget(self.sampleEdit_sample_from, 1, 3, 1, 1)

        self.fileChooser_save_file = IodeFileChooser(self.layoutWidget1)
        self.fileChooser_save_file.setObjectName(u"fileChooser_save_file")
        sizePolicy2 = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Fixed)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.fileChooser_save_file.sizePolicy().hasHeightForWidth())
        self.fileChooser_save_file.setSizePolicy(sizePolicy2)
        self.fileChooser_save_file.setMinimumSize(QSize(0, 20))

        self.gridLayout_2.addWidget(self.fileChooser_save_file, 0, 1, 1, 7)

        self.label_sample = QLabel(self.layoutWidget1)
        self.label_sample.setObjectName(u"label_sample")

        self.gridLayout_2.addWidget(self.label_sample, 1, 0, 1, 1)

        self.label_sample_from = QLabel(self.layoutWidget1)
        self.label_sample_from.setObjectName(u"label_sample_from")

        self.gridLayout_2.addWidget(self.label_sample_from, 1, 1, 1, 2)

        self.label_save = QLabel(self.layoutWidget1)
        self.label_save.setObjectName(u"label_save")

        self.gridLayout_2.addWidget(self.label_save, 0, 0, 1, 1)

        self.frame_bottom = QFrame(MenuFileImportVariables)
        self.frame_bottom.setObjectName(u"frame_bottom")
        self.frame_bottom.setGeometry(QRect(10, 200, 481, 60))
        self.frame_bottom.setFrameShape(QFrame.Box)
        self.frame_bottom.setFrameShadow(QFrame.Raised)
        self.label_debug = QLabel(self.frame_bottom)
        self.label_debug.setObjectName(u"label_debug")
        self.label_debug.setGeometry(QRect(10, 20, 61, 16))
        self.fileChooser_debug_file = IodeFileChooser(self.frame_bottom)
        self.fileChooser_debug_file.setObjectName(u"fileChooser_debug_file")
        self.fileChooser_debug_file.setGeometry(QRect(80, 20, 391, 21))
        self.layoutWidget_2 = QWidget(MenuFileImportVariables)
        self.layoutWidget_2.setObjectName(u"layoutWidget_2")
        self.layoutWidget_2.setGeometry(QRect(510, 10, 77, 251))
        self.verticalLayout = QVBoxLayout(self.layoutWidget_2)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.openButton = QPushButton(self.layoutWidget_2)
        self.openButton.setObjectName(u"openButton")
        sizePolicy3 = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy3.setHorizontalStretch(0)
        sizePolicy3.setVerticalStretch(0)
        sizePolicy3.setHeightForWidth(self.openButton.sizePolicy().hasHeightForWidth())
        self.openButton.setSizePolicy(sizePolicy3)

        self.verticalLayout.addWidget(self.openButton)

        self.cancelButton = QPushButton(self.layoutWidget_2)
        self.cancelButton.setObjectName(u"cancelButton")
        sizePolicy3.setHeightForWidth(self.cancelButton.sizePolicy().hasHeightForWidth())
        self.cancelButton.setSizePolicy(sizePolicy3)

        self.verticalLayout.addWidget(self.cancelButton)

        self.helpButton = QPushButton(self.layoutWidget_2)
        self.helpButton.setObjectName(u"helpButton")
        sizePolicy3.setHeightForWidth(self.helpButton.sizePolicy().hasHeightForWidth())
        self.helpButton.setSizePolicy(sizePolicy3)

        self.verticalLayout.addWidget(self.helpButton)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer)


        self.retranslateUi(MenuFileImportVariables)
        self.cancelButton.clicked.connect(MenuFileImportVariables.reject)
        self.helpButton.clicked.connect(MenuFileImportVariables.help)
        self.openButton.clicked.connect(MenuFileImportVariables.import)

        QMetaObject.connectSlotsByName(MenuFileImportVariables)
    # setupUi

    def retranslateUi(self, MenuFileImportVariables):
        MenuFileImportVariables.setWindowTitle(QCoreApplication.translate("MenuFileImportVariables", u"IMPORT  FILE  BY  RULE", None))
        self.label_rule_file.setText(QCoreApplication.translate("MenuFileImportVariables", u"Rule file", None))
        self.label_input_file.setText(QCoreApplication.translate("MenuFileImportVariables", u"Input file", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setToolTip(QCoreApplication.translate("MenuFileImportVariables", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setInputMask("")
        self.sampleEdit_sample_to.setPlaceholderText("")
        self.label_sample_to.setText(QCoreApplication.translate("MenuFileImportVariables", u"To", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setToolTip(QCoreApplication.translate("MenuFileImportVariables", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setInputMask("")
        self.sampleEdit_sample_from.setPlaceholderText("")
        self.label_sample.setText(QCoreApplication.translate("MenuFileImportVariables", u"Output sample", None))
        self.label_sample_from.setText(QCoreApplication.translate("MenuFileImportVariables", u"From", None))
        self.label_save.setText(QCoreApplication.translate("MenuFileImportVariables", u"Save in IODE file", None))
        self.label_debug.setText(QCoreApplication.translate("MenuFileImportVariables", u"Debug file", None))
        self.openButton.setText(QCoreApplication.translate("MenuFileImportVariables", u"Open", None))
#if QT_CONFIG(shortcut)
        self.openButton.setShortcut(QCoreApplication.translate("MenuFileImportVariables", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.cancelButton.setText(QCoreApplication.translate("MenuFileImportVariables", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.cancelButton.setShortcut(QCoreApplication.translate("MenuFileImportVariables", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.helpButton.setText(QCoreApplication.translate("MenuFileImportVariables", u"Help", None))
#if QT_CONFIG(shortcut)
        self.helpButton.setShortcut(QCoreApplication.translate("MenuFileImportVariables", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

