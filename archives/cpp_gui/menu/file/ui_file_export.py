# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'file_export.ui'
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
from PySide6.QtWidgets import (QApplication, QComboBox, QDialog, QFormLayout,
    QFrame, QGridLayout, QLabel, QLineEdit,
    QPushButton, QSizePolicy, QSpacerItem, QVBoxLayout,
    QWidget)

from util.widgets.file_chooser import IodeFileChooser
from util.widgets.sample_edit import IodeSampleEdit

class Ui_MenuFileExport(object):
    def setupUi(self, MenuFileExport):
        if not MenuFileExport.objectName():
            MenuFileExport.setObjectName(u"MenuFileExport")
        MenuFileExport.resize(640, 320)
        MenuFileExport.setMinimumSize(QSize(640, 320))
        MenuFileExport.setMaximumSize(QSize(640, 320))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuFileExport.setWindowIcon(icon)
        self.frame_top = QFrame(MenuFileExport)
        self.frame_top.setObjectName(u"frame_top")
        self.frame_top.setGeometry(QRect(10, 10, 521, 121))
        self.frame_top.setFrameShape(QFrame.Box)
        self.frame_top.setFrameShadow(QFrame.Raised)
        self.layoutWidget = QWidget(self.frame_top)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(10, 5, 501, 111))
        self.formLayout = QFormLayout(self.layoutWidget)
        self.formLayout.setObjectName(u"formLayout")
        self.formLayout.setContentsMargins(0, 0, 0, 0)
        self.label_variable_file = QLabel(self.layoutWidget)
        self.label_variable_file.setObjectName(u"label_variable_file")

        self.formLayout.setWidget(0, QFormLayout.LabelRole, self.label_variable_file)

        self.fileChooser_variable_file = IodeFileChooser(self.layoutWidget)
        self.fileChooser_variable_file.setObjectName(u"fileChooser_variable_file")
        self.fileChooser_variable_file.setMinimumSize(QSize(0, 30))

        self.formLayout.setWidget(0, QFormLayout.FieldRole, self.fileChooser_variable_file)

        self.label_comment_file = QLabel(self.layoutWidget)
        self.label_comment_file.setObjectName(u"label_comment_file")

        self.formLayout.setWidget(1, QFormLayout.LabelRole, self.label_comment_file)

        self.fileChooser_comment_file = IodeFileChooser(self.layoutWidget)
        self.fileChooser_comment_file.setObjectName(u"fileChooser_comment_file")
        self.fileChooser_comment_file.setMinimumSize(QSize(0, 30))

        self.formLayout.setWidget(1, QFormLayout.FieldRole, self.fileChooser_comment_file)

        self.label_rule_file = QLabel(self.layoutWidget)
        self.label_rule_file.setObjectName(u"label_rule_file")

        self.formLayout.setWidget(2, QFormLayout.LabelRole, self.label_rule_file)

        self.fileChooser_rule_file = IodeFileChooser(self.layoutWidget)
        self.fileChooser_rule_file.setObjectName(u"fileChooser_rule_file")
        self.fileChooser_rule_file.setMinimumSize(QSize(0, 30))

        self.formLayout.setWidget(2, QFormLayout.FieldRole, self.fileChooser_rule_file)

        self.frame_bottom = QFrame(MenuFileExport)
        self.frame_bottom.setObjectName(u"frame_bottom")
        self.frame_bottom.setGeometry(QRect(10, 130, 521, 171))
        self.frame_bottom.setFrameShape(QFrame.Box)
        self.frame_bottom.setFrameShadow(QFrame.Raised)
        self.layoutWidget1 = QWidget(self.frame_bottom)
        self.layoutWidget1.setObjectName(u"layoutWidget1")
        self.layoutWidget1.setGeometry(QRect(10, 10, 501, 151))
        self.gridLayout = QGridLayout(self.layoutWidget1)
        self.gridLayout.setObjectName(u"gridLayout")
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.label_sample = QLabel(self.layoutWidget1)
        self.label_sample.setObjectName(u"label_sample")

        self.gridLayout.addWidget(self.label_sample, 0, 0, 1, 1)

        self.label_sample_from = QLabel(self.layoutWidget1)
        self.label_sample_from.setObjectName(u"label_sample_from")

        self.gridLayout.addWidget(self.label_sample_from, 0, 1, 1, 2)

        self.sampleEdit_sample_from = IodeSampleEdit(self.layoutWidget1)
        self.sampleEdit_sample_from.setObjectName(u"sampleEdit_sample_from")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.sampleEdit_sample_from.sizePolicy().hasHeightForWidth())
        self.sampleEdit_sample_from.setSizePolicy(sizePolicy)
        self.sampleEdit_sample_from.setMinimumSize(QSize(80, 0))
        self.sampleEdit_sample_from.setMaximumSize(QSize(80, 16777215))

        self.gridLayout.addWidget(self.sampleEdit_sample_from, 0, 3, 1, 2)

        self.label_sample_to = QLabel(self.layoutWidget1)
        self.label_sample_to.setObjectName(u"label_sample_to")

        self.gridLayout.addWidget(self.label_sample_to, 0, 5, 1, 1)

        self.sampleEdit_sample_to = IodeSampleEdit(self.layoutWidget1)
        self.sampleEdit_sample_to.setObjectName(u"sampleEdit_sample_to")
        sizePolicy.setHeightForWidth(self.sampleEdit_sample_to.sizePolicy().hasHeightForWidth())
        self.sampleEdit_sample_to.setSizePolicy(sizePolicy)
        self.sampleEdit_sample_to.setMinimumSize(QSize(80, 0))
        self.sampleEdit_sample_to.setMaximumSize(QSize(80, 16777215))

        self.gridLayout.addWidget(self.sampleEdit_sample_to, 0, 6, 1, 1)

        self.label_format = QLabel(self.layoutWidget1)
        self.label_format.setObjectName(u"label_format")

        self.gridLayout.addWidget(self.label_format, 0, 7, 1, 2)

        self.comboBox_format = QComboBox(self.layoutWidget1)
        self.comboBox_format.setObjectName(u"comboBox_format")
        sizePolicy.setHeightForWidth(self.comboBox_format.sizePolicy().hasHeightForWidth())
        self.comboBox_format.setSizePolicy(sizePolicy)
        self.comboBox_format.setMinimumSize(QSize(140, 0))
        self.comboBox_format.setMaximumSize(QSize(140, 16777215))

        self.gridLayout.addWidget(self.comboBox_format, 0, 9, 1, 1)

        self.label_save_file = QLabel(self.layoutWidget1)
        self.label_save_file.setObjectName(u"label_save_file")

        self.gridLayout.addWidget(self.label_save_file, 1, 0, 1, 2)

        self.fileChooser_save_file = IodeFileChooser(self.layoutWidget1)
        self.fileChooser_save_file.setObjectName(u"fileChooser_save_file")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.fileChooser_save_file.sizePolicy().hasHeightForWidth())
        self.fileChooser_save_file.setSizePolicy(sizePolicy1)
        self.fileChooser_save_file.setMinimumSize(QSize(0, 30))

        self.gridLayout.addWidget(self.fileChooser_save_file, 1, 2, 1, 8)

        self.label_csv_only = QLabel(self.layoutWidget1)
        self.label_csv_only.setObjectName(u"label_csv_only")

        self.gridLayout.addWidget(self.label_csv_only, 2, 0, 1, 2)

        self.label_nan = QLabel(self.layoutWidget1)
        self.label_nan.setObjectName(u"label_nan")

        self.gridLayout.addWidget(self.label_nan, 2, 3, 1, 1)

        self.lineEdit_nan = QLineEdit(self.layoutWidget1)
        self.lineEdit_nan.setObjectName(u"lineEdit_nan")

        self.gridLayout.addWidget(self.lineEdit_nan, 2, 4, 1, 1)

        self.label_separator = QLabel(self.layoutWidget1)
        self.label_separator.setObjectName(u"label_separator")

        self.gridLayout.addWidget(self.label_separator, 2, 6, 1, 1)

        self.lineEdit_separator = QLineEdit(self.layoutWidget1)
        self.lineEdit_separator.setObjectName(u"lineEdit_separator")
        sizePolicy2 = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Preferred)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.lineEdit_separator.sizePolicy().hasHeightForWidth())
        self.lineEdit_separator.setSizePolicy(sizePolicy2)
        self.lineEdit_separator.setMinimumSize(QSize(40, 0))
        self.lineEdit_separator.setMaximumSize(QSize(40, 16777215))

        self.gridLayout.addWidget(self.lineEdit_separator, 2, 7, 1, 1)

        self.horizontalSpacer = QSpacerItem(158, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 2, 8, 1, 2)

        self.label_debug = QLabel(self.layoutWidget1)
        self.label_debug.setObjectName(u"label_debug")

        self.gridLayout.addWidget(self.label_debug, 3, 0, 1, 2)

        self.fileChooser_debug_file = IodeFileChooser(self.layoutWidget1)
        self.fileChooser_debug_file.setObjectName(u"fileChooser_debug_file")
        sizePolicy1.setHeightForWidth(self.fileChooser_debug_file.sizePolicy().hasHeightForWidth())
        self.fileChooser_debug_file.setSizePolicy(sizePolicy1)
        self.fileChooser_debug_file.setMinimumSize(QSize(0, 30))

        self.gridLayout.addWidget(self.fileChooser_debug_file, 3, 2, 1, 8)

        self.layoutWidget_2 = QWidget(MenuFileExport)
        self.layoutWidget_2.setObjectName(u"layoutWidget_2")
        self.layoutWidget_2.setGeometry(QRect(550, 10, 77, 291))
        self.verticalLayout = QVBoxLayout(self.layoutWidget_2)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.saveButton = QPushButton(self.layoutWidget_2)
        self.saveButton.setObjectName(u"saveButton")
        sizePolicy.setHeightForWidth(self.saveButton.sizePolicy().hasHeightForWidth())
        self.saveButton.setSizePolicy(sizePolicy)

        self.verticalLayout.addWidget(self.saveButton)

        self.cancelButton = QPushButton(self.layoutWidget_2)
        self.cancelButton.setObjectName(u"cancelButton")
        sizePolicy.setHeightForWidth(self.cancelButton.sizePolicy().hasHeightForWidth())
        self.cancelButton.setSizePolicy(sizePolicy)

        self.verticalLayout.addWidget(self.cancelButton)

        self.helpButton = QPushButton(self.layoutWidget_2)
        self.helpButton.setObjectName(u"helpButton")
        sizePolicy.setHeightForWidth(self.helpButton.sizePolicy().hasHeightForWidth())
        self.helpButton.setSizePolicy(sizePolicy)

        self.verticalLayout.addWidget(self.helpButton)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer)


        self.retranslateUi(MenuFileExport)
        self.saveButton.clicked.connect(MenuFileExport.save)
        self.cancelButton.clicked.connect(MenuFileExport.reject)
        self.helpButton.clicked.connect(MenuFileExport.help)

        QMetaObject.connectSlotsByName(MenuFileExport)
    # setupUi

    def retranslateUi(self, MenuFileExport):
        MenuFileExport.setWindowTitle(QCoreApplication.translate("MenuFileExport", u"EXPORT  FILE  BY  RULE", None))
        self.label_variable_file.setText(QCoreApplication.translate("MenuFileExport", u"Variable file", None))
        self.label_comment_file.setText(QCoreApplication.translate("MenuFileExport", u"Comment file", None))
        self.label_rule_file.setText(QCoreApplication.translate("MenuFileExport", u"Rule file", None))
        self.label_sample.setText(QCoreApplication.translate("MenuFileExport", u"Sample", None))
        self.label_sample_from.setText(QCoreApplication.translate("MenuFileExport", u"From", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setToolTip(QCoreApplication.translate("MenuFileExport", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setInputMask("")
        self.sampleEdit_sample_from.setPlaceholderText("")
        self.label_sample_to.setText(QCoreApplication.translate("MenuFileExport", u"To", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setToolTip(QCoreApplication.translate("MenuFileExport", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setInputMask("")
        self.sampleEdit_sample_to.setPlaceholderText("")
        self.label_format.setText(QCoreApplication.translate("MenuFileExport", u"Format", None))
        self.label_save_file.setText(QCoreApplication.translate("MenuFileExport", u"File", None))
        self.label_csv_only.setText(QCoreApplication.translate("MenuFileExport", u"CSV only :", None))
        self.label_nan.setText(QCoreApplication.translate("MenuFileExport", u"NaN = ", None))
        self.lineEdit_nan.setText(QCoreApplication.translate("MenuFileExport", u"#N/A", None))
        self.label_separator.setText(QCoreApplication.translate("MenuFileExport", u"Separator =", None))
        self.lineEdit_separator.setText(QCoreApplication.translate("MenuFileExport", u";", None))
        self.label_debug.setText(QCoreApplication.translate("MenuFileExport", u"Debug file", None))
        self.saveButton.setText(QCoreApplication.translate("MenuFileExport", u"Save", None))
#if QT_CONFIG(shortcut)
        self.saveButton.setShortcut(QCoreApplication.translate("MenuFileExport", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.cancelButton.setText(QCoreApplication.translate("MenuFileExport", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.cancelButton.setShortcut(QCoreApplication.translate("MenuFileExport", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.helpButton.setText(QCoreApplication.translate("MenuFileExport", u"Help", None))
#if QT_CONFIG(shortcut)
        self.helpButton.setShortcut(QCoreApplication.translate("MenuFileExport", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

