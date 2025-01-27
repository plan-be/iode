# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'graph_tables.ui'
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
    QSpacerItem, QSplitter, QTextEdit, QWidget)

from text_edit.complete_text_edit import IodeAutoCompleteTextEdit
from util.widgets.file_chooser import IodeFileChooser

class Ui_MenuGraphTables(object):
    def setupUi(self, MenuGraphTables):
        if not MenuGraphTables.objectName():
            MenuGraphTables.setObjectName(u"MenuGraphTables")
        MenuGraphTables.setWindowModality(Qt.ApplicationModal)
        MenuGraphTables.resize(600, 400)
        MenuGraphTables.setMinimumSize(QSize(600, 400))
        MenuGraphTables.setMaximumSize(QSize(600, 400))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuGraphTables.setWindowIcon(icon)
        self.gridLayout = QGridLayout(MenuGraphTables)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_table_names = QLabel(MenuGraphTables)
        self.label_table_names.setObjectName(u"label_table_names")
        self.label_table_names.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_table_names, 0, 0, 1, 1)

        self.textEdit_table_names = IodeAutoCompleteTextEdit(MenuGraphTables)
        self.textEdit_table_names.setObjectName(u"textEdit_table_names")

        self.gridLayout.addWidget(self.textEdit_table_names, 0, 1, 1, 2)

        self.splitter = QSplitter(MenuGraphTables)
        self.splitter.setObjectName(u"splitter")
        self.splitter.setOrientation(Qt.Vertical)
        self.pushButton_display = QPushButton(self.splitter)
        self.pushButton_display.setObjectName(u"pushButton_display")
        self.splitter.addWidget(self.pushButton_display)
        self.pushButton_setup = QPushButton(self.splitter)
        self.pushButton_setup.setObjectName(u"pushButton_setup")
        self.splitter.addWidget(self.pushButton_setup)
        self.pushButton_cancel = QPushButton(self.splitter)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")
        self.splitter.addWidget(self.pushButton_cancel)
        self.pushButton_help = QPushButton(self.splitter)
        self.pushButton_help.setObjectName(u"pushButton_help")
        self.splitter.addWidget(self.pushButton_help)

        self.gridLayout.addWidget(self.splitter, 0, 3, 2, 1)

        self.label_sample = QLabel(MenuGraphTables)
        self.label_sample.setObjectName(u"label_sample")
        self.label_sample.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_sample, 1, 0, 1, 1)

        self.textEdit_sample = QTextEdit(MenuGraphTables)
        self.textEdit_sample.setObjectName(u"textEdit_sample")

        self.gridLayout.addWidget(self.textEdit_sample, 1, 1, 2, 2)

        self.verticalSpacer = QSpacerItem(20, 196, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 2, 3, 7, 1)

        self.label_file_1 = QLabel(MenuGraphTables)
        self.label_file_1.setObjectName(u"label_file_1")
        self.label_file_1.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_file_1, 3, 0, 1, 1)

        self.lineEdit_file_1 = QLineEdit(MenuGraphTables)
        self.lineEdit_file_1.setObjectName(u"lineEdit_file_1")
        self.lineEdit_file_1.setEnabled(True)
        self.lineEdit_file_1.setReadOnly(True)
        self.lineEdit_file_1.setClearButtonEnabled(False)

        self.gridLayout.addWidget(self.lineEdit_file_1, 3, 1, 1, 2)

        self.label_file_2 = QLabel(MenuGraphTables)
        self.label_file_2.setObjectName(u"label_file_2")
        self.label_file_2.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_file_2, 4, 0, 1, 1)

        self.fileChooser_file_2 = IodeFileChooser(MenuGraphTables)
        self.fileChooser_file_2.setObjectName(u"fileChooser_file_2")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.fileChooser_file_2.sizePolicy().hasHeightForWidth())
        self.fileChooser_file_2.setSizePolicy(sizePolicy)
        self.fileChooser_file_2.setMinimumSize(QSize(0, 20))

        self.gridLayout.addWidget(self.fileChooser_file_2, 4, 1, 1, 2)

        self.label_file_3 = QLabel(MenuGraphTables)
        self.label_file_3.setObjectName(u"label_file_3")
        self.label_file_3.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_file_3, 5, 0, 1, 1)

        self.fileChooser_file_3 = IodeFileChooser(MenuGraphTables)
        self.fileChooser_file_3.setObjectName(u"fileChooser_file_3")
        sizePolicy.setHeightForWidth(self.fileChooser_file_3.sizePolicy().hasHeightForWidth())
        self.fileChooser_file_3.setSizePolicy(sizePolicy)
        self.fileChooser_file_3.setMinimumSize(QSize(0, 20))

        self.gridLayout.addWidget(self.fileChooser_file_3, 5, 1, 1, 2)

        self.label_file_4 = QLabel(MenuGraphTables)
        self.label_file_4.setObjectName(u"label_file_4")
        self.label_file_4.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_file_4, 6, 0, 1, 1)

        self.fileChooser_file_4 = IodeFileChooser(MenuGraphTables)
        self.fileChooser_file_4.setObjectName(u"fileChooser_file_4")
        sizePolicy.setHeightForWidth(self.fileChooser_file_4.sizePolicy().hasHeightForWidth())
        self.fileChooser_file_4.setSizePolicy(sizePolicy)
        self.fileChooser_file_4.setMinimumSize(QSize(0, 20))

        self.gridLayout.addWidget(self.fileChooser_file_4, 6, 1, 1, 2)

        self.label_file_5 = QLabel(MenuGraphTables)
        self.label_file_5.setObjectName(u"label_file_5")
        self.label_file_5.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_file_5, 7, 0, 1, 1)

        self.fileChooser_file_5 = IodeFileChooser(MenuGraphTables)
        self.fileChooser_file_5.setObjectName(u"fileChooser_file_5")
        sizePolicy.setHeightForWidth(self.fileChooser_file_5.sizePolicy().hasHeightForWidth())
        self.fileChooser_file_5.setSizePolicy(sizePolicy)
        self.fileChooser_file_5.setMinimumSize(QSize(0, 20))

        self.gridLayout.addWidget(self.fileChooser_file_5, 7, 1, 1, 2)

        self.label_language = QLabel(MenuGraphTables)
        self.label_language.setObjectName(u"label_language")
        self.label_language.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_language, 8, 0, 1, 1)

        self.comboBox_language = QComboBox(MenuGraphTables)
        self.comboBox_language.setObjectName(u"comboBox_language")
        self.comboBox_language.setMinimumSize(QSize(80, 0))

        self.gridLayout.addWidget(self.comboBox_language, 8, 1, 1, 1)

        self.horizontalSpacer = QSpacerItem(338, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 8, 2, 1, 1)


        self.retranslateUi(MenuGraphTables)
        self.pushButton_display.clicked.connect(MenuGraphTables.display)
        self.pushButton_setup.clicked.connect(MenuGraphTables.setup)
        self.pushButton_cancel.clicked.connect(MenuGraphTables.reject)
        self.pushButton_help.clicked.connect(MenuGraphTables.help)

        QMetaObject.connectSlotsByName(MenuGraphTables)
    # setupUi

    def retranslateUi(self, MenuGraphTables):
        MenuGraphTables.setWindowTitle(QCoreApplication.translate("MenuGraphTables", u"DISPLAY AND PRINT GRAPHICS", None))
        self.label_table_names.setText(QCoreApplication.translate("MenuGraphTables", u"Table Names", None))
        self.pushButton_display.setText(QCoreApplication.translate("MenuGraphTables", u"Display", None))
#if QT_CONFIG(shortcut)
        self.pushButton_display.setShortcut(QCoreApplication.translate("MenuGraphTables", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_setup.setText(QCoreApplication.translate("MenuGraphTables", u"Setup", None))
#if QT_CONFIG(shortcut)
        self.pushButton_setup.setShortcut(QCoreApplication.translate("MenuGraphTables", u"F6", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuGraphTables", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuGraphTables", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuGraphTables", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuGraphTables", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label_sample.setText(QCoreApplication.translate("MenuGraphTables", u"Sample", None))
        self.label_file_1.setText(QCoreApplication.translate("MenuGraphTables", u"File [1]", None))
        self.lineEdit_file_1.setText(QCoreApplication.translate("MenuGraphTables", u"Current workspace", None))
        self.label_file_2.setText(QCoreApplication.translate("MenuGraphTables", u"File [2]", None))
        self.label_file_3.setText(QCoreApplication.translate("MenuGraphTables", u"File [3]", None))
        self.label_file_4.setText(QCoreApplication.translate("MenuGraphTables", u"File [4]", None))
        self.label_file_5.setText(QCoreApplication.translate("MenuGraphTables", u"File [5]", None))
        self.label_language.setText(QCoreApplication.translate("MenuGraphTables", u"Language", None))
    # retranslateUi

