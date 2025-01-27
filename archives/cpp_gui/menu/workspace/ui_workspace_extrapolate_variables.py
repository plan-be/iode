# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'workspace_extrapolate_variables.ui'
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
    QLabel, QPushButton, QSizePolicy, QSpacerItem,
    QVBoxLayout, QWidget)

from text_edit.complete_text_edit import IodeAutoCompleteTextEdit
from util.widgets.sample_edit import IodeSampleEdit

class Ui_MenuWorkspaceExtrapolateVariables(object):
    def setupUi(self, MenuWorkspaceExtrapolateVariables):
        if not MenuWorkspaceExtrapolateVariables.objectName():
            MenuWorkspaceExtrapolateVariables.setObjectName(u"MenuWorkspaceExtrapolateVariables")
        MenuWorkspaceExtrapolateVariables.resize(500, 250)
        MenuWorkspaceExtrapolateVariables.setMinimumSize(QSize(170, 250))
        MenuWorkspaceExtrapolateVariables.setMaximumSize(QSize(500, 250))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuWorkspaceExtrapolateVariables.setWindowIcon(icon)
        self.gridLayout = QGridLayout(MenuWorkspaceExtrapolateVariables)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_extrapolate = QLabel(MenuWorkspaceExtrapolateVariables)
        self.label_extrapolate.setObjectName(u"label_extrapolate")

        self.gridLayout.addWidget(self.label_extrapolate, 0, 0, 1, 1)

        self.label_sample_from = QLabel(MenuWorkspaceExtrapolateVariables)
        self.label_sample_from.setObjectName(u"label_sample_from")

        self.gridLayout.addWidget(self.label_sample_from, 0, 1, 1, 1)

        self.sampleEdit_sample_from = IodeSampleEdit(MenuWorkspaceExtrapolateVariables)
        self.sampleEdit_sample_from.setObjectName(u"sampleEdit_sample_from")

        self.gridLayout.addWidget(self.sampleEdit_sample_from, 0, 2, 1, 1)

        self.label_sample_to = QLabel(MenuWorkspaceExtrapolateVariables)
        self.label_sample_to.setObjectName(u"label_sample_to")

        self.gridLayout.addWidget(self.label_sample_to, 0, 3, 1, 1)

        self.sampleEdit_sample_to = IodeSampleEdit(MenuWorkspaceExtrapolateVariables)
        self.sampleEdit_sample_to.setObjectName(u"sampleEdit_sample_to")

        self.gridLayout.addWidget(self.sampleEdit_sample_to, 0, 4, 1, 1)

        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.pushButton_extrapolate = QPushButton(MenuWorkspaceExtrapolateVariables)
        self.pushButton_extrapolate.setObjectName(u"pushButton_extrapolate")

        self.verticalLayout.addWidget(self.pushButton_extrapolate)

        self.pushButton_cancel = QPushButton(MenuWorkspaceExtrapolateVariables)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.verticalLayout.addWidget(self.pushButton_cancel)

        self.pushButton_help = QPushButton(MenuWorkspaceExtrapolateVariables)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.gridLayout.addLayout(self.verticalLayout, 0, 5, 3, 1)

        self.label_method = QLabel(MenuWorkspaceExtrapolateVariables)
        self.label_method.setObjectName(u"label_method")

        self.gridLayout.addWidget(self.label_method, 1, 0, 1, 1)

        self.comboBox_method = QComboBox(MenuWorkspaceExtrapolateVariables)
        self.comboBox_method.setObjectName(u"comboBox_method")
        self.comboBox_method.setMinimumSize(QSize(170, 0))

        self.gridLayout.addWidget(self.comboBox_method, 1, 1, 1, 3)

        self.horizontalSpacer = QSpacerItem(131, 18, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 1, 4, 1, 1)

        self.label_variables_list = QLabel(MenuWorkspaceExtrapolateVariables)
        self.label_variables_list.setObjectName(u"label_variables_list")

        self.gridLayout.addWidget(self.label_variables_list, 2, 0, 2, 1)

        self.textEdit_variables_list = IodeAutoCompleteTextEdit(MenuWorkspaceExtrapolateVariables)
        self.textEdit_variables_list.setObjectName(u"textEdit_variables_list")

        self.gridLayout.addWidget(self.textEdit_variables_list, 2, 1, 2, 4)

        self.verticalSpacer = QSpacerItem(20, 137, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 3, 5, 1, 1)


        self.retranslateUi(MenuWorkspaceExtrapolateVariables)
        self.pushButton_extrapolate.clicked.connect(MenuWorkspaceExtrapolateVariables.extrapolate_variables)
        self.pushButton_cancel.clicked.connect(MenuWorkspaceExtrapolateVariables.reject)
        self.pushButton_help.clicked.connect(MenuWorkspaceExtrapolateVariables.help)

        QMetaObject.connectSlotsByName(MenuWorkspaceExtrapolateVariables)
    # setupUi

    def retranslateUi(self, MenuWorkspaceExtrapolateVariables):
        MenuWorkspaceExtrapolateVariables.setWindowTitle(QCoreApplication.translate("MenuWorkspaceExtrapolateVariables", u"EXTRAPOLATE VARIABLES", None))
        self.label_extrapolate.setText(QCoreApplication.translate("MenuWorkspaceExtrapolateVariables", u"Extrapolate", None))
        self.label_sample_from.setText(QCoreApplication.translate("MenuWorkspaceExtrapolateVariables", u"From", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setToolTip(QCoreApplication.translate("MenuWorkspaceExtrapolateVariables", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setInputMask("")
        self.sampleEdit_sample_from.setPlaceholderText("")
        self.label_sample_to.setText(QCoreApplication.translate("MenuWorkspaceExtrapolateVariables", u"To", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setToolTip(QCoreApplication.translate("MenuWorkspaceExtrapolateVariables", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setInputMask("")
        self.sampleEdit_sample_to.setPlaceholderText("")
        self.pushButton_extrapolate.setText(QCoreApplication.translate("MenuWorkspaceExtrapolateVariables", u"Extrapolate", None))
#if QT_CONFIG(shortcut)
        self.pushButton_extrapolate.setShortcut(QCoreApplication.translate("MenuWorkspaceExtrapolateVariables", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuWorkspaceExtrapolateVariables", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuWorkspaceExtrapolateVariables", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuWorkspaceExtrapolateVariables", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuWorkspaceExtrapolateVariables", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label_method.setText(QCoreApplication.translate("MenuWorkspaceExtrapolateVariables", u"Method", None))
        self.label_variables_list.setText(QCoreApplication.translate("MenuWorkspaceExtrapolateVariables", u"Variables list\n"
"\n"
"(empty=all)", None))
    # retranslateUi

