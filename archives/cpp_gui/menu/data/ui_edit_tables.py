# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'edit_tables.ui'
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
from PySide6.QtWidgets import (QApplication, QDialog, QGridLayout, QLabel,
    QPushButton, QSizePolicy, QSpacerItem, QWidget)

from text_edit.complete_text_edit import IodeAutoCompleteTextEdit

class Ui_MenuDataEditTables(object):
    def setupUi(self, MenuDataEditTables):
        if not MenuDataEditTables.objectName():
            MenuDataEditTables.setObjectName(u"MenuDataEditTables")
        MenuDataEditTables.resize(603, 165)
        self.gridLayout = QGridLayout(MenuDataEditTables)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_table_names = QLabel(MenuDataEditTables)
        self.label_table_names.setObjectName(u"label_table_names")
        self.label_table_names.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_table_names, 0, 0, 4, 1)

        self.textEdit_table_names = IodeAutoCompleteTextEdit(MenuDataEditTables)
        self.textEdit_table_names.setObjectName(u"textEdit_table_names")

        self.gridLayout.addWidget(self.textEdit_table_names, 0, 1, 4, 1)

        self.pushButton_display = QPushButton(MenuDataEditTables)
        self.pushButton_display.setObjectName(u"pushButton_display")

        self.gridLayout.addWidget(self.pushButton_display, 0, 2, 1, 1)

        self.pushButton_cancel = QPushButton(MenuDataEditTables)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.gridLayout.addWidget(self.pushButton_cancel, 1, 2, 1, 1)

        self.pushButton_help = QPushButton(MenuDataEditTables)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.gridLayout.addWidget(self.pushButton_help, 2, 2, 1, 1)

        self.verticalSpacer = QSpacerItem(20, 54, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 3, 2, 1, 1)


        self.retranslateUi(MenuDataEditTables)
        self.pushButton_display.clicked.connect(MenuDataEditTables.display)
        self.pushButton_cancel.clicked.connect(MenuDataEditTables.reject)
        self.pushButton_help.clicked.connect(MenuDataEditTables.help)

        QMetaObject.connectSlotsByName(MenuDataEditTables)
    # setupUi

    def retranslateUi(self, MenuDataEditTables):
        MenuDataEditTables.setWindowTitle(QCoreApplication.translate("MenuDataEditTables", u"DISPLAY AND UPDATE VARIABLES BY TABLES", None))
        self.label_table_names.setText(QCoreApplication.translate("MenuDataEditTables", u"Table Names", None))
        self.pushButton_display.setText(QCoreApplication.translate("MenuDataEditTables", u"Display", None))
#if QT_CONFIG(shortcut)
        self.pushButton_display.setShortcut(QCoreApplication.translate("MenuDataEditTables", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuDataEditTables", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuDataEditTables", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuDataEditTables", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuDataEditTables", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

