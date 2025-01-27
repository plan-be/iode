# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'workspace_merge_into.ui'
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
    QHBoxLayout, QLabel, QPushButton, QSizePolicy,
    QSpacerItem, QWidget)

from util.widgets.file_chooser import IodeFileChooser

class Ui_MenuWorkspaceMergeInto(object):
    def setupUi(self, MenuWorkspaceMergeInto):
        if not MenuWorkspaceMergeInto.objectName():
            MenuWorkspaceMergeInto.setObjectName(u"MenuWorkspaceMergeInto")
        MenuWorkspaceMergeInto.resize(480, 100)
        MenuWorkspaceMergeInto.setMinimumSize(QSize(480, 100))
        MenuWorkspaceMergeInto.setMaximumSize(QSize(600, 140))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuWorkspaceMergeInto.setWindowIcon(icon)
        self.layoutWidget = QWidget(MenuWorkspaceMergeInto)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(10, 10, 461, 81))
        self.gridLayout = QGridLayout(self.layoutWidget)
        self.gridLayout.setObjectName(u"gridLayout")
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.label_object_type = QLabel(self.layoutWidget)
        self.label_object_type.setObjectName(u"label_object_type")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_object_type.sizePolicy().hasHeightForWidth())
        self.label_object_type.setSizePolicy(sizePolicy)
        self.label_object_type.setMinimumSize(QSize(70, 0))

        self.gridLayout.addWidget(self.label_object_type, 0, 0, 1, 1)

        self.comboBox_object_type = QComboBox(self.layoutWidget)
        self.comboBox_object_type.setObjectName(u"comboBox_object_type")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.comboBox_object_type.sizePolicy().hasHeightForWidth())
        self.comboBox_object_type.setSizePolicy(sizePolicy1)
        self.comboBox_object_type.setMinimumSize(QSize(120, 0))
        self.comboBox_object_type.setMaxCount(10)

        self.gridLayout.addWidget(self.comboBox_object_type, 0, 1, 1, 2)

        self.horizontalSpacer_3 = QSpacerItem(248, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_3, 0, 3, 1, 2)

        self.label_input_file = QLabel(self.layoutWidget)
        self.label_input_file.setObjectName(u"label_input_file")
        sizePolicy.setHeightForWidth(self.label_input_file.sizePolicy().hasHeightForWidth())
        self.label_input_file.setSizePolicy(sizePolicy)
        self.label_input_file.setMinimumSize(QSize(70, 0))

        self.gridLayout.addWidget(self.label_input_file, 1, 0, 1, 1)

        self.fileChooser_input_file = IodeFileChooser(self.layoutWidget)
        self.fileChooser_input_file.setObjectName(u"fileChooser_input_file")

        self.gridLayout.addWidget(self.fileChooser_input_file, 1, 1, 1, 4)

        self.horizontalSpacer = QSpacerItem(88, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 2, 0, 1, 2)

        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.pushButton_merge = QPushButton(self.layoutWidget)
        self.pushButton_merge.setObjectName(u"pushButton_merge")

        self.horizontalLayout.addWidget(self.pushButton_merge)

        self.pushButton_cancel = QPushButton(self.layoutWidget)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.horizontalLayout.addWidget(self.pushButton_cancel)

        self.pushButton_help = QPushButton(self.layoutWidget)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.horizontalLayout.addWidget(self.pushButton_help)


        self.gridLayout.addLayout(self.horizontalLayout, 2, 2, 1, 2)

        self.horizontalSpacer_2 = QSpacerItem(78, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_2, 2, 4, 1, 1)


        self.retranslateUi(MenuWorkspaceMergeInto)
        self.pushButton_merge.clicked.connect(MenuWorkspaceMergeInto.merge_into_workspace)
        self.pushButton_cancel.clicked.connect(MenuWorkspaceMergeInto.reject)
        self.pushButton_help.clicked.connect(MenuWorkspaceMergeInto.help)

        QMetaObject.connectSlotsByName(MenuWorkspaceMergeInto)
    # setupUi

    def retranslateUi(self, MenuWorkspaceMergeInto):
        MenuWorkspaceMergeInto.setWindowTitle(QCoreApplication.translate("MenuWorkspaceMergeInto", u"MERGE FILE INTO WORKSPACE", None))
        self.label_object_type.setText(QCoreApplication.translate("MenuWorkspaceMergeInto", u"Object type", None))
        self.label_input_file.setText(QCoreApplication.translate("MenuWorkspaceMergeInto", u"Input file", None))
        self.pushButton_merge.setText(QCoreApplication.translate("MenuWorkspaceMergeInto", u"Merge", None))
#if QT_CONFIG(shortcut)
        self.pushButton_merge.setShortcut(QCoreApplication.translate("MenuWorkspaceMergeInto", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuWorkspaceMergeInto", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuWorkspaceMergeInto", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuWorkspaceMergeInto", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuWorkspaceMergeInto", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

