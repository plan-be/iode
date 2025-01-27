# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'estimation_results.ui'
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
from PySide6.QtWidgets import (QAbstractItemView, QAbstractScrollArea, QApplication, QDialog,
    QGridLayout, QHeaderView, QPushButton, QSizePolicy,
    QSpacerItem, QTabWidget, QTableView, QWidget)

class Ui_EstimationResultsDialog(object):
    def setupUi(self, EstimationResultsDialog):
        if not EstimationResultsDialog.objectName():
            EstimationResultsDialog.setObjectName(u"EstimationResultsDialog")
        EstimationResultsDialog.resize(760, 380)
        self.gridLayout = QGridLayout(EstimationResultsDialog)
        self.gridLayout.setObjectName(u"gridLayout")
        self.pushButton_help = QPushButton(EstimationResultsDialog)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.gridLayout.addWidget(self.pushButton_help, 3, 1, 1, 1)

        self.pushButton_graph_residuals = QPushButton(EstimationResultsDialog)
        self.pushButton_graph_residuals.setObjectName(u"pushButton_graph_residuals")

        self.gridLayout.addWidget(self.pushButton_graph_residuals, 1, 1, 1, 1)

        self.verticalSpacer = QSpacerItem(20, 239, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 4, 1, 1, 1)

        self.pushButton_print_output = QPushButton(EstimationResultsDialog)
        self.pushButton_print_output.setObjectName(u"pushButton_print_output")

        self.gridLayout.addWidget(self.pushButton_print_output, 2, 1, 1, 1)

        self.pushButton_graph_YOBS = QPushButton(EstimationResultsDialog)
        self.pushButton_graph_YOBS.setObjectName(u"pushButton_graph_YOBS")

        self.gridLayout.addWidget(self.pushButton_graph_YOBS, 0, 1, 1, 1)

        self.tabWidget = QTabWidget(EstimationResultsDialog)
        self.tabWidget.setObjectName(u"tabWidget")
        self.tab_coefs = QWidget()
        self.tab_coefs.setObjectName(u"tab_coefs")
        self.gridLayout_2 = QGridLayout(self.tab_coefs)
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.tableView_coefs = QTableView(self.tab_coefs)
        self.tableView_coefs.setObjectName(u"tableView_coefs")
        self.tableView_coefs.setSizeAdjustPolicy(QAbstractScrollArea.AdjustIgnored)
        self.tableView_coefs.setEditTriggers(QAbstractItemView.AnyKeyPressed|QAbstractItemView.DoubleClicked|QAbstractItemView.EditKeyPressed)

        self.gridLayout_2.addWidget(self.tableView_coefs, 0, 0, 1, 1)

        self.tabWidget.addTab(self.tab_coefs, "")
        self.tab_corr = QWidget()
        self.tab_corr.setObjectName(u"tab_corr")
        self.gridLayout_3 = QGridLayout(self.tab_corr)
        self.gridLayout_3.setObjectName(u"gridLayout_3")
        self.tableView_corr_matrix = QTableView(self.tab_corr)
        self.tableView_corr_matrix.setObjectName(u"tableView_corr_matrix")
        self.tableView_corr_matrix.setSizeAdjustPolicy(QAbstractScrollArea.AdjustIgnored)
        self.tableView_corr_matrix.setEditTriggers(QAbstractItemView.NoEditTriggers)

        self.gridLayout_3.addWidget(self.tableView_corr_matrix, 0, 0, 1, 1)

        self.tabWidget.addTab(self.tab_corr, "")
        self.tab_tests = QWidget()
        self.tab_tests.setObjectName(u"tab_tests")
        self.gridLayout_4 = QGridLayout(self.tab_tests)
        self.gridLayout_4.setObjectName(u"gridLayout_4")
        self.tableView_tests = QTableView(self.tab_tests)
        self.tableView_tests.setObjectName(u"tableView_tests")
        self.tableView_tests.setSizeAdjustPolicy(QAbstractScrollArea.AdjustIgnored)
        self.tableView_tests.setEditTriggers(QAbstractItemView.NoEditTriggers)
        self.tableView_tests.setProperty(u"showDropIndicator", True)
        self.tableView_tests.setDragDropOverwriteMode(True)
        self.tableView_tests.setSelectionMode(QAbstractItemView.ContiguousSelection)

        self.gridLayout_4.addWidget(self.tableView_tests, 0, 0, 1, 1)

        self.tabWidget.addTab(self.tab_tests, "")

        self.gridLayout.addWidget(self.tabWidget, 0, 0, 5, 1)


        self.retranslateUi(EstimationResultsDialog)
        self.pushButton_graph_YOBS.clicked.connect(EstimationResultsDialog.plot_yobs_yest)
        self.pushButton_graph_residuals.clicked.connect(EstimationResultsDialog.plot_residual)
        self.pushButton_print_output.clicked.connect(EstimationResultsDialog.print_output)
        self.pushButton_help.clicked.connect(EstimationResultsDialog.help)

        self.tabWidget.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(EstimationResultsDialog)
    # setupUi

    def retranslateUi(self, EstimationResultsDialog):
        EstimationResultsDialog.setWindowTitle(QCoreApplication.translate("EstimationResultsDialog", u"Estimation Results", None))
        self.pushButton_help.setText(QCoreApplication.translate("EstimationResultsDialog", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("EstimationResultsDialog", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_graph_residuals.setText(QCoreApplication.translate("EstimationResultsDialog", u"Graph of Residuals", None))
        self.pushButton_print_output.setText(QCoreApplication.translate("EstimationResultsDialog", u"Print Output", None))
        self.pushButton_graph_YOBS.setText(QCoreApplication.translate("EstimationResultsDialog", u"Graph of YOBS-YEST", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_coefs), QCoreApplication.translate("EstimationResultsDialog", u"Coefficients", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_corr), QCoreApplication.translate("EstimationResultsDialog", u"Correlation Matrix", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_tests), QCoreApplication.translate("EstimationResultsDialog", u"Tests By Equation", None))
    # retranslateUi

