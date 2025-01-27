# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'compute_simulation.ui'
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
    QSizePolicy, QSpacerItem, QSpinBox, QWidget)

from text_edit.complete_text_edit import IodeAutoCompleteTextEdit
from util.widgets.sample_edit import IodeSampleEdit

class Ui_MenuComputeSimulation(object):
    def setupUi(self, MenuComputeSimulation):
        if not MenuComputeSimulation.objectName():
            MenuComputeSimulation.setObjectName(u"MenuComputeSimulation")
        MenuComputeSimulation.resize(620, 350)
        MenuComputeSimulation.setMinimumSize(QSize(620, 350))
        MenuComputeSimulation.setMaximumSize(QSize(620, 350))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuComputeSimulation.setWindowIcon(icon)
        self.gridLayout = QGridLayout(MenuComputeSimulation)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_equations_list = QLabel(MenuComputeSimulation)
        self.label_equations_list.setObjectName(u"label_equations_list")
        self.label_equations_list.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_equations_list, 0, 0, 3, 1)

        self.textEdit_equations_list = IodeAutoCompleteTextEdit(MenuComputeSimulation)
        self.textEdit_equations_list.setObjectName(u"textEdit_equations_list")

        self.gridLayout.addWidget(self.textEdit_equations_list, 0, 1, 4, 6)

        self.OKButton = QPushButton(MenuComputeSimulation)
        self.OKButton.setObjectName(u"OKButton")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.OKButton.sizePolicy().hasHeightForWidth())
        self.OKButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.OKButton, 0, 7, 1, 1)

        self.cancelButton = QPushButton(MenuComputeSimulation)
        self.cancelButton.setObjectName(u"cancelButton")
        sizePolicy.setHeightForWidth(self.cancelButton.sizePolicy().hasHeightForWidth())
        self.cancelButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.cancelButton, 1, 7, 1, 1)

        self.helpButton = QPushButton(MenuComputeSimulation)
        self.helpButton.setObjectName(u"helpButton")
        sizePolicy.setHeightForWidth(self.helpButton.sizePolicy().hasHeightForWidth())
        self.helpButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.helpButton, 2, 7, 1, 1)

        self.verticalSpacer = QSpacerItem(20, 239, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 3, 7, 6, 1)

        self.label_exchange = QLabel(MenuComputeSimulation)
        self.label_exchange.setObjectName(u"label_exchange")
        self.label_exchange.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_exchange, 4, 0, 1, 1)

        self.textEdit_exchange = IodeAutoCompleteTextEdit(MenuComputeSimulation)
        self.textEdit_exchange.setObjectName(u"textEdit_exchange")

        self.gridLayout.addWidget(self.textEdit_exchange, 4, 1, 1, 6)

        self.label_simulation = QLabel(MenuComputeSimulation)
        self.label_simulation.setObjectName(u"label_simulation")

        self.gridLayout.addWidget(self.label_simulation, 5, 0, 1, 1)

        self.label_simulation_from = QLabel(MenuComputeSimulation)
        self.label_simulation_from.setObjectName(u"label_simulation_from")

        self.gridLayout.addWidget(self.label_simulation_from, 5, 1, 1, 1)

        self.sampleEdit_sample_from = IodeSampleEdit(MenuComputeSimulation)
        self.sampleEdit_sample_from.setObjectName(u"sampleEdit_sample_from")

        self.gridLayout.addWidget(self.sampleEdit_sample_from, 5, 2, 1, 1)

        self.label_simulation_to = QLabel(MenuComputeSimulation)
        self.label_simulation_to.setObjectName(u"label_simulation_to")

        self.gridLayout.addWidget(self.label_simulation_to, 5, 3, 1, 1)

        self.sampleEdit_sample_to = IodeSampleEdit(MenuComputeSimulation)
        self.sampleEdit_sample_to.setObjectName(u"sampleEdit_sample_to")

        self.gridLayout.addWidget(self.sampleEdit_sample_to, 5, 4, 1, 1)

        self.label_convergence = QLabel(MenuComputeSimulation)
        self.label_convergence.setObjectName(u"label_convergence")

        self.gridLayout.addWidget(self.label_convergence, 5, 5, 1, 1)

        self.lineEdit_convergence = QLineEdit(MenuComputeSimulation)
        self.lineEdit_convergence.setObjectName(u"lineEdit_convergence")

        self.gridLayout.addWidget(self.lineEdit_convergence, 5, 6, 1, 1)

        self.label_max_iterations = QLabel(MenuComputeSimulation)
        self.label_max_iterations.setObjectName(u"label_max_iterations")

        self.gridLayout.addWidget(self.label_max_iterations, 6, 0, 1, 1)

        self.spinBox_max_iterations = QSpinBox(MenuComputeSimulation)
        self.spinBox_max_iterations.setObjectName(u"spinBox_max_iterations")
        self.spinBox_max_iterations.setMaximum(1000000)
        self.spinBox_max_iterations.setValue(100)
        self.spinBox_max_iterations.setDisplayIntegerBase(10)

        self.gridLayout.addWidget(self.spinBox_max_iterations, 6, 1, 1, 2)

        self.label_debug = QLabel(MenuComputeSimulation)
        self.label_debug.setObjectName(u"label_debug")

        self.gridLayout.addWidget(self.label_debug, 6, 3, 1, 1)

        self.checkBox_debug = QCheckBox(MenuComputeSimulation)
        self.checkBox_debug.setObjectName(u"checkBox_debug")

        self.gridLayout.addWidget(self.checkBox_debug, 6, 4, 1, 1)

        self.label_relaxation = QLabel(MenuComputeSimulation)
        self.label_relaxation.setObjectName(u"label_relaxation")

        self.gridLayout.addWidget(self.label_relaxation, 6, 5, 1, 1)

        self.lineEdit_relaxation = QLineEdit(MenuComputeSimulation)
        self.lineEdit_relaxation.setObjectName(u"lineEdit_relaxation")

        self.gridLayout.addWidget(self.lineEdit_relaxation, 6, 6, 1, 1)

        self.label_initialisation = QLabel(MenuComputeSimulation)
        self.label_initialisation.setObjectName(u"label_initialisation")

        self.gridLayout.addWidget(self.label_initialisation, 7, 0, 1, 1)

        self.comboBox_initialization = QComboBox(MenuComputeSimulation)
        self.comboBox_initialization.setObjectName(u"comboBox_initialization")
        self.comboBox_initialization.setMinimumSize(QSize(240, 0))

        self.gridLayout.addWidget(self.comboBox_initialization, 7, 1, 1, 4)

        self.horizontalSpacer = QSpacerItem(159, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 7, 5, 1, 2)

        self.label_sort_algorithm = QLabel(MenuComputeSimulation)
        self.label_sort_algorithm.setObjectName(u"label_sort_algorithm")

        self.gridLayout.addWidget(self.label_sort_algorithm, 8, 0, 1, 1)

        self.comboBox_sort_algorithm = QComboBox(MenuComputeSimulation)
        self.comboBox_sort_algorithm.setObjectName(u"comboBox_sort_algorithm")
        self.comboBox_sort_algorithm.setMinimumSize(QSize(240, 0))

        self.gridLayout.addWidget(self.comboBox_sort_algorithm, 8, 1, 1, 4)

        self.label_passes = QLabel(MenuComputeSimulation)
        self.label_passes.setObjectName(u"label_passes")

        self.gridLayout.addWidget(self.label_passes, 8, 5, 1, 1)

        self.spinBox_nb_passes = QSpinBox(MenuComputeSimulation)
        self.spinBox_nb_passes.setObjectName(u"spinBox_nb_passes")
        self.spinBox_nb_passes.setValue(5)

        self.gridLayout.addWidget(self.spinBox_nb_passes, 8, 6, 1, 1)


        self.retranslateUi(MenuComputeSimulation)
        self.OKButton.clicked.connect(MenuComputeSimulation.compute)
        self.cancelButton.clicked.connect(MenuComputeSimulation.reject)
        self.helpButton.clicked.connect(MenuComputeSimulation.help)

        QMetaObject.connectSlotsByName(MenuComputeSimulation)
    # setupUi

    def retranslateUi(self, MenuComputeSimulation):
        MenuComputeSimulation.setWindowTitle(QCoreApplication.translate("MenuComputeSimulation", u"MODEL SIMULATION", None))
        self.label_equations_list.setText(QCoreApplication.translate("MenuComputeSimulation", u"Equations List\n"
"\n"
"(empty for\n"
"all equations)", None))
        self.OKButton.setText(QCoreApplication.translate("MenuComputeSimulation", u"OK", None))
#if QT_CONFIG(shortcut)
        self.OKButton.setShortcut(QCoreApplication.translate("MenuComputeSimulation", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.cancelButton.setText(QCoreApplication.translate("MenuComputeSimulation", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.cancelButton.setShortcut(QCoreApplication.translate("MenuComputeSimulation", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.helpButton.setText(QCoreApplication.translate("MenuComputeSimulation", u"Help", None))
#if QT_CONFIG(shortcut)
        self.helpButton.setShortcut(QCoreApplication.translate("MenuComputeSimulation", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label_exchange.setText(QCoreApplication.translate("MenuComputeSimulation", u"Exchange (EN-EX)", None))
        self.label_simulation.setText(QCoreApplication.translate("MenuComputeSimulation", u"Simulation", None))
        self.label_simulation_from.setText(QCoreApplication.translate("MenuComputeSimulation", u"From", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setToolTip(QCoreApplication.translate("MenuComputeSimulation", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setInputMask("")
        self.sampleEdit_sample_from.setPlaceholderText("")
        self.label_simulation_to.setText(QCoreApplication.translate("MenuComputeSimulation", u"To", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setToolTip(QCoreApplication.translate("MenuComputeSimulation", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setInputMask("")
        self.sampleEdit_sample_to.setPlaceholderText("")
        self.label_convergence.setText(QCoreApplication.translate("MenuComputeSimulation", u"Convergence", None))
        self.lineEdit_convergence.setText(QCoreApplication.translate("MenuComputeSimulation", u"0.001", None))
        self.label_max_iterations.setText(QCoreApplication.translate("MenuComputeSimulation", u"Max. Iterations", None))
        self.label_debug.setText(QCoreApplication.translate("MenuComputeSimulation", u"Debug", None))
        self.checkBox_debug.setText("")
        self.label_relaxation.setText(QCoreApplication.translate("MenuComputeSimulation", u"Relaxation", None))
        self.lineEdit_relaxation.setText(QCoreApplication.translate("MenuComputeSimulation", u"1", None))
        self.label_initialisation.setText(QCoreApplication.translate("MenuComputeSimulation", u"Initialisation", None))
        self.label_sort_algorithm.setText(QCoreApplication.translate("MenuComputeSimulation", u"Sort Algorithm", None))
        self.label_passes.setText(QCoreApplication.translate("MenuComputeSimulation", u"Passes", None))
    # retranslateUi

