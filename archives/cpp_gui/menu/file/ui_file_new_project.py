# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'file_new_project.ui'
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

from util.widgets.file_chooser import IodeFileChooser

class Ui_MenuFileNewProject(object):
    def setupUi(self, MenuFileNewProject):
        if not MenuFileNewProject.objectName():
            MenuFileNewProject.setObjectName(u"MenuFileNewProject")
        MenuFileNewProject.resize(631, 79)
        self.layoutWidget = QWidget(MenuFileNewProject)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(10, 20, 601, 48))
        self.gridLayout = QGridLayout(self.layoutWidget)
        self.gridLayout.setObjectName(u"gridLayout")
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.label_location = QLabel(self.layoutWidget)
        self.label_location.setObjectName(u"label_location")

        self.gridLayout.addWidget(self.label_location, 0, 0, 1, 1)

        self.fileChooser_new_project = IodeFileChooser(self.layoutWidget)
        self.fileChooser_new_project.setObjectName(u"fileChooser_new_project")
        self.fileChooser_new_project.setMinimumSize(QSize(540, 0))

        self.gridLayout.addWidget(self.fileChooser_new_project, 0, 1, 1, 2)

        self.horizontalSpacer = QSpacerItem(518, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 1, 0, 1, 2)

        self.create_button = QPushButton(self.layoutWidget)
        self.create_button.setObjectName(u"create_button")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.create_button.sizePolicy().hasHeightForWidth())
        self.create_button.setSizePolicy(sizePolicy)
        self.create_button.setMinimumSize(QSize(80, 0))

        self.gridLayout.addWidget(self.create_button, 1, 2, 1, 1)


        self.retranslateUi(MenuFileNewProject)
        self.create_button.clicked.connect(MenuFileNewProject.create)

        QMetaObject.connectSlotsByName(MenuFileNewProject)
    # setupUi

    def retranslateUi(self, MenuFileNewProject):
        MenuFileNewProject.setWindowTitle(QCoreApplication.translate("MenuFileNewProject", u"New Project", None))
#if QT_CONFIG(tooltip)
        MenuFileNewProject.setToolTip(QCoreApplication.translate("MenuFileNewProject", u"Create a new IODE project", None))
#endif // QT_CONFIG(tooltip)
        self.label_location.setText(QCoreApplication.translate("MenuFileNewProject", u"Location:", None))
        self.create_button.setText(QCoreApplication.translate("MenuFileNewProject", u"Create", None))
#if QT_CONFIG(shortcut)
        self.create_button.setShortcut(QCoreApplication.translate("MenuFileNewProject", u"F10", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

