# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'workspace_describe.ui'
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
from PySide6.QtWidgets import (QApplication, QDialog, QFormLayout, QLabel,
    QLineEdit, QPushButton, QSizePolicy, QVBoxLayout,
    QWidget)

class Ui_MenuWorkspaceDescribe(object):
    def setupUi(self, MenuWorkspaceDescribe):
        if not MenuWorkspaceDescribe.objectName():
            MenuWorkspaceDescribe.setObjectName(u"MenuWorkspaceDescribe")
        MenuWorkspaceDescribe.resize(600, 250)
        MenuWorkspaceDescribe.setMinimumSize(QSize(600, 250))
        MenuWorkspaceDescribe.setMaximumSize(QSize(600, 250))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuWorkspaceDescribe.setWindowIcon(icon)
        self.layoutWidget = QWidget(MenuWorkspaceDescribe)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(20, 20, 461, 211))
        self.formLayout = QFormLayout(self.layoutWidget)
        self.formLayout.setObjectName(u"formLayout")
        self.formLayout.setContentsMargins(0, 0, 0, 0)
        self.label_comments = QLabel(self.layoutWidget)
        self.label_comments.setObjectName(u"label_comments")

        self.formLayout.setWidget(0, QFormLayout.LabelRole, self.label_comments)

        self.lineEdit_comments = QLineEdit(self.layoutWidget)
        self.lineEdit_comments.setObjectName(u"lineEdit_comments")

        self.formLayout.setWidget(0, QFormLayout.FieldRole, self.lineEdit_comments)

        self.label_equations = QLabel(self.layoutWidget)
        self.label_equations.setObjectName(u"label_equations")

        self.formLayout.setWidget(1, QFormLayout.LabelRole, self.label_equations)

        self.lineEdit_equations = QLineEdit(self.layoutWidget)
        self.lineEdit_equations.setObjectName(u"lineEdit_equations")

        self.formLayout.setWidget(1, QFormLayout.FieldRole, self.lineEdit_equations)

        self.label_identities = QLabel(self.layoutWidget)
        self.label_identities.setObjectName(u"label_identities")

        self.formLayout.setWidget(2, QFormLayout.LabelRole, self.label_identities)

        self.lineEdit_identities = QLineEdit(self.layoutWidget)
        self.lineEdit_identities.setObjectName(u"lineEdit_identities")

        self.formLayout.setWidget(2, QFormLayout.FieldRole, self.lineEdit_identities)

        self.label_lists = QLabel(self.layoutWidget)
        self.label_lists.setObjectName(u"label_lists")

        self.formLayout.setWidget(3, QFormLayout.LabelRole, self.label_lists)

        self.lineEdit_lists = QLineEdit(self.layoutWidget)
        self.lineEdit_lists.setObjectName(u"lineEdit_lists")

        self.formLayout.setWidget(3, QFormLayout.FieldRole, self.lineEdit_lists)

        self.label_scalars = QLabel(self.layoutWidget)
        self.label_scalars.setObjectName(u"label_scalars")

        self.formLayout.setWidget(4, QFormLayout.LabelRole, self.label_scalars)

        self.lineEdit_scalars = QLineEdit(self.layoutWidget)
        self.lineEdit_scalars.setObjectName(u"lineEdit_scalars")

        self.formLayout.setWidget(4, QFormLayout.FieldRole, self.lineEdit_scalars)

        self.label_tables = QLabel(self.layoutWidget)
        self.label_tables.setObjectName(u"label_tables")

        self.formLayout.setWidget(5, QFormLayout.LabelRole, self.label_tables)

        self.lineEdit_tables = QLineEdit(self.layoutWidget)
        self.lineEdit_tables.setObjectName(u"lineEdit_tables")

        self.formLayout.setWidget(5, QFormLayout.FieldRole, self.lineEdit_tables)

        self.label_variables = QLabel(self.layoutWidget)
        self.label_variables.setObjectName(u"label_variables")

        self.formLayout.setWidget(6, QFormLayout.LabelRole, self.label_variables)

        self.lineEdit_variables = QLineEdit(self.layoutWidget)
        self.lineEdit_variables.setObjectName(u"lineEdit_variables")

        self.formLayout.setWidget(6, QFormLayout.FieldRole, self.lineEdit_variables)

        self.verticalLayoutWidget = QWidget(MenuWorkspaceDescribe)
        self.verticalLayoutWidget.setObjectName(u"verticalLayoutWidget")
        self.verticalLayoutWidget.setGeometry(QRect(490, 20, 91, 91))
        self.verticalLayout = QVBoxLayout(self.verticalLayoutWidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.pushButton_describe = QPushButton(self.verticalLayoutWidget)
        self.pushButton_describe.setObjectName(u"pushButton_describe")

        self.verticalLayout.addWidget(self.pushButton_describe)

        self.pushButton_cancel = QPushButton(self.verticalLayoutWidget)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.verticalLayout.addWidget(self.pushButton_cancel)

        self.pushButton_help = QPushButton(self.verticalLayoutWidget)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.retranslateUi(MenuWorkspaceDescribe)
        self.pushButton_describe.clicked.connect(MenuWorkspaceDescribe.update_descriptions)
        self.pushButton_cancel.clicked.connect(MenuWorkspaceDescribe.reject)
        self.pushButton_help.clicked.connect(MenuWorkspaceDescribe.help)

        QMetaObject.connectSlotsByName(MenuWorkspaceDescribe)
    # setupUi

    def retranslateUi(self, MenuWorkspaceDescribe):
        MenuWorkspaceDescribe.setWindowTitle(QCoreApplication.translate("MenuWorkspaceDescribe", u"CHANGE WORKSPACES DESCRIPTIONS", None))
        self.label_comments.setText(QCoreApplication.translate("MenuWorkspaceDescribe", u"Comments", None))
        self.label_equations.setText(QCoreApplication.translate("MenuWorkspaceDescribe", u"Equations", None))
        self.label_identities.setText(QCoreApplication.translate("MenuWorkspaceDescribe", u"Identities", None))
        self.label_lists.setText(QCoreApplication.translate("MenuWorkspaceDescribe", u"Lists", None))
        self.label_scalars.setText(QCoreApplication.translate("MenuWorkspaceDescribe", u"Scalars", None))
        self.label_tables.setText(QCoreApplication.translate("MenuWorkspaceDescribe", u"Tables", None))
        self.label_variables.setText(QCoreApplication.translate("MenuWorkspaceDescribe", u"Variables", None))
        self.pushButton_describe.setText(QCoreApplication.translate("MenuWorkspaceDescribe", u"Describe", None))
#if QT_CONFIG(shortcut)
        self.pushButton_describe.setShortcut(QCoreApplication.translate("MenuWorkspaceDescribe", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuWorkspaceDescribe", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuWorkspaceDescribe", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuWorkspaceDescribe", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuWorkspaceDescribe", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

