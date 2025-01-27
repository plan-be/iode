# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'compute_identities.ui'
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
from PySide6.QtWidgets import (QApplication, QCheckBox, QComboBox, QDialog,
    QFrame, QGridLayout, QLabel, QPushButton,
    QSizePolicy, QSpacerItem, QWidget)

from text_edit.complete_text_edit import IodeAutoCompleteTextEdit
from util.widgets.file_chooser import IodeFileChooser
from util.widgets.sample_edit import IodeSampleEdit

class Ui_MenuComputeIdentities(object):
    def setupUi(self, MenuComputeIdentities):
        if not MenuComputeIdentities.objectName():
            MenuComputeIdentities.setObjectName(u"MenuComputeIdentities")
        MenuComputeIdentities.setWindowModality(Qt.ApplicationModal)
        MenuComputeIdentities.resize(600, 420)
        MenuComputeIdentities.setMinimumSize(QSize(0, 420))
        MenuComputeIdentities.setMaximumSize(QSize(600, 420))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuComputeIdentities.setWindowIcon(icon)
        self.gridLayout = QGridLayout(MenuComputeIdentities)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_execute = QLabel(MenuComputeIdentities)
        self.label_execute.setObjectName(u"label_execute")

        self.gridLayout.addWidget(self.label_execute, 0, 0, 1, 1)

        self.label_from = QLabel(MenuComputeIdentities)
        self.label_from.setObjectName(u"label_from")

        self.gridLayout.addWidget(self.label_from, 0, 1, 1, 1)

        self.sampleEdit_sample_from = IodeSampleEdit(MenuComputeIdentities)
        self.sampleEdit_sample_from.setObjectName(u"sampleEdit_sample_from")

        self.gridLayout.addWidget(self.sampleEdit_sample_from, 0, 2, 1, 2)

        self.label_to = QLabel(MenuComputeIdentities)
        self.label_to.setObjectName(u"label_to")

        self.gridLayout.addWidget(self.label_to, 0, 4, 1, 1)

        self.sampleEdit_sample_to = IodeSampleEdit(MenuComputeIdentities)
        self.sampleEdit_sample_to.setObjectName(u"sampleEdit_sample_to")

        self.gridLayout.addWidget(self.sampleEdit_sample_to, 0, 5, 1, 1)

        self.horizontalSpacer_2 = QSpacerItem(74, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_2, 0, 6, 1, 1)

        self.OKButton = QPushButton(MenuComputeIdentities)
        self.OKButton.setObjectName(u"OKButton")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.OKButton.sizePolicy().hasHeightForWidth())
        self.OKButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.OKButton, 0, 7, 1, 1)

        self.label_identities_list = QLabel(MenuComputeIdentities)
        self.label_identities_list.setObjectName(u"label_identities_list")
        self.label_identities_list.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_identities_list, 1, 0, 2, 1)

        self.textEdit_identities_list = IodeAutoCompleteTextEdit(MenuComputeIdentities)
        self.textEdit_identities_list.setObjectName(u"textEdit_identities_list")

        self.gridLayout.addWidget(self.textEdit_identities_list, 1, 1, 2, 6)

        self.cancelButton = QPushButton(MenuComputeIdentities)
        self.cancelButton.setObjectName(u"cancelButton")
        sizePolicy.setHeightForWidth(self.cancelButton.sizePolicy().hasHeightForWidth())
        self.cancelButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.cancelButton, 1, 7, 1, 1)

        self.helpButton = QPushButton(MenuComputeIdentities)
        self.helpButton.setObjectName(u"helpButton")
        sizePolicy.setHeightForWidth(self.helpButton.sizePolicy().hasHeightForWidth())
        self.helpButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.helpButton, 2, 7, 1, 1)

        self.line = QFrame(MenuComputeIdentities)
        self.line.setObjectName(u"line")
        self.line.setFrameShape(QFrame.Shape.HLine)
        self.line.setFrameShadow(QFrame.Shadow.Sunken)

        self.gridLayout.addWidget(self.line, 3, 0, 1, 7)

        self.verticalSpacer = QSpacerItem(20, 280, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 3, 7, 10, 1)

        self.label_variables_list = QLabel(MenuComputeIdentities)
        self.label_variables_list.setObjectName(u"label_variables_list")
        self.label_variables_list.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_variables_list, 4, 0, 2, 1)

        self.fileChooser_var_file1 = IodeFileChooser(MenuComputeIdentities)
        self.fileChooser_var_file1.setObjectName(u"fileChooser_var_file1")

        self.gridLayout.addWidget(self.fileChooser_var_file1, 4, 1, 1, 6)

        self.fileChooser_var_file2 = IodeFileChooser(MenuComputeIdentities)
        self.fileChooser_var_file2.setObjectName(u"fileChooser_var_file2")

        self.gridLayout.addWidget(self.fileChooser_var_file2, 5, 1, 1, 6)

        self.fileChooser_var_file3 = IodeFileChooser(MenuComputeIdentities)
        self.fileChooser_var_file3.setObjectName(u"fileChooser_var_file3")

        self.gridLayout.addWidget(self.fileChooser_var_file3, 6, 1, 1, 6)

        self.line_2 = QFrame(MenuComputeIdentities)
        self.line_2.setObjectName(u"line_2")
        self.line_2.setFrameShape(QFrame.Shape.HLine)
        self.line_2.setFrameShadow(QFrame.Shadow.Sunken)

        self.gridLayout.addWidget(self.line_2, 7, 0, 1, 7)

        self.label_scalars_list = QLabel(MenuComputeIdentities)
        self.label_scalars_list.setObjectName(u"label_scalars_list")
        self.label_scalars_list.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_scalars_list, 8, 0, 2, 1)

        self.fileChooser_scl_file1 = IodeFileChooser(MenuComputeIdentities)
        self.fileChooser_scl_file1.setObjectName(u"fileChooser_scl_file1")

        self.gridLayout.addWidget(self.fileChooser_scl_file1, 8, 1, 1, 6)

        self.fileChooser_scl_file2 = IodeFileChooser(MenuComputeIdentities)
        self.fileChooser_scl_file2.setObjectName(u"fileChooser_scl_file2")

        self.gridLayout.addWidget(self.fileChooser_scl_file2, 9, 1, 1, 6)

        self.fileChooser_scl_file3 = IodeFileChooser(MenuComputeIdentities)
        self.fileChooser_scl_file3.setObjectName(u"fileChooser_scl_file3")

        self.gridLayout.addWidget(self.fileChooser_scl_file3, 10, 1, 1, 6)

        self.line_3 = QFrame(MenuComputeIdentities)
        self.line_3.setObjectName(u"line_3")
        self.line_3.setFrameShape(QFrame.Shape.HLine)
        self.line_3.setFrameShadow(QFrame.Shadow.Sunken)

        self.gridLayout.addWidget(self.line_3, 11, 0, 1, 7)

        self.label_trace = QLabel(MenuComputeIdentities)
        self.label_trace.setObjectName(u"label_trace")
        self.label_trace.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_trace, 12, 0, 1, 1)

        self.checkBox_trace = QCheckBox(MenuComputeIdentities)
        self.checkBox_trace.setObjectName(u"checkBox_trace")

        self.gridLayout.addWidget(self.checkBox_trace, 12, 1, 1, 1)

        self.label_language = QLabel(MenuComputeIdentities)
        self.label_language.setObjectName(u"label_language")

        self.gridLayout.addWidget(self.label_language, 12, 2, 1, 1)

        self.comboBox_language = QComboBox(MenuComputeIdentities)
        self.comboBox_language.addItem("")
        self.comboBox_language.addItem("")
        self.comboBox_language.addItem("")
        self.comboBox_language.setObjectName(u"comboBox_language")

        self.gridLayout.addWidget(self.comboBox_language, 12, 3, 1, 1)

        self.horizontalSpacer = QSpacerItem(236, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 12, 4, 1, 3)


        self.retranslateUi(MenuComputeIdentities)
        self.OKButton.clicked.connect(MenuComputeIdentities.compute)
        self.cancelButton.clicked.connect(MenuComputeIdentities.reject)
        self.helpButton.clicked.connect(MenuComputeIdentities.help)

        QMetaObject.connectSlotsByName(MenuComputeIdentities)
    # setupUi

    def retranslateUi(self, MenuComputeIdentities):
        MenuComputeIdentities.setWindowTitle(QCoreApplication.translate("MenuComputeIdentities", u"EXECUTE IDENTITIES", None))
        self.label_execute.setText(QCoreApplication.translate("MenuComputeIdentities", u"Execute", None))
        self.label_from.setText(QCoreApplication.translate("MenuComputeIdentities", u"From", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setToolTip(QCoreApplication.translate("MenuComputeIdentities", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setInputMask("")
        self.sampleEdit_sample_from.setPlaceholderText("")
        self.label_to.setText(QCoreApplication.translate("MenuComputeIdentities", u"To", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setToolTip(QCoreApplication.translate("MenuComputeIdentities", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setInputMask("")
        self.sampleEdit_sample_to.setPlaceholderText("")
        self.OKButton.setText(QCoreApplication.translate("MenuComputeIdentities", u"OK", None))
#if QT_CONFIG(shortcut)
        self.OKButton.setShortcut(QCoreApplication.translate("MenuComputeIdentities", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.label_identities_list.setText(QCoreApplication.translate("MenuComputeIdentities", u"Identities List\n"
"\n"
"(empty for\n"
"all identities)", None))
        self.cancelButton.setText(QCoreApplication.translate("MenuComputeIdentities", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.cancelButton.setShortcut(QCoreApplication.translate("MenuComputeIdentities", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.helpButton.setText(QCoreApplication.translate("MenuComputeIdentities", u"Help", None))
#if QT_CONFIG(shortcut)
        self.helpButton.setShortcut(QCoreApplication.translate("MenuComputeIdentities", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label_variables_list.setText(QCoreApplication.translate("MenuComputeIdentities", u"Input Variables\n"
"Files", None))
        self.label_scalars_list.setText(QCoreApplication.translate("MenuComputeIdentities", u"Input Scalars\n"
"Files", None))
        self.label_trace.setText(QCoreApplication.translate("MenuComputeIdentities", u"Trace ?", None))
        self.checkBox_trace.setText("")
        self.label_language.setText(QCoreApplication.translate("MenuComputeIdentities", u"Language", None))
        self.comboBox_language.setItemText(0, QCoreApplication.translate("MenuComputeIdentities", u"English", None))
        self.comboBox_language.setItemText(1, QCoreApplication.translate("MenuComputeIdentities", u"French", None))
        self.comboBox_language.setItemText(2, QCoreApplication.translate("MenuComputeIdentities", u"Dutch", None))

    # retranslateUi

