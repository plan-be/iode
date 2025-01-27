# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'scan_objects.ui'
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
    QGridLayout, QLabel, QPushButton, QSizePolicy,
    QSpacerItem, QTextEdit, QVBoxLayout, QWidget)

from text_edit.complete_text_edit import IodeAutoCompleteTextEdit

class Ui_MenuDataScanObjects(object):
    def setupUi(self, MenuDataScanObjects):
        if not MenuDataScanObjects.objectName():
            MenuDataScanObjects.setObjectName(u"MenuDataScanObjects")
        MenuDataScanObjects.resize(505, 218)
        self.gridLayout = QGridLayout(MenuDataScanObjects)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_objs_to_scan = QLabel(MenuDataScanObjects)
        self.label_objs_to_scan.setObjectName(u"label_objs_to_scan")

        self.gridLayout.addWidget(self.label_objs_to_scan, 0, 0, 1, 1)

        self.comboBox_iode_types = QComboBox(MenuDataScanObjects)
        self.comboBox_iode_types.setObjectName(u"comboBox_iode_types")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.comboBox_iode_types.sizePolicy().hasHeightForWidth())
        self.comboBox_iode_types.setSizePolicy(sizePolicy)
        self.comboBox_iode_types.setMinimumSize(QSize(180, 20))
        self.comboBox_iode_types.setMaximumSize(QSize(180, 20))

        self.gridLayout.addWidget(self.comboBox_iode_types, 0, 1, 1, 1)

        self.horizontalSpacer = QSpacerItem(127, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 0, 2, 1, 1)

        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.pushButton_scan = QPushButton(MenuDataScanObjects)
        self.pushButton_scan.setObjectName(u"pushButton_scan")

        self.verticalLayout.addWidget(self.pushButton_scan)

        self.pushButton_cancel = QPushButton(MenuDataScanObjects)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.verticalLayout.addWidget(self.pushButton_cancel)

        self.pushButton_help = QPushButton(MenuDataScanObjects)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.gridLayout.addLayout(self.verticalLayout, 0, 3, 2, 1)

        self.label_lists_to_scan = QLabel(MenuDataScanObjects)
        self.label_lists_to_scan.setObjectName(u"label_lists_to_scan")

        self.gridLayout.addWidget(self.label_lists_to_scan, 1, 0, 1, 1)

        self.textEdit_lists_to_scan = IodeAutoCompleteTextEdit(MenuDataScanObjects)
        self.textEdit_lists_to_scan.setObjectName(u"textEdit_lists_to_scan")

        self.gridLayout.addWidget(self.textEdit_lists_to_scan, 1, 1, 2, 2)

        self.verticalSpacer = QSpacerItem(20, 105, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 2, 3, 2, 1)

        self.textEdit_note = QTextEdit(MenuDataScanObjects)
        self.textEdit_note.setObjectName(u"textEdit_note")
        self.textEdit_note.setFrameShadow(QFrame.Sunken)
        self.textEdit_note.setReadOnly(True)

        self.gridLayout.addWidget(self.textEdit_note, 3, 0, 1, 3)


        self.retranslateUi(MenuDataScanObjects)
        self.pushButton_scan.clicked.connect(MenuDataScanObjects.scan)
        self.pushButton_cancel.clicked.connect(MenuDataScanObjects.reject)
        self.pushButton_help.clicked.connect(MenuDataScanObjects.help)

        QMetaObject.connectSlotsByName(MenuDataScanObjects)
    # setupUi

    def retranslateUi(self, MenuDataScanObjects):
        MenuDataScanObjects.setWindowTitle(QCoreApplication.translate("MenuDataScanObjects", u"SCAN OBJECTS FOR VARIABLES AND SCALARS", None))
        self.label_objs_to_scan.setText(QCoreApplication.translate("MenuDataScanObjects", u"Objects to scan", None))
        self.pushButton_scan.setText(QCoreApplication.translate("MenuDataScanObjects", u"Scan", None))
#if QT_CONFIG(shortcut)
        self.pushButton_scan.setShortcut(QCoreApplication.translate("MenuDataScanObjects", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuDataScanObjects", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuDataScanObjects", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuDataScanObjects", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuDataScanObjects", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label_lists_to_scan.setText(QCoreApplication.translate("MenuDataScanObjects", u"List(s) to Scan", None))
        self.textEdit_note.setHtml(QCoreApplication.translate("MenuDataScanObjects", u"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Segoe UI'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">Note</span>: The results are saved in the lists (and sublists) named:</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">-&gt; <spa"
                        "n style=\" font-weight:700;\">_EXO</span> for the Variables</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">-&gt; <span style=\" font-weight:700;\">_SCAL</span> for the Scalars</p></body></html>", None))
    # retranslateUi

