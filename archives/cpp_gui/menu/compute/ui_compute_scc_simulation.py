# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'compute_scc_simulation.ui'
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
    QFormLayout, QGridLayout, QLabel, QLineEdit,
    QPushButton, QSizePolicy, QSpacerItem, QSpinBox,
    QVBoxLayout, QWidget)

from util.widgets.sample_edit import IodeSampleEdit

class Ui_MenuComputeSCCSimulation(object):
    def setupUi(self, MenuComputeSCCSimulation):
        if not MenuComputeSCCSimulation.objectName():
            MenuComputeSCCSimulation.setObjectName(u"MenuComputeSCCSimulation")
        MenuComputeSCCSimulation.resize(620, 270)
        MenuComputeSCCSimulation.setMinimumSize(QSize(620, 270))
        MenuComputeSCCSimulation.setMaximumSize(QSize(620, 270))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuComputeSCCSimulation.setWindowIcon(icon)
        self.label_save_title = QLabel(MenuComputeSCCSimulation)
        self.label_save_title.setObjectName(u"label_save_title")
        self.label_save_title.setGeometry(QRect(20, 150, 491, 19))
        self.label_save_title.setTextFormat(Qt.MarkdownText)
        self.layoutWidget = QWidget(MenuComputeSCCSimulation)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(20, 170, 491, 80))
        self.formLayout = QFormLayout(self.layoutWidget)
        self.formLayout.setObjectName(u"formLayout")
        self.formLayout.setContentsMargins(0, 0, 0, 0)
        self.label_pre_recursive = QLabel(self.layoutWidget)
        self.label_pre_recursive.setObjectName(u"label_pre_recursive")

        self.formLayout.setWidget(0, QFormLayout.LabelRole, self.label_pre_recursive)

        self.lineEdit_pre_recursive_list_name = QLineEdit(self.layoutWidget)
        self.lineEdit_pre_recursive_list_name.setObjectName(u"lineEdit_pre_recursive_list_name")

        self.formLayout.setWidget(0, QFormLayout.FieldRole, self.lineEdit_pre_recursive_list_name)

        self.label_inter_recursive = QLabel(self.layoutWidget)
        self.label_inter_recursive.setObjectName(u"label_inter_recursive")

        self.formLayout.setWidget(1, QFormLayout.LabelRole, self.label_inter_recursive)

        self.lineEdit_inter_recursive_list_name = QLineEdit(self.layoutWidget)
        self.lineEdit_inter_recursive_list_name.setObjectName(u"lineEdit_inter_recursive_list_name")

        self.formLayout.setWidget(1, QFormLayout.FieldRole, self.lineEdit_inter_recursive_list_name)

        self.label_post_recursive = QLabel(self.layoutWidget)
        self.label_post_recursive.setObjectName(u"label_post_recursive")

        self.formLayout.setWidget(2, QFormLayout.LabelRole, self.label_post_recursive)

        self.lineEdit_post_recursive_list_name = QLineEdit(self.layoutWidget)
        self.lineEdit_post_recursive_list_name.setObjectName(u"lineEdit_post_recursive_list_name")

        self.formLayout.setWidget(2, QFormLayout.FieldRole, self.lineEdit_post_recursive_list_name)

        self.layoutWidget1 = QWidget(MenuComputeSCCSimulation)
        self.layoutWidget1.setObjectName(u"layoutWidget1")
        self.layoutWidget1.setGeometry(QRect(20, 20, 491, 105))
        self.gridLayout = QGridLayout(self.layoutWidget1)
        self.gridLayout.setObjectName(u"gridLayout")
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.label_max_iterations = QLabel(self.layoutWidget1)
        self.label_max_iterations.setObjectName(u"label_max_iterations")

        self.gridLayout.addWidget(self.label_max_iterations, 1, 0, 1, 1)

        self.label_simulation_to = QLabel(self.layoutWidget1)
        self.label_simulation_to.setObjectName(u"label_simulation_to")

        self.gridLayout.addWidget(self.label_simulation_to, 0, 3, 1, 1)

        self.checkBox_debug = QCheckBox(self.layoutWidget1)
        self.checkBox_debug.setObjectName(u"checkBox_debug")

        self.gridLayout.addWidget(self.checkBox_debug, 1, 4, 1, 1)

        self.label_debug = QLabel(self.layoutWidget1)
        self.label_debug.setObjectName(u"label_debug")

        self.gridLayout.addWidget(self.label_debug, 1, 3, 1, 1)

        self.sampleEdit_sample_from = IodeSampleEdit(self.layoutWidget1)
        self.sampleEdit_sample_from.setObjectName(u"sampleEdit_sample_from")

        self.gridLayout.addWidget(self.sampleEdit_sample_from, 0, 2, 1, 1)

        self.lineEdit_relaxation = QLineEdit(self.layoutWidget1)
        self.lineEdit_relaxation.setObjectName(u"lineEdit_relaxation")

        self.gridLayout.addWidget(self.lineEdit_relaxation, 1, 6, 1, 1)

        self.label_initialisation = QLabel(self.layoutWidget1)
        self.label_initialisation.setObjectName(u"label_initialisation")

        self.gridLayout.addWidget(self.label_initialisation, 2, 0, 1, 1)

        self.label_convergence = QLabel(self.layoutWidget1)
        self.label_convergence.setObjectName(u"label_convergence")

        self.gridLayout.addWidget(self.label_convergence, 0, 5, 1, 1)

        self.label_relaxation = QLabel(self.layoutWidget1)
        self.label_relaxation.setObjectName(u"label_relaxation")

        self.gridLayout.addWidget(self.label_relaxation, 1, 5, 1, 1)

        self.lineEdit_convergence = QLineEdit(self.layoutWidget1)
        self.lineEdit_convergence.setObjectName(u"lineEdit_convergence")

        self.gridLayout.addWidget(self.lineEdit_convergence, 0, 6, 1, 1)

        self.spinBox_max_iterations = QSpinBox(self.layoutWidget1)
        self.spinBox_max_iterations.setObjectName(u"spinBox_max_iterations")
        self.spinBox_max_iterations.setMaximum(1000000)
        self.spinBox_max_iterations.setValue(100)
        self.spinBox_max_iterations.setDisplayIntegerBase(10)

        self.gridLayout.addWidget(self.spinBox_max_iterations, 1, 1, 1, 2)

        self.label_simulation_from = QLabel(self.layoutWidget1)
        self.label_simulation_from.setObjectName(u"label_simulation_from")

        self.gridLayout.addWidget(self.label_simulation_from, 0, 1, 1, 1)

        self.sampleEdit_sample_to = IodeSampleEdit(self.layoutWidget1)
        self.sampleEdit_sample_to.setObjectName(u"sampleEdit_sample_to")

        self.gridLayout.addWidget(self.sampleEdit_sample_to, 0, 4, 1, 1)

        self.label_simulation = QLabel(self.layoutWidget1)
        self.label_simulation.setObjectName(u"label_simulation")

        self.gridLayout.addWidget(self.label_simulation, 0, 0, 1, 1)

        self.comboBox_initialization = QComboBox(self.layoutWidget1)
        self.comboBox_initialization.setObjectName(u"comboBox_initialization")

        self.gridLayout.addWidget(self.comboBox_initialization, 2, 1, 1, 6)

        self.layoutWidget_2 = QWidget(MenuComputeSCCSimulation)
        self.layoutWidget_2.setObjectName(u"layoutWidget_2")
        self.layoutWidget_2.setGeometry(QRect(530, 20, 77, 231))
        self.verticalLayout = QVBoxLayout(self.layoutWidget_2)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.OKButton = QPushButton(self.layoutWidget_2)
        self.OKButton.setObjectName(u"OKButton")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.OKButton.sizePolicy().hasHeightForWidth())
        self.OKButton.setSizePolicy(sizePolicy)

        self.verticalLayout.addWidget(self.OKButton)

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


        self.retranslateUi(MenuComputeSCCSimulation)
        self.OKButton.clicked.connect(MenuComputeSCCSimulation.compute)
        self.cancelButton.clicked.connect(MenuComputeSCCSimulation.reject)
        self.helpButton.clicked.connect(MenuComputeSCCSimulation.help)

        QMetaObject.connectSlotsByName(MenuComputeSCCSimulation)
    # setupUi

    def retranslateUi(self, MenuComputeSCCSimulation):
        MenuComputeSCCSimulation.setWindowTitle(QCoreApplication.translate("MenuComputeSCCSimulation", u"SCC SIMULATION", None))
        self.label_save_title.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"### Save Results In Lists", None))
        self.label_pre_recursive.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"Pre-recursive list name", None))
        self.lineEdit_pre_recursive_list_name.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"_PRE", None))
        self.label_inter_recursive.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"recursive list name", None))
        self.lineEdit_inter_recursive_list_name.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"_INTER", None))
        self.label_post_recursive.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"Post-recursive list name", None))
        self.lineEdit_post_recursive_list_name.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"_POST", None))
        self.label_max_iterations.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"Max. Iterations", None))
        self.label_simulation_to.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"To", None))
        self.checkBox_debug.setText("")
        self.label_debug.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"Debug", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setToolTip(QCoreApplication.translate("MenuComputeSCCSimulation", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setInputMask("")
        self.sampleEdit_sample_from.setPlaceholderText("")
        self.lineEdit_relaxation.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"1", None))
        self.label_initialisation.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"Initialisation", None))
        self.label_convergence.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"Convergence", None))
        self.label_relaxation.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"Relaxation", None))
        self.lineEdit_convergence.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"0.001", None))
        self.label_simulation_from.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"From", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setToolTip(QCoreApplication.translate("MenuComputeSCCSimulation", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setInputMask("")
        self.sampleEdit_sample_to.setPlaceholderText("")
        self.label_simulation.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"Simulation", None))
        self.OKButton.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"OK", None))
#if QT_CONFIG(shortcut)
        self.OKButton.setShortcut(QCoreApplication.translate("MenuComputeSCCSimulation", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.cancelButton.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.cancelButton.setShortcut(QCoreApplication.translate("MenuComputeSCCSimulation", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.helpButton.setText(QCoreApplication.translate("MenuComputeSCCSimulation", u"Help", None))
#if QT_CONFIG(shortcut)
        self.helpButton.setShortcut(QCoreApplication.translate("MenuComputeSCCSimulation", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

