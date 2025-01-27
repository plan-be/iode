# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'graph_variables.ui'
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
    QGridLayout, QLabel, QLineEdit, QPushButton,
    QSizePolicy, QSpacerItem, QSplitter, QWidget)

from text_edit.complete_text_edit import IodeAutoCompleteTextEdit
from util.widgets.sample_edit import IodeSampleEdit

class Ui_MenuGraphVariables(object):
    def setupUi(self, MenuGraphVariables):
        if not MenuGraphVariables.objectName():
            MenuGraphVariables.setObjectName(u"MenuGraphVariables")
        MenuGraphVariables.setWindowModality(Qt.ApplicationModal)
        MenuGraphVariables.resize(600, 280)
        MenuGraphVariables.setMinimumSize(QSize(120, 280))
        MenuGraphVariables.setMaximumSize(QSize(600, 280))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuGraphVariables.setWindowIcon(icon)
        self.gridLayout = QGridLayout(MenuGraphVariables)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_variables = QLabel(MenuGraphVariables)
        self.label_variables.setObjectName(u"label_variables")
        self.label_variables.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_variables, 0, 0, 1, 1)

        self.textEdit_variables = IodeAutoCompleteTextEdit(MenuGraphVariables)
        self.textEdit_variables.setObjectName(u"textEdit_variables")

        self.gridLayout.addWidget(self.textEdit_variables, 0, 1, 1, 6)

        self.splitter = QSplitter(MenuGraphVariables)
        self.splitter.setObjectName(u"splitter")
        self.splitter.setOrientation(Qt.Vertical)
        self.pushButton_display = QPushButton(self.splitter)
        self.pushButton_display.setObjectName(u"pushButton_display")
        self.splitter.addWidget(self.pushButton_display)
        self.pushButton_apply = QPushButton(self.splitter)
        self.pushButton_apply.setObjectName(u"pushButton_apply")
        self.splitter.addWidget(self.pushButton_apply)
        self.pushButton_setup = QPushButton(self.splitter)
        self.pushButton_setup.setObjectName(u"pushButton_setup")
        self.splitter.addWidget(self.pushButton_setup)
        self.pushButton_cancel = QPushButton(self.splitter)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")
        self.splitter.addWidget(self.pushButton_cancel)
        self.pushButton_help = QPushButton(self.splitter)
        self.pushButton_help.setObjectName(u"pushButton_help")
        self.splitter.addWidget(self.pushButton_help)

        self.gridLayout.addWidget(self.splitter, 0, 7, 3, 1)

        self.label_x_axis_type = QLabel(MenuGraphVariables)
        self.label_x_axis_type.setObjectName(u"label_x_axis_type")
        self.label_x_axis_type.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_x_axis_type, 1, 0, 1, 1)

        self.comboBox_x_axis_type = QComboBox(MenuGraphVariables)
        self.comboBox_x_axis_type.setObjectName(u"comboBox_x_axis_type")
        self.comboBox_x_axis_type.setMinimumSize(QSize(120, 0))

        self.gridLayout.addWidget(self.comboBox_x_axis_type, 1, 1, 1, 1)

        self.label_sample_from = QLabel(MenuGraphVariables)
        self.label_sample_from.setObjectName(u"label_sample_from")

        self.gridLayout.addWidget(self.label_sample_from, 1, 2, 1, 1)

        self.sampleEdit_sample_from = IodeSampleEdit(MenuGraphVariables)
        self.sampleEdit_sample_from.setObjectName(u"sampleEdit_sample_from")

        self.gridLayout.addWidget(self.sampleEdit_sample_from, 1, 3, 1, 2)

        self.label_sample_to = QLabel(MenuGraphVariables)
        self.label_sample_to.setObjectName(u"label_sample_to")

        self.gridLayout.addWidget(self.label_sample_to, 1, 5, 1, 1)

        self.sampleEdit_sample_to = IodeSampleEdit(MenuGraphVariables)
        self.sampleEdit_sample_to.setObjectName(u"sampleEdit_sample_to")

        self.gridLayout.addWidget(self.sampleEdit_sample_to, 1, 6, 1, 1)

        self.label_chart_type = QLabel(MenuGraphVariables)
        self.label_chart_type.setObjectName(u"label_chart_type")
        self.label_chart_type.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_chart_type, 2, 0, 1, 1)

        self.comboBox_chart_type = QComboBox(MenuGraphVariables)
        self.comboBox_chart_type.setObjectName(u"comboBox_chart_type")
        self.comboBox_chart_type.setMinimumSize(QSize(120, 0))

        self.gridLayout.addWidget(self.comboBox_chart_type, 2, 1, 2, 1)

        self.checkBox_log_scale = QCheckBox(MenuGraphVariables)
        self.checkBox_log_scale.setObjectName(u"checkBox_log_scale")

        self.gridLayout.addWidget(self.checkBox_log_scale, 2, 2, 2, 2)

        self.horizontalSpacer = QSpacerItem(218, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 2, 4, 2, 3)

        self.verticalSpacer = QSpacerItem(20, 84, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 3, 7, 4, 1)

        self.label_X_ticks = QLabel(MenuGraphVariables)
        self.label_X_ticks.setObjectName(u"label_X_ticks")
        self.label_X_ticks.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_X_ticks, 4, 0, 1, 1)

        self.comboBox_X_ticks = QComboBox(MenuGraphVariables)
        self.comboBox_X_ticks.setObjectName(u"comboBox_X_ticks")
        self.comboBox_X_ticks.setMinimumSize(QSize(120, 0))

        self.gridLayout.addWidget(self.comboBox_X_ticks, 4, 1, 1, 1)

        self.horizontalSpacer_2 = QSpacerItem(314, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_2, 4, 2, 1, 5)

        self.label_Y_ticks = QLabel(MenuGraphVariables)
        self.label_Y_ticks.setObjectName(u"label_Y_ticks")
        self.label_Y_ticks.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_Y_ticks, 5, 0, 1, 1)

        self.comboBox_Y_ticks = QComboBox(MenuGraphVariables)
        self.comboBox_Y_ticks.setObjectName(u"comboBox_Y_ticks")
        self.comboBox_Y_ticks.setMinimumSize(QSize(120, 0))

        self.gridLayout.addWidget(self.comboBox_Y_ticks, 5, 1, 1, 1)

        self.label_min_Y = QLabel(MenuGraphVariables)
        self.label_min_Y.setObjectName(u"label_min_Y")
        self.label_min_Y.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_min_Y, 5, 2, 1, 1)

        self.lineEdit_min_Y = QLineEdit(MenuGraphVariables)
        self.lineEdit_min_Y.setObjectName(u"lineEdit_min_Y")

        self.gridLayout.addWidget(self.lineEdit_min_Y, 5, 3, 1, 2)

        self.label_max_Y = QLabel(MenuGraphVariables)
        self.label_max_Y.setObjectName(u"label_max_Y")
        self.label_max_Y.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_max_Y, 5, 5, 1, 1)

        self.lineEdit_max_Y = QLineEdit(MenuGraphVariables)
        self.lineEdit_max_Y.setObjectName(u"lineEdit_max_Y")

        self.gridLayout.addWidget(self.lineEdit_max_Y, 5, 6, 1, 1)

        self.label_language = QLabel(MenuGraphVariables)
        self.label_language.setObjectName(u"label_language")
        self.label_language.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_language, 6, 0, 1, 1)

        self.comboBox_language = QComboBox(MenuGraphVariables)
        self.comboBox_language.setObjectName(u"comboBox_language")
        self.comboBox_language.setMinimumSize(QSize(120, 0))

        self.gridLayout.addWidget(self.comboBox_language, 6, 1, 1, 1)

        self.horizontalSpacer_3 = QSpacerItem(314, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_3, 6, 2, 1, 5)


        self.retranslateUi(MenuGraphVariables)
        self.pushButton_display.clicked.connect(MenuGraphVariables.display)
        self.pushButton_apply.clicked.connect(MenuGraphVariables.apply)
        self.pushButton_setup.clicked.connect(MenuGraphVariables.setup)
        self.pushButton_cancel.clicked.connect(MenuGraphVariables.reject)
        self.pushButton_help.clicked.connect(MenuGraphVariables.help)

        QMetaObject.connectSlotsByName(MenuGraphVariables)
    # setupUi

    def retranslateUi(self, MenuGraphVariables):
        MenuGraphVariables.setWindowTitle(QCoreApplication.translate("MenuGraphVariables", u"GRAPH VARIABLES", None))
        self.label_variables.setText(QCoreApplication.translate("MenuGraphVariables", u"Variables", None))
        self.pushButton_display.setText(QCoreApplication.translate("MenuGraphVariables", u"Display", None))
#if QT_CONFIG(shortcut)
        self.pushButton_display.setShortcut(QCoreApplication.translate("MenuGraphVariables", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_apply.setText(QCoreApplication.translate("MenuGraphVariables", u"Apply", None))
#if QT_CONFIG(shortcut)
        self.pushButton_apply.setShortcut(QCoreApplication.translate("MenuGraphVariables", u"F7", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_setup.setText(QCoreApplication.translate("MenuGraphVariables", u"Setup", None))
#if QT_CONFIG(shortcut)
        self.pushButton_setup.setShortcut(QCoreApplication.translate("MenuGraphVariables", u"F6", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuGraphVariables", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuGraphVariables", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuGraphVariables", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuGraphVariables", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label_x_axis_type.setText(QCoreApplication.translate("MenuGraphVariables", u"Sample", None))
        self.label_sample_from.setText(QCoreApplication.translate("MenuGraphVariables", u"From", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setToolTip(QCoreApplication.translate("MenuGraphVariables", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setInputMask("")
        self.sampleEdit_sample_from.setPlaceholderText("")
        self.label_sample_to.setText(QCoreApplication.translate("MenuGraphVariables", u"To", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setToolTip(QCoreApplication.translate("MenuGraphVariables", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setInputMask("")
        self.sampleEdit_sample_to.setPlaceholderText("")
        self.label_chart_type.setText(QCoreApplication.translate("MenuGraphVariables", u"Layout", None))
        self.checkBox_log_scale.setText(QCoreApplication.translate("MenuGraphVariables", u"Log Scale", None))
        self.label_X_ticks.setText(QCoreApplication.translate("MenuGraphVariables", u"X grid", None))
        self.label_Y_ticks.setText(QCoreApplication.translate("MenuGraphVariables", u"Y grid", None))
        self.label_min_Y.setText(QCoreApplication.translate("MenuGraphVariables", u"Min Y", None))
        self.label_max_Y.setText(QCoreApplication.translate("MenuGraphVariables", u"Max Y", None))
        self.label_language.setText(QCoreApplication.translate("MenuGraphVariables", u"Language", None))
    # retranslateUi

