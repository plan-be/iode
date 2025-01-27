# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'workspace_copy_into.ui'
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
from util.widgets.sample_edit import IodeSampleEdit

class Ui_MenuWorkspaceCopyInto(object):
    def setupUi(self, MenuWorkspaceCopyInto):
        if not MenuWorkspaceCopyInto.objectName():
            MenuWorkspaceCopyInto.setObjectName(u"MenuWorkspaceCopyInto")
        MenuWorkspaceCopyInto.resize(600, 340)
        MenuWorkspaceCopyInto.setMinimumSize(QSize(600, 340))
        MenuWorkspaceCopyInto.setMaximumSize(QSize(600, 340))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuWorkspaceCopyInto.setWindowIcon(icon)
        self.layoutWidget = QWidget(MenuWorkspaceCopyInto)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(22, 22, 461, 291))
        self.gridLayout = QGridLayout(self.layoutWidget)
        self.gridLayout.setObjectName(u"gridLayout")
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.label_object_type = QLabel(self.layoutWidget)
        self.label_object_type.setObjectName(u"label_object_type")

        self.gridLayout.addWidget(self.label_object_type, 0, 0, 1, 1)

        self.comboBox_object_type = QComboBox(self.layoutWidget)
        self.comboBox_object_type.setObjectName(u"comboBox_object_type")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.comboBox_object_type.sizePolicy().hasHeightForWidth())
        self.comboBox_object_type.setSizePolicy(sizePolicy)
        self.comboBox_object_type.setMinimumSize(QSize(120, 0))
        self.comboBox_object_type.setMaxCount(10)

        self.gridLayout.addWidget(self.comboBox_object_type, 0, 1, 1, 2)

        self.label_input_file = QLabel(self.layoutWidget)
        self.label_input_file.setObjectName(u"label_input_file")

        self.gridLayout.addWidget(self.label_input_file, 1, 0, 1, 1)

        self.fileChooser_input_file = IodeFileChooser(self.layoutWidget)
        self.fileChooser_input_file.setObjectName(u"fileChooser_input_file")

        self.gridLayout.addWidget(self.fileChooser_input_file, 1, 1, 1, 4)

        self.label_sample_from = QLabel(self.layoutWidget)
        self.label_sample_from.setObjectName(u"label_sample_from")

        self.gridLayout.addWidget(self.label_sample_from, 2, 0, 1, 1)

        self.sampleEdit_sample_from = IodeSampleEdit(self.layoutWidget)
        self.sampleEdit_sample_from.setObjectName(u"sampleEdit_sample_from")

        self.gridLayout.addWidget(self.sampleEdit_sample_from, 2, 1, 1, 1)

        self.label_sample_to = QLabel(self.layoutWidget)
        self.label_sample_to.setObjectName(u"label_sample_to")

        self.gridLayout.addWidget(self.label_sample_to, 2, 2, 1, 1)

        self.sampleEdit_sample_to = IodeSampleEdit(self.layoutWidget)
        self.sampleEdit_sample_to.setObjectName(u"sampleEdit_sample_to")

        self.gridLayout.addWidget(self.sampleEdit_sample_to, 2, 3, 1, 1)

        self.label = QLabel(self.layoutWidget)
        self.label.setObjectName(u"label")

        self.gridLayout.addWidget(self.label, 2, 4, 1, 1)

        self.label_object_names = QLabel(self.layoutWidget)
        self.label_object_names.setObjectName(u"label_object_names")

        self.gridLayout.addWidget(self.label_object_names, 3, 0, 1, 1)

        self.textEdit_object_names = QTextEdit(self.layoutWidget)
        self.textEdit_object_names.setObjectName(u"textEdit_object_names")

        self.gridLayout.addWidget(self.textEdit_object_names, 3, 1, 1, 4)

        self.verticalLayoutWidget = QWidget(MenuWorkspaceCopyInto)
        self.verticalLayoutWidget.setObjectName(u"verticalLayoutWidget")
        self.verticalLayoutWidget.setGeometry(QRect(500, 20, 91, 91))
        self.verticalLayout = QVBoxLayout(self.verticalLayoutWidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.pushButton_copy = QPushButton(self.verticalLayoutWidget)
        self.pushButton_copy.setObjectName(u"pushButton_copy")

        self.verticalLayout.addWidget(self.pushButton_copy)

        self.pushButton_cancel = QPushButton(self.verticalLayoutWidget)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.verticalLayout.addWidget(self.pushButton_cancel)

        self.pushButton_help = QPushButton(self.verticalLayoutWidget)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.retranslateUi(MenuWorkspaceCopyInto)
        self.pushButton_copy.clicked.connect(MenuWorkspaceCopyInto.copy_into_workspace)
        self.pushButton_cancel.clicked.connect(MenuWorkspaceCopyInto.reject)
        self.pushButton_help.clicked.connect(MenuWorkspaceCopyInto.help)

        QMetaObject.connectSlotsByName(MenuWorkspaceCopyInto)
    # setupUi

    def retranslateUi(self, MenuWorkspaceCopyInto):
        MenuWorkspaceCopyInto.setWindowTitle(QCoreApplication.translate("MenuWorkspaceCopyInto", u"COPY OBJECTS INTO WORKSPACE", None))
        self.label_object_type.setText(QCoreApplication.translate("MenuWorkspaceCopyInto", u"Object type", None))
        self.label_input_file.setText(QCoreApplication.translate("MenuWorkspaceCopyInto", u"Input file", None))
        self.label_sample_from.setText(QCoreApplication.translate("MenuWorkspaceCopyInto", u"Sample From", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setToolTip(QCoreApplication.translate("MenuWorkspaceCopyInto", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_from.setInputMask("")
        self.sampleEdit_sample_from.setPlaceholderText("")
        self.label_sample_to.setText(QCoreApplication.translate("MenuWorkspaceCopyInto", u"To", None))
#if QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setToolTip(QCoreApplication.translate("MenuWorkspaceCopyInto", u"- yyyy is the year\n"
" -P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
" -ppp is the period of the year", None))
#endif // QT_CONFIG(tooltip)
        self.sampleEdit_sample_to.setInputMask("")
        self.sampleEdit_sample_to.setPlaceholderText("")
        self.label.setText("")
        self.label_object_names.setText(QCoreApplication.translate("MenuWorkspaceCopyInto", u"Object names", None))
        self.pushButton_copy.setText(QCoreApplication.translate("MenuWorkspaceCopyInto", u"Copy", None))
#if QT_CONFIG(shortcut)
        self.pushButton_copy.setShortcut(QCoreApplication.translate("MenuWorkspaceCopyInto", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuWorkspaceCopyInto", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuWorkspaceCopyInto", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuWorkspaceCopyInto", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuWorkspaceCopyInto", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

