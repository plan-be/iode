# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'print_file_dialog.ui'
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
    QLabel, QPushButton, QSizePolicy, QSpacerItem,
    QWidget)

from util.widgets.file_chooser import IodeFileChooser

class Ui_PrintFileDialog(object):
    def setupUi(self, PrintFileDialog):
        if not PrintFileDialog.objectName():
            PrintFileDialog.setObjectName(u"PrintFileDialog")
        PrintFileDialog.resize(446, 125)
        self.gridLayout = QGridLayout(PrintFileDialog)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_print_format = QLabel(PrintFileDialog)
        self.label_print_format.setObjectName(u"label_print_format")

        self.gridLayout.addWidget(self.label_print_format, 0, 0, 1, 1)

        self.comboBox_print_format = QComboBox(PrintFileDialog)
        self.comboBox_print_format.setObjectName(u"comboBox_print_format")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.comboBox_print_format.sizePolicy().hasHeightForWidth())
        self.comboBox_print_format.setSizePolicy(sizePolicy)
        self.comboBox_print_format.setMinimumSize(QSize(220, 0))
        self.comboBox_print_format.setMaximumSize(QSize(220, 16777215))

        self.gridLayout.addWidget(self.comboBox_print_format, 0, 1, 1, 3)

        self.horizontalSpacer = QSpacerItem(123, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 0, 4, 1, 2)

        self.label_output_file = QLabel(PrintFileDialog)
        self.label_output_file.setObjectName(u"label_output_file")

        self.gridLayout.addWidget(self.label_output_file, 1, 0, 1, 1)

        self.fileChooser_output_file = IodeFileChooser(PrintFileDialog)
        self.fileChooser_output_file.setObjectName(u"fileChooser_output_file")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.fileChooser_output_file.sizePolicy().hasHeightForWidth())
        self.fileChooser_output_file.setSizePolicy(sizePolicy1)
        self.fileChooser_output_file.setMinimumSize(QSize(0, 20))
        self.fileChooser_output_file.setMaximumSize(QSize(16777215, 20))

        self.gridLayout.addWidget(self.fileChooser_output_file, 1, 1, 1, 5)

        self.horizontalSpacer_2 = QSpacerItem(35, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_2, 2, 0, 1, 1)

        self.pushButton_apply = QPushButton(PrintFileDialog)
        self.pushButton_apply.setObjectName(u"pushButton_apply")
        sizePolicy.setHeightForWidth(self.pushButton_apply.sizePolicy().hasHeightForWidth())
        self.pushButton_apply.setSizePolicy(sizePolicy)
        self.pushButton_apply.setMinimumSize(QSize(80, 0))
        self.pushButton_apply.setMaximumSize(QSize(80, 16777215))

        self.gridLayout.addWidget(self.pushButton_apply, 2, 1, 1, 1)

        self.pushButton_cancel = QPushButton(PrintFileDialog)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")
        sizePolicy.setHeightForWidth(self.pushButton_cancel.sizePolicy().hasHeightForWidth())
        self.pushButton_cancel.setSizePolicy(sizePolicy)
        self.pushButton_cancel.setMinimumSize(QSize(80, 0))
        self.pushButton_cancel.setMaximumSize(QSize(80, 16777215))

        self.gridLayout.addWidget(self.pushButton_cancel, 2, 2, 1, 1)

        self.pushButton_options = QPushButton(PrintFileDialog)
        self.pushButton_options.setObjectName(u"pushButton_options")
        sizePolicy.setHeightForWidth(self.pushButton_options.sizePolicy().hasHeightForWidth())
        self.pushButton_options.setSizePolicy(sizePolicy)
        self.pushButton_options.setMinimumSize(QSize(80, 0))
        self.pushButton_options.setMaximumSize(QSize(80, 16777215))

        self.gridLayout.addWidget(self.pushButton_options, 2, 3, 1, 1)

        self.pushButton_help = QPushButton(PrintFileDialog)
        self.pushButton_help.setObjectName(u"pushButton_help")
        sizePolicy.setHeightForWidth(self.pushButton_help.sizePolicy().hasHeightForWidth())
        self.pushButton_help.setSizePolicy(sizePolicy)
        self.pushButton_help.setMinimumSize(QSize(80, 0))
        self.pushButton_help.setMaximumSize(QSize(80, 16777215))

        self.gridLayout.addWidget(self.pushButton_help, 2, 4, 1, 1)

        self.horizontalSpacer_3 = QSpacerItem(37, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_3, 2, 5, 1, 1)


        self.retranslateUi(PrintFileDialog)
        self.pushButton_apply.clicked.connect(PrintFileDialog.apply)
        self.pushButton_cancel.clicked.connect(PrintFileDialog.reject)
        self.pushButton_options.clicked.connect(PrintFileDialog.set_print_options)
        self.pushButton_help.clicked.connect(PrintFileDialog.help)

        QMetaObject.connectSlotsByName(PrintFileDialog)
    # setupUi

    def retranslateUi(self, PrintFileDialog):
        PrintFileDialog.setWindowTitle(QCoreApplication.translate("PrintFileDialog", u"Dialog", None))
        self.label_print_format.setText(QCoreApplication.translate("PrintFileDialog", u"Format", None))
        self.label_output_file.setText(QCoreApplication.translate("PrintFileDialog", u"File", None))
        self.pushButton_apply.setText(QCoreApplication.translate("PrintFileDialog", u"Apply", None))
#if QT_CONFIG(shortcut)
        self.pushButton_apply.setShortcut(QCoreApplication.translate("PrintFileDialog", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("PrintFileDialog", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("PrintFileDialog", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_options.setText(QCoreApplication.translate("PrintFileDialog", u"Options", None))
#if QT_CONFIG(shortcut)
        self.pushButton_options.setShortcut(QCoreApplication.translate("PrintFileDialog", u"F8", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("PrintFileDialog", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("PrintFileDialog", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

