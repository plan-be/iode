# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'workspace_low_to_high.ui'
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
    QLabel, QPushButton, QSizePolicy, QTextEdit,
    QVBoxLayout, QWidget)

from util.widgets.file_chooser import IodeFileChooser

class Ui_MenuWorkspaceLowToHigh(object):
    def setupUi(self, MenuWorkspaceLowToHigh):
        if not MenuWorkspaceLowToHigh.objectName():
            MenuWorkspaceLowToHigh.setObjectName(u"MenuWorkspaceLowToHigh")
        MenuWorkspaceLowToHigh.resize(600, 320)
        MenuWorkspaceLowToHigh.setMinimumSize(QSize(600, 320))
        MenuWorkspaceLowToHigh.setMaximumSize(QSize(600, 320))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuWorkspaceLowToHigh.setWindowIcon(icon)
        self.layoutWidget = QWidget(MenuWorkspaceLowToHigh)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(20, 20, 461, 281))
        self.gridLayout = QGridLayout(self.layoutWidget)
        self.gridLayout.setObjectName(u"gridLayout")
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.label_input_file = QLabel(self.layoutWidget)
        self.label_input_file.setObjectName(u"label_input_file")

        self.gridLayout.addWidget(self.label_input_file, 0, 0, 1, 1)

        self.fileChooser_input_file = IodeFileChooser(self.layoutWidget)
        self.fileChooser_input_file.setObjectName(u"fileChooser_input_file")

        self.gridLayout.addWidget(self.fileChooser_input_file, 0, 1, 1, 2)

        self.label_method = QLabel(self.layoutWidget)
        self.label_method.setObjectName(u"label_method")
        self.label_method.setAlignment(Qt.AlignLeading|Qt.AlignLeft|Qt.AlignVCenter)

        self.gridLayout.addWidget(self.label_method, 1, 0, 1, 1)

        self.comboBox_method = QComboBox(self.layoutWidget)
        self.comboBox_method.setObjectName(u"comboBox_method")

        self.gridLayout.addWidget(self.comboBox_method, 1, 1, 1, 1)

        self.label = QLabel(self.layoutWidget)
        self.label.setObjectName(u"label")

        self.gridLayout.addWidget(self.label, 1, 2, 1, 1)

        self.label_stock_series = QLabel(self.layoutWidget)
        self.label_stock_series.setObjectName(u"label_stock_series")

        self.gridLayout.addWidget(self.label_stock_series, 2, 0, 1, 1)

        self.textEdit_stock_series = QTextEdit(self.layoutWidget)
        self.textEdit_stock_series.setObjectName(u"textEdit_stock_series")

        self.gridLayout.addWidget(self.textEdit_stock_series, 2, 1, 1, 2)

        self.label_flow_series = QLabel(self.layoutWidget)
        self.label_flow_series.setObjectName(u"label_flow_series")

        self.gridLayout.addWidget(self.label_flow_series, 3, 0, 1, 1)

        self.textEdit_flow_series = QTextEdit(self.layoutWidget)
        self.textEdit_flow_series.setObjectName(u"textEdit_flow_series")

        self.gridLayout.addWidget(self.textEdit_flow_series, 3, 1, 1, 2)

        self.verticalLayoutWidget = QWidget(MenuWorkspaceLowToHigh)
        self.verticalLayoutWidget.setObjectName(u"verticalLayoutWidget")
        self.verticalLayoutWidget.setGeometry(QRect(500, 20, 91, 91))
        self.verticalLayout = QVBoxLayout(self.verticalLayoutWidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.pushButton_ltoh = QPushButton(self.verticalLayoutWidget)
        self.pushButton_ltoh.setObjectName(u"pushButton_ltoh")

        self.verticalLayout.addWidget(self.pushButton_ltoh)

        self.pushButton_cancel = QPushButton(self.verticalLayoutWidget)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.verticalLayout.addWidget(self.pushButton_cancel)

        self.pushButton_help = QPushButton(self.verticalLayoutWidget)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.retranslateUi(MenuWorkspaceLowToHigh)
        self.pushButton_help.clicked.connect(MenuWorkspaceLowToHigh.help)
        self.pushButton_cancel.clicked.connect(MenuWorkspaceLowToHigh.reject)
        self.pushButton_ltoh.clicked.connect(MenuWorkspaceLowToHigh.slot_low_to_high)

        QMetaObject.connectSlotsByName(MenuWorkspaceLowToHigh)
    # setupUi

    def retranslateUi(self, MenuWorkspaceLowToHigh):
        MenuWorkspaceLowToHigh.setWindowTitle(QCoreApplication.translate("MenuWorkspaceLowToHigh", u"LOW TO HIGH", None))
        self.label_input_file.setText(QCoreApplication.translate("MenuWorkspaceLowToHigh", u"Input file", None))
        self.label_method.setText(QCoreApplication.translate("MenuWorkspaceLowToHigh", u"Method", None))
        self.label.setText("")
        self.label_stock_series.setText(QCoreApplication.translate("MenuWorkspaceLowToHigh", u"Stock Series", None))
        self.label_flow_series.setText(QCoreApplication.translate("MenuWorkspaceLowToHigh", u"Flow Series", None))
        self.pushButton_ltoh.setText(QCoreApplication.translate("MenuWorkspaceLowToHigh", u"OK", None))
#if QT_CONFIG(shortcut)
        self.pushButton_ltoh.setShortcut(QCoreApplication.translate("MenuWorkspaceLowToHigh", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuWorkspaceLowToHigh", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuWorkspaceLowToHigh", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuWorkspaceLowToHigh", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuWorkspaceLowToHigh", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

