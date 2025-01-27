# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'file_contents.ui'
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
from PySide6.QtWidgets import (QApplication, QComboBox, QDialog, QFrame,
    QGridLayout, QLabel, QLineEdit, QPushButton,
    QSizePolicy, QSpacerItem, QTextEdit, QVBoxLayout,
    QWidget)

from util.widgets.file_chooser import IodeFileChooser

class Ui_MenuDataFileContents(object):
    def setupUi(self, MenuDataFileContents):
        if not MenuDataFileContents.objectName():
            MenuDataFileContents.setObjectName(u"MenuDataFileContents")
        MenuDataFileContents.resize(504, 246)
        MenuDataFileContents.setMinimumSize(QSize(0, 20))
        self.gridLayout = QGridLayout(MenuDataFileContents)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_search = QLabel(MenuDataFileContents)
        self.label_search.setObjectName(u"label_search")
        self.label_search.setMinimumSize(QSize(50, 0))
        self.label_search.setMaximumSize(QSize(50, 16777215))
        self.label_search.setAlignment(Qt.AlignLeading|Qt.AlignLeft|Qt.AlignVCenter)

        self.gridLayout.addWidget(self.label_search, 0, 0, 1, 1)

        self.lineEdit_search = QLineEdit(MenuDataFileContents)
        self.lineEdit_search.setObjectName(u"lineEdit_search")
        self.lineEdit_search.setMinimumSize(QSize(0, 20))
        self.lineEdit_search.setMaximumSize(QSize(16777215, 20))

        self.gridLayout.addWidget(self.lineEdit_search, 0, 1, 1, 4)

        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.pushButton_search = QPushButton(MenuDataFileContents)
        self.pushButton_search.setObjectName(u"pushButton_search")

        self.verticalLayout.addWidget(self.pushButton_search)

        self.pushButton_cancel = QPushButton(MenuDataFileContents)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.verticalLayout.addWidget(self.pushButton_cancel)

        self.pushButton_help = QPushButton(MenuDataFileContents)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.gridLayout.addLayout(self.verticalLayout, 0, 5, 3, 1)

        self.label_iode_types = QLabel(MenuDataFileContents)
        self.label_iode_types.setObjectName(u"label_iode_types")
        self.label_iode_types.setMinimumSize(QSize(50, 0))
        self.label_iode_types.setMaximumSize(QSize(50, 16777215))
        self.label_iode_types.setAlignment(Qt.AlignLeading|Qt.AlignLeft|Qt.AlignVCenter)

        self.gridLayout.addWidget(self.label_iode_types, 1, 0, 1, 1)

        self.comboBox_iode_types = QComboBox(MenuDataFileContents)
        self.comboBox_iode_types.setObjectName(u"comboBox_iode_types")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.comboBox_iode_types.sizePolicy().hasHeightForWidth())
        self.comboBox_iode_types.setSizePolicy(sizePolicy)
        self.comboBox_iode_types.setMinimumSize(QSize(150, 20))
        self.comboBox_iode_types.setMaximumSize(QSize(150, 20))

        self.gridLayout.addWidget(self.comboBox_iode_types, 1, 1, 1, 2)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 1, 3, 1, 2)

        self.label_input_file = QLabel(MenuDataFileContents)
        self.label_input_file.setObjectName(u"label_input_file")
        self.label_input_file.setMinimumSize(QSize(50, 0))
        self.label_input_file.setMaximumSize(QSize(50, 16777215))
        self.label_input_file.setAlignment(Qt.AlignLeading|Qt.AlignLeft|Qt.AlignVCenter)

        self.gridLayout.addWidget(self.label_input_file, 2, 0, 1, 1)

        self.fileChooser_input_file = IodeFileChooser(MenuDataFileContents)
        self.fileChooser_input_file.setObjectName(u"fileChooser_input_file")
        self.fileChooser_input_file.setMinimumSize(QSize(0, 20))

        self.gridLayout.addWidget(self.fileChooser_input_file, 2, 1, 1, 4)

        self.line = QFrame(MenuDataFileContents)
        self.line.setObjectName(u"line")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.line.sizePolicy().hasHeightForWidth())
        self.line.setSizePolicy(sizePolicy1)
        self.line.setFrameShape(QFrame.Shape.HLine)
        self.line.setFrameShadow(QFrame.Shadow.Sunken)

        self.gridLayout.addWidget(self.line, 3, 0, 1, 5)

        self.verticalSpacer = QSpacerItem(20, 133, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 3, 5, 4, 1)

        self.label_save_list = QLabel(MenuDataFileContents)
        self.label_save_list.setObjectName(u"label_save_list")
        self.label_save_list.setMinimumSize(QSize(100, 0))
        self.label_save_list.setMaximumSize(QSize(100, 16777215))

        self.gridLayout.addWidget(self.label_save_list, 4, 0, 1, 2)

        self.lineEdit_save_list = QLineEdit(MenuDataFileContents)
        self.lineEdit_save_list.setObjectName(u"lineEdit_save_list")
        self.lineEdit_save_list.setMinimumSize(QSize(0, 20))
        self.lineEdit_save_list.setMaximumSize(QSize(16777215, 20))

        self.gridLayout.addWidget(self.lineEdit_save_list, 4, 2, 1, 2)

        self.label_nb_elements = QLabel(MenuDataFileContents)
        self.label_nb_elements.setObjectName(u"label_nb_elements")
        self.label_nb_elements.setMinimumSize(QSize(80, 20))
        self.label_nb_elements.setMaximumSize(QSize(80, 20))

        self.gridLayout.addWidget(self.label_nb_elements, 4, 4, 1, 1)

        self.line_2 = QFrame(MenuDataFileContents)
        self.line_2.setObjectName(u"line_2")
        self.line_2.setFrameShape(QFrame.Shape.HLine)
        self.line_2.setFrameShadow(QFrame.Shadow.Sunken)

        self.gridLayout.addWidget(self.line_2, 5, 0, 1, 5)

        self.label_result = QLabel(MenuDataFileContents)
        self.label_result.setObjectName(u"label_result")

        self.gridLayout.addWidget(self.label_result, 6, 0, 1, 1)

        self.textEdit_result = QTextEdit(MenuDataFileContents)
        self.textEdit_result.setObjectName(u"textEdit_result")
        self.textEdit_result.setReadOnly(True)

        self.gridLayout.addWidget(self.textEdit_result, 6, 1, 1, 4)


        self.retranslateUi(MenuDataFileContents)
        self.pushButton_search.clicked.connect(MenuDataFileContents.search)
        self.pushButton_cancel.clicked.connect(MenuDataFileContents.reject)
        self.pushButton_help.clicked.connect(MenuDataFileContents.help)

        QMetaObject.connectSlotsByName(MenuDataFileContents)
    # setupUi

    def retranslateUi(self, MenuDataFileContents):
        MenuDataFileContents.setWindowTitle(QCoreApplication.translate("MenuDataFileContents", u"FILE OR WS CONTENTS", None))
        self.label_search.setText(QCoreApplication.translate("MenuDataFileContents", u"Search", None))
        self.pushButton_search.setText(QCoreApplication.translate("MenuDataFileContents", u"Search", None))
#if QT_CONFIG(shortcut)
        self.pushButton_search.setShortcut(QCoreApplication.translate("MenuDataFileContents", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuDataFileContents", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuDataFileContents", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuDataFileContents", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuDataFileContents", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label_iode_types.setText(QCoreApplication.translate("MenuDataFileContents", u"Of", None))
        self.label_input_file.setText(QCoreApplication.translate("MenuDataFileContents", u"In File", None))
        self.label_save_list.setText(QCoreApplication.translate("MenuDataFileContents", u"Save results in list", None))
        self.lineEdit_save_list.setText(QCoreApplication.translate("MenuDataFileContents", u"_RES", None))
        self.label_nb_elements.setText(QCoreApplication.translate("MenuDataFileContents", u"0 Elements", None))
        self.label_result.setText(QCoreApplication.translate("MenuDataFileContents", u"Result", None))
    # retranslateUi

