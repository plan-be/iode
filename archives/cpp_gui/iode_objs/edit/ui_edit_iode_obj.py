# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'edit_iode_obj.ui'
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
    QLineEdit, QPushButton, QSizePolicy, QSpacerItem,
    QVBoxLayout, QWidget)

from text_edit.complete_text_edit import IodeAutoCompleteTextEdit

class Ui_EditObjectDialog(object):
    def setupUi(self, EditObjectDialog):
        if not EditObjectDialog.objectName():
            EditObjectDialog.setObjectName(u"EditObjectDialog")
        EditObjectDialog.resize(504, 307)
        self.gridLayout = QGridLayout(EditObjectDialog)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_name = QLabel(EditObjectDialog)
        self.label_name.setObjectName(u"label_name")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_name.sizePolicy().hasHeightForWidth())
        self.label_name.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.label_name, 0, 0, 1, 1)

        self.lineEdit_name = QLineEdit(EditObjectDialog)
        self.lineEdit_name.setObjectName(u"lineEdit_name")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.lineEdit_name.sizePolicy().hasHeightForWidth())
        self.lineEdit_name.setSizePolicy(sizePolicy1)
        self.lineEdit_name.setReadOnly(False)

        self.gridLayout.addWidget(self.lineEdit_name, 0, 1, 1, 1)

        self.horizontalSpacer = QSpacerItem(224, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 0, 2, 1, 1)

        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.pushButton_save = QPushButton(EditObjectDialog)
        self.pushButton_save.setObjectName(u"pushButton_save")

        self.verticalLayout.addWidget(self.pushButton_save)

        self.pushButton_quit = QPushButton(EditObjectDialog)
        self.pushButton_quit.setObjectName(u"pushButton_quit")

        self.verticalLayout.addWidget(self.pushButton_quit)

        self.pushButton_help = QPushButton(EditObjectDialog)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.gridLayout.addLayout(self.verticalLayout, 0, 3, 2, 1)

        self.textEdit_content = IodeAutoCompleteTextEdit(EditObjectDialog)
        self.textEdit_content.setObjectName(u"textEdit_content")

        self.gridLayout.addWidget(self.textEdit_content, 1, 0, 2, 3)

        self.verticalSpacer = QSpacerItem(20, 194, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 2, 3, 1, 1)


        self.retranslateUi(EditObjectDialog)
        self.pushButton_save.clicked.connect(EditObjectDialog.edit)
        self.pushButton_quit.clicked.connect(EditObjectDialog.reject)
        self.pushButton_help.clicked.connect(EditObjectDialog.help)

        QMetaObject.connectSlotsByName(EditObjectDialog)
    # setupUi

    def retranslateUi(self, EditObjectDialog):
        EditObjectDialog.setWindowTitle(QCoreApplication.translate("EditObjectDialog", u"Edit IODE object", None))
        self.label_name.setText(QCoreApplication.translate("EditObjectDialog", u"Name", None))
        self.pushButton_save.setText(QCoreApplication.translate("EditObjectDialog", u"Save", None))
#if QT_CONFIG(shortcut)
        self.pushButton_save.setShortcut(QCoreApplication.translate("EditObjectDialog", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_quit.setText(QCoreApplication.translate("EditObjectDialog", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_quit.setShortcut(QCoreApplication.translate("EditObjectDialog", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("EditObjectDialog", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("EditObjectDialog", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

