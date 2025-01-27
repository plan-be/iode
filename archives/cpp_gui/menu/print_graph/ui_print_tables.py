# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'print_tables.ui'
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
    QSpacerItem, QSpinBox, QSplitter, QTextEdit,
    QWidget)

from text_edit.complete_text_edit import IodeAutoCompleteTextEdit
from util.widgets.file_chooser import IodeFileChooser

class Ui_MenuPrintTables(object):
    def setupUi(self, MenuPrintTables):
        if not MenuPrintTables.objectName():
            MenuPrintTables.setObjectName(u"MenuPrintTables")
        MenuPrintTables.setWindowModality(Qt.ApplicationModal)
        MenuPrintTables.resize(600, 400)
        MenuPrintTables.setMinimumSize(QSize(600, 400))
        MenuPrintTables.setMaximumSize(QSize(600, 400))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuPrintTables.setWindowIcon(icon)
        self.gridLayout = QGridLayout(MenuPrintTables)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_table_names = QLabel(MenuPrintTables)
        self.label_table_names.setObjectName(u"label_table_names")
        self.label_table_names.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_table_names, 0, 0, 1, 1)

        self.textEdit_table_names = IodeAutoCompleteTextEdit(MenuPrintTables)
        self.textEdit_table_names.setObjectName(u"textEdit_table_names")

        self.gridLayout.addWidget(self.textEdit_table_names, 0, 1, 1, 4)

        self.splitter = QSplitter(MenuPrintTables)
        self.splitter.setObjectName(u"splitter")
        self.splitter.setOrientation(Qt.Vertical)
        self.pushButton_display = QPushButton(self.splitter)
        self.pushButton_display.setObjectName(u"pushButton_display")
        self.splitter.addWidget(self.pushButton_display)
        self.pushButton_print = QPushButton(self.splitter)
        self.pushButton_print.setObjectName(u"pushButton_print")
        self.splitter.addWidget(self.pushButton_print)
        self.pushButton_setup = QPushButton(self.splitter)
        self.pushButton_setup.setObjectName(u"pushButton_setup")
        self.splitter.addWidget(self.pushButton_setup)
        self.pushButton_cancel = QPushButton(self.splitter)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")
        self.splitter.addWidget(self.pushButton_cancel)
        self.pushButton_help = QPushButton(self.splitter)
        self.pushButton_help.setObjectName(u"pushButton_help")
        self.splitter.addWidget(self.pushButton_help)

        self.gridLayout.addWidget(self.splitter, 0, 5, 2, 1)

        self.label_sample = QLabel(MenuPrintTables)
        self.label_sample.setObjectName(u"label_sample")
        self.label_sample.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_sample, 1, 0, 1, 1)

        self.textEdit_sample = QTextEdit(MenuPrintTables)
        self.textEdit_sample.setObjectName(u"textEdit_sample")

        self.gridLayout.addWidget(self.textEdit_sample, 1, 1, 2, 4)

        self.verticalSpacer = QSpacerItem(20, 196, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 2, 5, 7, 1)

        self.label_file_1 = QLabel(MenuPrintTables)
        self.label_file_1.setObjectName(u"label_file_1")
        self.label_file_1.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_file_1, 3, 0, 1, 1)

        self.lineEdit_file_1 = QLineEdit(MenuPrintTables)
        self.lineEdit_file_1.setObjectName(u"lineEdit_file_1")
        self.lineEdit_file_1.setEnabled(True)
        self.lineEdit_file_1.setReadOnly(True)
        self.lineEdit_file_1.setClearButtonEnabled(False)

        self.gridLayout.addWidget(self.lineEdit_file_1, 3, 1, 1, 4)

        self.label_file_2 = QLabel(MenuPrintTables)
        self.label_file_2.setObjectName(u"label_file_2")
        self.label_file_2.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_file_2, 4, 0, 1, 1)

        self.fileChooser_file_2 = IodeFileChooser(MenuPrintTables)
        self.fileChooser_file_2.setObjectName(u"fileChooser_file_2")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.fileChooser_file_2.sizePolicy().hasHeightForWidth())
        self.fileChooser_file_2.setSizePolicy(sizePolicy)
        self.fileChooser_file_2.setMinimumSize(QSize(0, 20))

        self.gridLayout.addWidget(self.fileChooser_file_2, 4, 1, 1, 4)

        self.label_file_3 = QLabel(MenuPrintTables)
        self.label_file_3.setObjectName(u"label_file_3")
        self.label_file_3.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_file_3, 5, 0, 1, 1)

        self.fileChooser_file_3 = IodeFileChooser(MenuPrintTables)
        self.fileChooser_file_3.setObjectName(u"fileChooser_file_3")
        sizePolicy.setHeightForWidth(self.fileChooser_file_3.sizePolicy().hasHeightForWidth())
        self.fileChooser_file_3.setSizePolicy(sizePolicy)
        self.fileChooser_file_3.setMinimumSize(QSize(0, 20))

        self.gridLayout.addWidget(self.fileChooser_file_3, 5, 1, 1, 4)

        self.label_file_4 = QLabel(MenuPrintTables)
        self.label_file_4.setObjectName(u"label_file_4")
        self.label_file_4.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_file_4, 6, 0, 1, 1)

        self.fileChooser_file_4 = IodeFileChooser(MenuPrintTables)
        self.fileChooser_file_4.setObjectName(u"fileChooser_file_4")
        sizePolicy.setHeightForWidth(self.fileChooser_file_4.sizePolicy().hasHeightForWidth())
        self.fileChooser_file_4.setSizePolicy(sizePolicy)
        self.fileChooser_file_4.setMinimumSize(QSize(0, 20))

        self.gridLayout.addWidget(self.fileChooser_file_4, 6, 1, 1, 4)

        self.label_file_5 = QLabel(MenuPrintTables)
        self.label_file_5.setObjectName(u"label_file_5")
        self.label_file_5.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_file_5, 7, 0, 1, 1)

        self.fileChooser_file_5 = IodeFileChooser(MenuPrintTables)
        self.fileChooser_file_5.setObjectName(u"fileChooser_file_5")
        sizePolicy.setHeightForWidth(self.fileChooser_file_5.sizePolicy().hasHeightForWidth())
        self.fileChooser_file_5.setSizePolicy(sizePolicy)
        self.fileChooser_file_5.setMinimumSize(QSize(0, 20))

        self.gridLayout.addWidget(self.fileChooser_file_5, 7, 1, 1, 4)

        self.label_language = QLabel(MenuPrintTables)
        self.label_language.setObjectName(u"label_language")
        self.label_language.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_language, 8, 0, 1, 1)

        self.comboBox_language = QComboBox(MenuPrintTables)
        self.comboBox_language.setObjectName(u"comboBox_language")
        self.comboBox_language.setMinimumSize(QSize(120, 0))

        self.gridLayout.addWidget(self.comboBox_language, 8, 1, 1, 1)

        self.horizontalSpacer = QSpacerItem(172, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 8, 2, 1, 1)

        self.label_nb_decimals = QLabel(MenuPrintTables)
        self.label_nb_decimals.setObjectName(u"label_nb_decimals")
        self.label_nb_decimals.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_nb_decimals, 8, 3, 1, 1)

        self.spinBox_nb_decimals = QSpinBox(MenuPrintTables)
        self.spinBox_nb_decimals.setObjectName(u"spinBox_nb_decimals")
        self.spinBox_nb_decimals.setMinimumSize(QSize(50, 0))
        self.spinBox_nb_decimals.setMaximum(10)

        self.gridLayout.addWidget(self.spinBox_nb_decimals, 8, 4, 1, 1)


        self.retranslateUi(MenuPrintTables)
        self.pushButton_display.clicked.connect(MenuPrintTables.display)
        self.pushButton_print.clicked.connect(MenuPrintTables.print)
        self.pushButton_setup.clicked.connect(MenuPrintTables.setup)
        self.pushButton_cancel.clicked.connect(MenuPrintTables.reject)
        self.pushButton_help.clicked.connect(MenuPrintTables.help)

        QMetaObject.connectSlotsByName(MenuPrintTables)
    # setupUi

    def retranslateUi(self, MenuPrintTables):
        MenuPrintTables.setWindowTitle(QCoreApplication.translate("MenuPrintTables", u"PRINT AND DISPLAY TABLES", None))
        self.label_table_names.setText(QCoreApplication.translate("MenuPrintTables", u"Table Names", None))
        self.pushButton_display.setText(QCoreApplication.translate("MenuPrintTables", u"Display", None))
#if QT_CONFIG(shortcut)
        self.pushButton_display.setShortcut(QCoreApplication.translate("MenuPrintTables", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_print.setText(QCoreApplication.translate("MenuPrintTables", u"Print", None))
#if QT_CONFIG(shortcut)
        self.pushButton_print.setShortcut(QCoreApplication.translate("MenuPrintTables", u"F8", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_setup.setText(QCoreApplication.translate("MenuPrintTables", u"Setup", None))
#if QT_CONFIG(shortcut)
        self.pushButton_setup.setShortcut(QCoreApplication.translate("MenuPrintTables", u"F6", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuPrintTables", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuPrintTables", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuPrintTables", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuPrintTables", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label_sample.setText(QCoreApplication.translate("MenuPrintTables", u"Sample", None))
        self.label_file_1.setText(QCoreApplication.translate("MenuPrintTables", u"File [1]", None))
        self.lineEdit_file_1.setText(QCoreApplication.translate("MenuPrintTables", u"Current workspace", None))
        self.label_file_2.setText(QCoreApplication.translate("MenuPrintTables", u"File [2]", None))
        self.label_file_3.setText(QCoreApplication.translate("MenuPrintTables", u"File [3]", None))
        self.label_file_4.setText(QCoreApplication.translate("MenuPrintTables", u"File [4]", None))
        self.label_file_5.setText(QCoreApplication.translate("MenuPrintTables", u"File [5]", None))
        self.label_language.setText(QCoreApplication.translate("MenuPrintTables", u"Language", None))
        self.label_nb_decimals.setText(QCoreApplication.translate("MenuPrintTables", u"nb decimals", None))
    # retranslateUi

