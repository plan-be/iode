# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'search_text.ui'
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
    QFrame, QGridLayout, QLabel, QLineEdit,
    QPushButton, QSizePolicy, QSpacerItem, QTextEdit,
    QVBoxLayout, QWidget)

class Ui_MenuDataSearchText(object):
    def setupUi(self, MenuDataSearchText):
        if not MenuDataSearchText.objectName():
            MenuDataSearchText.setObjectName(u"MenuDataSearchText")
        MenuDataSearchText.resize(559, 335)
        self.gridLayout = QGridLayout(MenuDataSearchText)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_text = QLabel(MenuDataSearchText)
        self.label_text.setObjectName(u"label_text")

        self.gridLayout.addWidget(self.label_text, 0, 0, 1, 1)

        self.lineEdit_text = QLineEdit(MenuDataSearchText)
        self.lineEdit_text.setObjectName(u"lineEdit_text")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.lineEdit_text.sizePolicy().hasHeightForWidth())
        self.lineEdit_text.setSizePolicy(sizePolicy)
        self.lineEdit_text.setMinimumSize(QSize(270, 20))
        self.lineEdit_text.setMaximumSize(QSize(10000, 20))

        self.gridLayout.addWidget(self.lineEdit_text, 0, 1, 1, 7)

        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.pushButton_search = QPushButton(MenuDataSearchText)
        self.pushButton_search.setObjectName(u"pushButton_search")

        self.verticalLayout.addWidget(self.pushButton_search)

        self.pushButton_cancel = QPushButton(MenuDataSearchText)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.verticalLayout.addWidget(self.pushButton_cancel)

        self.pushButton_help = QPushButton(MenuDataSearchText)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.gridLayout.addLayout(self.verticalLayout, 0, 8, 3, 1)

        self.checkBox_whole_word = QCheckBox(MenuDataSearchText)
        self.checkBox_whole_word.setObjectName(u"checkBox_whole_word")

        self.gridLayout.addWidget(self.checkBox_whole_word, 1, 0, 1, 4)

        self.checkBox_exact_case = QCheckBox(MenuDataSearchText)
        self.checkBox_exact_case.setObjectName(u"checkBox_exact_case")

        self.gridLayout.addWidget(self.checkBox_exact_case, 1, 4, 1, 2)

        self.line = QFrame(MenuDataSearchText)
        self.line.setObjectName(u"line")
        sizePolicy.setHeightForWidth(self.line.sizePolicy().hasHeightForWidth())
        self.line.setSizePolicy(sizePolicy)
        self.line.setFrameShape(QFrame.Shape.HLine)
        self.line.setFrameShadow(QFrame.Shadow.Sunken)

        self.gridLayout.addWidget(self.line, 2, 0, 1, 8)

        self.label_objs_to_scan = QLabel(MenuDataSearchText)
        self.label_objs_to_scan.setObjectName(u"label_objs_to_scan")

        self.gridLayout.addWidget(self.label_objs_to_scan, 3, 0, 1, 3)

        self.comboBox_iode_types = QComboBox(MenuDataSearchText)
        self.comboBox_iode_types.setObjectName(u"comboBox_iode_types")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.comboBox_iode_types.sizePolicy().hasHeightForWidth())
        self.comboBox_iode_types.setSizePolicy(sizePolicy1)
        self.comboBox_iode_types.setMinimumSize(QSize(180, 20))
        self.comboBox_iode_types.setMaximumSize(QSize(180, 20))

        self.gridLayout.addWidget(self.comboBox_iode_types, 3, 3, 1, 3)

        self.horizontalSpacer = QSpacerItem(181, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 3, 6, 1, 2)

        self.verticalSpacer = QSpacerItem(20, 222, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 3, 8, 6, 1)

        self.checkBox_search_formulas = QCheckBox(MenuDataSearchText)
        self.checkBox_search_formulas.setObjectName(u"checkBox_search_formulas")

        self.gridLayout.addWidget(self.checkBox_search_formulas, 4, 4, 1, 1)

        self.checkBox_search_free_text = QCheckBox(MenuDataSearchText)
        self.checkBox_search_free_text.setObjectName(u"checkBox_search_free_text")

        self.gridLayout.addWidget(self.checkBox_search_free_text, 4, 5, 1, 2)

        self.horizontalSpacer_2 = QSpacerItem(159, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_2, 4, 7, 1, 1)

        self.line_2 = QFrame(MenuDataSearchText)
        self.line_2.setObjectName(u"line_2")
        sizePolicy.setHeightForWidth(self.line_2.sizePolicy().hasHeightForWidth())
        self.line_2.setSizePolicy(sizePolicy)
        self.line_2.setFrameShape(QFrame.Shape.HLine)
        self.line_2.setFrameShadow(QFrame.Shadow.Sunken)

        self.gridLayout.addWidget(self.line_2, 5, 0, 1, 8)

        self.label_save_list = QLabel(MenuDataSearchText)
        self.label_save_list.setObjectName(u"label_save_list")
        sizePolicy2 = QSizePolicy(QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Preferred)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.label_save_list.sizePolicy().hasHeightForWidth())
        self.label_save_list.setSizePolicy(sizePolicy2)

        self.gridLayout.addWidget(self.label_save_list, 6, 0, 1, 4)

        self.lineEdit_save_list = QLineEdit(MenuDataSearchText)
        self.lineEdit_save_list.setObjectName(u"lineEdit_save_list")
        self.lineEdit_save_list.setMinimumSize(QSize(170, 20))
        self.lineEdit_save_list.setMaximumSize(QSize(100000, 20))

        self.gridLayout.addWidget(self.lineEdit_save_list, 6, 4, 1, 4)

        self.line_3 = QFrame(MenuDataSearchText)
        self.line_3.setObjectName(u"line_3")
        sizePolicy.setHeightForWidth(self.line_3.sizePolicy().hasHeightForWidth())
        self.line_3.setSizePolicy(sizePolicy)
        self.line_3.setFrameShape(QFrame.Shape.HLine)
        self.line_3.setFrameShadow(QFrame.Shadow.Sunken)

        self.gridLayout.addWidget(self.line_3, 7, 0, 1, 8)

        self.label_result = QLabel(MenuDataSearchText)
        self.label_result.setObjectName(u"label_result")

        self.gridLayout.addWidget(self.label_result, 8, 0, 1, 2)

        self.textEdit_result = QTextEdit(MenuDataSearchText)
        self.textEdit_result.setObjectName(u"textEdit_result")

        self.gridLayout.addWidget(self.textEdit_result, 8, 2, 1, 6)

        self.horizontalSpacer_3 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_3, 1, 6, 1, 2)

        self.checkBox_search_names = QCheckBox(MenuDataSearchText)
        self.checkBox_search_names.setObjectName(u"checkBox_search_names")

        self.gridLayout.addWidget(self.checkBox_search_names, 4, 0, 1, 4)


        self.retranslateUi(MenuDataSearchText)
        self.pushButton_search.clicked.connect(MenuDataSearchText.search)
        self.pushButton_cancel.clicked.connect(MenuDataSearchText.reject)
        self.pushButton_help.clicked.connect(MenuDataSearchText.help)

        QMetaObject.connectSlotsByName(MenuDataSearchText)
    # setupUi

    def retranslateUi(self, MenuDataSearchText):
        MenuDataSearchText.setWindowTitle(QCoreApplication.translate("MenuDataSearchText", u"SEARCH TEXT IN WORKSPACE OBJECTS", None))
        self.label_text.setText(QCoreApplication.translate("MenuDataSearchText", u"Text", None))
        self.pushButton_search.setText(QCoreApplication.translate("MenuDataSearchText", u"Search", None))
#if QT_CONFIG(shortcut)
        self.pushButton_search.setShortcut(QCoreApplication.translate("MenuDataSearchText", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuDataSearchText", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuDataSearchText", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuDataSearchText", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuDataSearchText", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.checkBox_whole_word.setText(QCoreApplication.translate("MenuDataSearchText", u"Whole Word", None))
        self.checkBox_exact_case.setText(QCoreApplication.translate("MenuDataSearchText", u"Exact Case", None))
        self.label_objs_to_scan.setText(QCoreApplication.translate("MenuDataSearchText", u"Objects to scan", None))
        self.checkBox_search_formulas.setText(QCoreApplication.translate("MenuDataSearchText", u"Formulas", None))
        self.checkBox_search_free_text.setText(QCoreApplication.translate("MenuDataSearchText", u"Free Text", None))
        self.label_save_list.setText(QCoreApplication.translate("MenuDataSearchText", u"Save result in list ", None))
        self.label_result.setText(QCoreApplication.translate("MenuDataSearchText", u"Result", None))
        self.checkBox_search_names.setText(QCoreApplication.translate("MenuDataSearchText", u"Search in Names", None))
    # retranslateUi

