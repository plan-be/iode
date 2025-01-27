# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'file_import_comments.ui'
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
    QGridLayout, QLabel, QPushButton, QSizePolicy,
    QSpacerItem, QVBoxLayout, QWidget)

from util.widgets.file_chooser import IodeFileChooser

class Ui_MenuFileImportComments(object):
    def setupUi(self, MenuFileImportComments):
        if not MenuFileImportComments.objectName():
            MenuFileImportComments.setObjectName(u"MenuFileImportComments")
        MenuFileImportComments.setWindowModality(Qt.ApplicationModal)
        MenuFileImportComments.resize(600, 280)
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(MenuFileImportComments.sizePolicy().hasHeightForWidth())
        MenuFileImportComments.setSizePolicy(sizePolicy)
        MenuFileImportComments.setMinimumSize(QSize(600, 280))
        MenuFileImportComments.setMaximumSize(QSize(600, 280))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuFileImportComments.setWindowIcon(icon)
        self.frame_top = QFrame(MenuFileImportComments)
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
        self.label_input_file = QLabel(self.layoutWidget)
        self.label_input_file.setObjectName(u"label_input_file")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Preferred)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.label_input_file.sizePolicy().hasHeightForWidth())
        self.label_input_file.setSizePolicy(sizePolicy1)

        self.gridLayout.addWidget(self.label_input_file, 0, 0, 1, 1)

        self.fileChooser_input_file = IodeFileChooser(self.layoutWidget)
        self.fileChooser_input_file.setObjectName(u"fileChooser_input_file")

        self.gridLayout.addWidget(self.fileChooser_input_file, 0, 1, 1, 2)

        self.label_rule_file = QLabel(self.layoutWidget)
        self.label_rule_file.setObjectName(u"label_rule_file")
        sizePolicy1.setHeightForWidth(self.label_rule_file.sizePolicy().hasHeightForWidth())
        self.label_rule_file.setSizePolicy(sizePolicy1)

        self.gridLayout.addWidget(self.label_rule_file, 1, 0, 1, 1)

        self.fileChooser_rule_file = IodeFileChooser(self.layoutWidget)
        self.fileChooser_rule_file.setObjectName(u"fileChooser_rule_file")

        self.gridLayout.addWidget(self.fileChooser_rule_file, 1, 1, 1, 2)

        self.frame_middle = QFrame(MenuFileImportComments)
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
        self.horizontalSpacer_3 = QSpacerItem(178, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_2.addItem(self.horizontalSpacer_3, 1, 4, 1, 2)

        self.fileChooser_save_file = IodeFileChooser(self.layoutWidget1)
        self.fileChooser_save_file.setObjectName(u"fileChooser_save_file")
        sizePolicy2 = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Fixed)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.fileChooser_save_file.sizePolicy().hasHeightForWidth())
        self.fileChooser_save_file.setSizePolicy(sizePolicy2)
        self.fileChooser_save_file.setMinimumSize(QSize(0, 20))

        self.gridLayout_2.addWidget(self.fileChooser_save_file, 0, 1, 1, 5)

        self.label_save = QLabel(self.layoutWidget1)
        self.label_save.setObjectName(u"label_save")

        self.gridLayout_2.addWidget(self.label_save, 0, 0, 1, 1)

        self.label_language = QLabel(self.layoutWidget1)
        self.label_language.setObjectName(u"label_language")

        self.gridLayout_2.addWidget(self.label_language, 1, 0, 1, 1)

        self.comboBox_language = QComboBox(self.layoutWidget1)
        self.comboBox_language.setObjectName(u"comboBox_language")
        self.comboBox_language.setMaxCount(10)

        self.gridLayout_2.addWidget(self.comboBox_language, 1, 1, 1, 3)

        self.frame_bottom = QFrame(MenuFileImportComments)
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
        self.layoutWidget_2 = QWidget(MenuFileImportComments)
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


        self.retranslateUi(MenuFileImportComments)
        self.helpButton.clicked.connect(MenuFileImportComments.help)
        self.cancelButton.clicked.connect(MenuFileImportComments.reject)
        self.openButton.clicked.connect(MenuFileImportComments.import)

        QMetaObject.connectSlotsByName(MenuFileImportComments)
    # setupUi

    def retranslateUi(self, MenuFileImportComments):
        MenuFileImportComments.setWindowTitle(QCoreApplication.translate("MenuFileImportComments", u"IMPORT  FILE  BY  RULE", None))
        self.label_input_file.setText(QCoreApplication.translate("MenuFileImportComments", u"Input file", None))
        self.label_rule_file.setText(QCoreApplication.translate("MenuFileImportComments", u"Rule file", None))
        self.label_save.setText(QCoreApplication.translate("MenuFileImportComments", u"Save in IODE file", None))
        self.label_language.setText(QCoreApplication.translate("MenuFileImportComments", u"Output Language", None))
        self.label_debug.setText(QCoreApplication.translate("MenuFileImportComments", u"Debug file", None))
        self.openButton.setText(QCoreApplication.translate("MenuFileImportComments", u"Open", None))
#if QT_CONFIG(shortcut)
        self.openButton.setShortcut(QCoreApplication.translate("MenuFileImportComments", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.cancelButton.setText(QCoreApplication.translate("MenuFileImportComments", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.cancelButton.setShortcut(QCoreApplication.translate("MenuFileImportComments", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.helpButton.setText(QCoreApplication.translate("MenuFileImportComments", u"Help", None))
#if QT_CONFIG(shortcut)
        self.helpButton.setShortcut(QCoreApplication.translate("MenuFileImportComments", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

