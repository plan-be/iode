# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'edit_equation.ui'
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
    QLabel, QLineEdit, QPushButton, QSizePolicy,
    QSpacerItem, QVBoxLayout, QWidget)

from text_edit.complete_text_edit import IodeAutoCompleteTextEdit
from util.widgets.sample_edit import IodeSampleEdit

class Ui_EditEquationDialog(object):
    def setupUi(self, EditEquationDialog):
        if not EditEquationDialog.objectName():
            EditEquationDialog.setObjectName(u"EditEquationDialog")
        EditEquationDialog.setWindowModality(Qt.NonModal)
        EditEquationDialog.resize(620, 300)
        EditEquationDialog.setMinimumSize(QSize(100, 0))
        self.gridLayout = QGridLayout(EditEquationDialog)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_name = QLabel(EditEquationDialog)
        self.label_name.setObjectName(u"label_name")

        self.gridLayout.addWidget(self.label_name, 0, 0, 1, 1)

        self.lineEdit_name = QLineEdit(EditEquationDialog)
        self.lineEdit_name.setObjectName(u"lineEdit_name")
        self.lineEdit_name.setReadOnly(True)

        self.gridLayout.addWidget(self.lineEdit_name, 0, 1, 1, 1)

        self.label_tests_r2adj = QLabel(EditEquationDialog)
        self.label_tests_r2adj.setObjectName(u"label_tests_r2adj")

        self.gridLayout.addWidget(self.label_tests_r2adj, 0, 2, 1, 1)

        self.lineEdit_tests_r2adj = QLineEdit(EditEquationDialog)
        self.lineEdit_tests_r2adj.setObjectName(u"lineEdit_tests_r2adj")
        self.lineEdit_tests_r2adj.setReadOnly(True)

        self.gridLayout.addWidget(self.lineEdit_tests_r2adj, 0, 3, 1, 1)

        self.label_tests_durbw = QLabel(EditEquationDialog)
        self.label_tests_durbw.setObjectName(u"label_tests_durbw")

        self.gridLayout.addWidget(self.label_tests_durbw, 0, 4, 1, 1)

        self.lineEdit_tests_durbw = QLineEdit(EditEquationDialog)
        self.lineEdit_tests_durbw.setObjectName(u"lineEdit_tests_durbw")
        self.lineEdit_tests_durbw.setReadOnly(True)

        self.gridLayout.addWidget(self.lineEdit_tests_durbw, 0, 5, 1, 1)

        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.pushButton_save = QPushButton(EditEquationDialog)
        self.pushButton_save.setObjectName(u"pushButton_save")

        self.verticalLayout.addWidget(self.pushButton_save)

        self.pushButton_quit = QPushButton(EditEquationDialog)
        self.pushButton_quit.setObjectName(u"pushButton_quit")

        self.verticalLayout.addWidget(self.pushButton_quit)

        self.pushButton_coefs = QPushButton(EditEquationDialog)
        self.pushButton_coefs.setObjectName(u"pushButton_coefs")

        self.verticalLayout.addWidget(self.pushButton_coefs)

        self.pushButton_estimate = QPushButton(EditEquationDialog)
        self.pushButton_estimate.setObjectName(u"pushButton_estimate")

        self.verticalLayout.addWidget(self.pushButton_estimate)

        self.pushButton_next = QPushButton(EditEquationDialog)
        self.pushButton_next.setObjectName(u"pushButton_next")

        self.verticalLayout.addWidget(self.pushButton_next)

        self.pushButton_dynamic_adjustment = QPushButton(EditEquationDialog)
        self.pushButton_dynamic_adjustment.setObjectName(u"pushButton_dynamic_adjustment")

        self.verticalLayout.addWidget(self.pushButton_dynamic_adjustment)

        self.pushButton_results = QPushButton(EditEquationDialog)
        self.pushButton_results.setObjectName(u"pushButton_results")

        self.verticalLayout.addWidget(self.pushButton_results)

        self.pushButton_unit_root = QPushButton(EditEquationDialog)
        self.pushButton_unit_root.setObjectName(u"pushButton_unit_root")

        self.verticalLayout.addWidget(self.pushButton_unit_root)

        self.pushButton_help = QPushButton(EditEquationDialog)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.gridLayout.addLayout(self.verticalLayout, 0, 6, 7, 1)

        self.horizontalSpacer_4 = QSpacerItem(185, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_4, 1, 0, 1, 2)

        self.label_tests_fstat = QLabel(EditEquationDialog)
        self.label_tests_fstat.setObjectName(u"label_tests_fstat")

        self.gridLayout.addWidget(self.label_tests_fstat, 1, 2, 1, 1)

        self.lineEdit_tests_fstat = QLineEdit(EditEquationDialog)
        self.lineEdit_tests_fstat.setObjectName(u"lineEdit_tests_fstat")
        self.lineEdit_tests_fstat.setReadOnly(True)

        self.gridLayout.addWidget(self.lineEdit_tests_fstat, 1, 3, 1, 1)

        self.label_tests_loglk = QLabel(EditEquationDialog)
        self.label_tests_loglk.setObjectName(u"label_tests_loglk")

        self.gridLayout.addWidget(self.label_tests_loglk, 1, 4, 1, 1)

        self.lineEdit_tests_loglk = QLineEdit(EditEquationDialog)
        self.lineEdit_tests_loglk.setObjectName(u"lineEdit_tests_loglk")
        self.lineEdit_tests_loglk.setReadOnly(True)

        self.gridLayout.addWidget(self.lineEdit_tests_loglk, 1, 5, 1, 1)

        self.label_method = QLabel(EditEquationDialog)
        self.label_method.setObjectName(u"label_method")

        self.gridLayout.addWidget(self.label_method, 2, 0, 1, 1)

        self.comboBox_method = QComboBox(EditEquationDialog)
        self.comboBox_method.setObjectName(u"comboBox_method")
        self.comboBox_method.setMinimumSize(QSize(100, 0))

        self.gridLayout.addWidget(self.comboBox_method, 2, 1, 1, 1)

        self.label_from = QLabel(EditEquationDialog)
        self.label_from.setObjectName(u"label_from")

        self.gridLayout.addWidget(self.label_from, 2, 2, 1, 1)

        self.sampleEdit_from = IodeSampleEdit(EditEquationDialog)
        self.sampleEdit_from.setObjectName(u"sampleEdit_from")

        self.gridLayout.addWidget(self.sampleEdit_from, 2, 3, 1, 1)

        self.label_to = QLabel(EditEquationDialog)
        self.label_to.setObjectName(u"label_to")

        self.gridLayout.addWidget(self.label_to, 2, 4, 1, 1)

        self.sampleEdit_to = IodeSampleEdit(EditEquationDialog)
        self.sampleEdit_to.setObjectName(u"sampleEdit_to")

        self.gridLayout.addWidget(self.sampleEdit_to, 2, 5, 1, 1)

        self.label_lec = QLabel(EditEquationDialog)
        self.label_lec.setObjectName(u"label_lec")

        self.gridLayout.addWidget(self.label_lec, 3, 0, 1, 1)

        self.textEdit_lec = IodeAutoCompleteTextEdit(EditEquationDialog)
        self.textEdit_lec.setObjectName(u"textEdit_lec")

        self.gridLayout.addWidget(self.textEdit_lec, 3, 1, 1, 5)

        self.label_comment = QLabel(EditEquationDialog)
        self.label_comment.setObjectName(u"label_comment")

        self.gridLayout.addWidget(self.label_comment, 4, 0, 1, 1)

        self.lineEdit_comment = QLineEdit(EditEquationDialog)
        self.lineEdit_comment.setObjectName(u"lineEdit_comment")
        self.lineEdit_comment.setReadOnly(False)

        self.gridLayout.addWidget(self.lineEdit_comment, 4, 1, 1, 2)

        self.horizontalSpacer = QSpacerItem(286, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 4, 3, 1, 3)

        self.label_block = QLabel(EditEquationDialog)
        self.label_block.setObjectName(u"label_block")

        self.gridLayout.addWidget(self.label_block, 5, 0, 1, 1)

        self.lineEdit_block = QLineEdit(EditEquationDialog)
        self.lineEdit_block.setObjectName(u"lineEdit_block")
        self.lineEdit_block.setReadOnly(False)

        self.gridLayout.addWidget(self.lineEdit_block, 5, 1, 1, 2)

        self.horizontalSpacer_2 = QSpacerItem(286, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_2, 5, 3, 1, 3)

        self.label_instruments = QLabel(EditEquationDialog)
        self.label_instruments.setObjectName(u"label_instruments")

        self.gridLayout.addWidget(self.label_instruments, 6, 0, 1, 1)

        self.lineEdit_instruments = QLineEdit(EditEquationDialog)
        self.lineEdit_instruments.setObjectName(u"lineEdit_instruments")
        self.lineEdit_instruments.setReadOnly(False)

        self.gridLayout.addWidget(self.lineEdit_instruments, 6, 1, 1, 2)

        self.horizontalSpacer_3 = QSpacerItem(286, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_3, 6, 3, 1, 3)


        self.retranslateUi(EditEquationDialog)
        self.pushButton_save.clicked.connect(EditEquationDialog.edit)
        self.pushButton_help.clicked.connect(EditEquationDialog.help)
        self.pushButton_coefs.clicked.connect(EditEquationDialog.display_coefs)
        self.pushButton_estimate.clicked.connect(EditEquationDialog.estimate)
        self.pushButton_next.clicked.connect(EditEquationDialog.next)
        self.pushButton_dynamic_adjustment.clicked.connect(EditEquationDialog.dynamic_adjustment)
        self.pushButton_results.clicked.connect(EditEquationDialog.results)
        self.pushButton_unit_root.clicked.connect(EditEquationDialog.unit_root)
        self.pushButton_quit.clicked.connect(EditEquationDialog.reject)

        QMetaObject.connectSlotsByName(EditEquationDialog)
    # setupUi

    def retranslateUi(self, EditEquationDialog):
        EditEquationDialog.setWindowTitle(QCoreApplication.translate("EditEquationDialog", u"Edit Equation", None))
        self.label_name.setText(QCoreApplication.translate("EditEquationDialog", u"Name", None))
        self.label_tests_r2adj.setText(QCoreApplication.translate("EditEquationDialog", u"R\u00b2Adj", None))
        self.label_tests_durbw.setText(QCoreApplication.translate("EditEquationDialog", u"DurbW", None))
        self.pushButton_save.setText(QCoreApplication.translate("EditEquationDialog", u"Save", None))
#if QT_CONFIG(shortcut)
        self.pushButton_save.setShortcut(QCoreApplication.translate("EditEquationDialog", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_quit.setText(QCoreApplication.translate("EditEquationDialog", u"Quit", None))
#if QT_CONFIG(shortcut)
        self.pushButton_quit.setShortcut(QCoreApplication.translate("EditEquationDialog", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_coefs.setText(QCoreApplication.translate("EditEquationDialog", u"Coefs", None))
#if QT_CONFIG(shortcut)
        self.pushButton_coefs.setShortcut(QCoreApplication.translate("EditEquationDialog", u"F4", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_estimate.setText(QCoreApplication.translate("EditEquationDialog", u"Estimate", None))
#if QT_CONFIG(shortcut)
        self.pushButton_estimate.setShortcut(QCoreApplication.translate("EditEquationDialog", u"F5", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_next.setText(QCoreApplication.translate("EditEquationDialog", u"Next", None))
#if QT_CONFIG(shortcut)
        self.pushButton_next.setShortcut(QCoreApplication.translate("EditEquationDialog", u"F6", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_dynamic_adjustment.setText(QCoreApplication.translate("EditEquationDialog", u"Dyn. Adj.", None))
#if QT_CONFIG(shortcut)
        self.pushButton_dynamic_adjustment.setShortcut(QCoreApplication.translate("EditEquationDialog", u"F7", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_results.setText(QCoreApplication.translate("EditEquationDialog", u"Results", None))
#if QT_CONFIG(shortcut)
        self.pushButton_results.setShortcut(QCoreApplication.translate("EditEquationDialog", u"F8", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_unit_root.setText(QCoreApplication.translate("EditEquationDialog", u"Unit Root", None))
#if QT_CONFIG(shortcut)
        self.pushButton_unit_root.setShortcut(QCoreApplication.translate("EditEquationDialog", u"F3", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("EditEquationDialog", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("EditEquationDialog", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label_tests_fstat.setText(QCoreApplication.translate("EditEquationDialog", u"Fstat", None))
        self.label_tests_loglk.setText(QCoreApplication.translate("EditEquationDialog", u"LogLk", None))
        self.label_method.setText(QCoreApplication.translate("EditEquationDialog", u"Method", None))
        self.label_from.setText(QCoreApplication.translate("EditEquationDialog", u"From", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_from.setToolTip(QCoreApplication.translate("EditEquationDialog", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_from.setInputMask("")
        self.sampleEdit_from.setPlaceholderText("")
        self.label_to.setText(QCoreApplication.translate("EditEquationDialog", u"To", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_to.setToolTip(QCoreApplication.translate("EditEquationDialog", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_to.setInputMask("")
        self.sampleEdit_to.setPlaceholderText("")
        self.label_lec.setText(QCoreApplication.translate("EditEquationDialog", u"Lec", None))
        self.label_comment.setText(QCoreApplication.translate("EditEquationDialog", u"Comment", None))
        self.label_block.setText(QCoreApplication.translate("EditEquationDialog", u"Block", None))
        self.label_instruments.setText(QCoreApplication.translate("EditEquationDialog", u"Instruments", None))
    # retranslateUi

