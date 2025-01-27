# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'file_settings.ui'
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
from PySide6.QtWidgets import (QApplication, QButtonGroup, QComboBox, QDialog,
    QGridLayout, QLabel, QPushButton, QRadioButton,
    QSizePolicy, QSpacerItem, QTabWidget, QWidget)

class Ui_MenuFileSettings(object):
    def setupUi(self, MenuFileSettings):
        if not MenuFileSettings.objectName():
            MenuFileSettings.setObjectName(u"MenuFileSettings")
        MenuFileSettings.resize(468, 152)
        self.gridLayout_2 = QGridLayout(MenuFileSettings)
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.tabWidget = QTabWidget(MenuFileSettings)
        self.tabWidget.setObjectName(u"tabWidget")
        self.tabWidget.setTabPosition(QTabWidget.North)
        self.tab_print = QWidget()
        self.tab_print.setObjectName(u"tab_print")
        self.gridLayout_3 = QGridLayout(self.tab_print)
        self.gridLayout_3.setObjectName(u"gridLayout_3")
        self.label_print_dest = QLabel(self.tab_print)
        self.label_print_dest.setObjectName(u"label_print_dest")

        self.gridLayout_3.addWidget(self.label_print_dest, 0, 0, 1, 1)

        self.comboBox_print_dest = QComboBox(self.tab_print)
        self.comboBox_print_dest.setObjectName(u"comboBox_print_dest")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.comboBox_print_dest.sizePolicy().hasHeightForWidth())
        self.comboBox_print_dest.setSizePolicy(sizePolicy)
        self.comboBox_print_dest.setMinimumSize(QSize(160, 0))
        self.comboBox_print_dest.setMaximumSize(QSize(160, 16777215))

        self.gridLayout_3.addWidget(self.comboBox_print_dest, 0, 1, 1, 1)

        self.pushButton_options = QPushButton(self.tab_print)
        self.pushButton_options.setObjectName(u"pushButton_options")
        sizePolicy.setHeightForWidth(self.pushButton_options.sizePolicy().hasHeightForWidth())
        self.pushButton_options.setSizePolicy(sizePolicy)
        self.pushButton_options.setMinimumSize(QSize(80, 0))
        self.pushButton_options.setMaximumSize(QSize(80, 16777215))

        self.gridLayout_3.addWidget(self.pushButton_options, 0, 2, 1, 1)

        self.horizontalSpacer = QSpacerItem(125, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_3.addItem(self.horizontalSpacer, 0, 3, 1, 1)

        self.verticalSpacer = QSpacerItem(20, 24, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout_3.addItem(self.verticalSpacer, 1, 0, 1, 1)

        self.verticalSpacer_2 = QSpacerItem(20, 24, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout_3.addItem(self.verticalSpacer_2, 1, 1, 1, 1)

        self.verticalSpacer_3 = QSpacerItem(20, 24, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout_3.addItem(self.verticalSpacer_3, 1, 2, 1, 1)

        self.tabWidget.addTab(self.tab_print, "")
        self.tab_reports = QWidget()
        self.tab_reports.setObjectName(u"tab_reports")
        self.gridLayout = QGridLayout(self.tab_reports)
        self.gridLayout.setObjectName(u"gridLayout")
        self.radioButton_run_from_project_dir = QRadioButton(self.tab_reports)
        self.buttonGroup_reports = QButtonGroup(MenuFileSettings)
        self.buttonGroup_reports.setObjectName(u"buttonGroup_reports")
        self.buttonGroup_reports.addButton(self.radioButton_run_from_project_dir)
        self.radioButton_run_from_project_dir.setObjectName(u"radioButton_run_from_project_dir")
        self.radioButton_run_from_project_dir.setChecked(True)

        self.gridLayout.addWidget(self.radioButton_run_from_project_dir, 0, 0, 1, 1)

        self.radioButton_run_from_parent_dir = QRadioButton(self.tab_reports)
        self.buttonGroup_reports.addButton(self.radioButton_run_from_parent_dir)
        self.radioButton_run_from_parent_dir.setObjectName(u"radioButton_run_from_parent_dir")

        self.gridLayout.addWidget(self.radioButton_run_from_parent_dir, 1, 0, 1, 1)

        self.tabWidget.addTab(self.tab_reports, "")

        self.gridLayout_2.addWidget(self.tabWidget, 0, 0, 1, 4)

        self.horizontalSpacer_2 = QSpacerItem(189, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_2.addItem(self.horizontalSpacer_2, 1, 0, 1, 1)

        self.pushButton_apply = QPushButton(MenuFileSettings)
        self.pushButton_apply.setObjectName(u"pushButton_apply")
        sizePolicy.setHeightForWidth(self.pushButton_apply.sizePolicy().hasHeightForWidth())
        self.pushButton_apply.setSizePolicy(sizePolicy)
        self.pushButton_apply.setMinimumSize(QSize(80, 0))
        self.pushButton_apply.setMaximumSize(QSize(80, 16777215))

        self.gridLayout_2.addWidget(self.pushButton_apply, 1, 1, 1, 1)

        self.pushButton_cancel = QPushButton(MenuFileSettings)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")
        sizePolicy.setHeightForWidth(self.pushButton_cancel.sizePolicy().hasHeightForWidth())
        self.pushButton_cancel.setSizePolicy(sizePolicy)
        self.pushButton_cancel.setMinimumSize(QSize(80, 0))
        self.pushButton_cancel.setMaximumSize(QSize(80, 16777215))

        self.gridLayout_2.addWidget(self.pushButton_cancel, 1, 2, 1, 1)

        self.pushButton_help = QPushButton(MenuFileSettings)
        self.pushButton_help.setObjectName(u"pushButton_help")
        sizePolicy.setHeightForWidth(self.pushButton_help.sizePolicy().hasHeightForWidth())
        self.pushButton_help.setSizePolicy(sizePolicy)
        self.pushButton_help.setMinimumSize(QSize(80, 0))
        self.pushButton_help.setMaximumSize(QSize(80, 16777215))

        self.gridLayout_2.addWidget(self.pushButton_help, 1, 3, 1, 1)


        self.retranslateUi(MenuFileSettings)
        self.pushButton_apply.clicked.connect(MenuFileSettings.apply)
        self.pushButton_cancel.clicked.connect(MenuFileSettings.reject)
        self.pushButton_help.clicked.connect(MenuFileSettings.help)
        self.pushButton_options.clicked.connect(MenuFileSettings.set_print_options)

        self.tabWidget.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(MenuFileSettings)
    # setupUi

    def retranslateUi(self, MenuFileSettings):
        MenuFileSettings.setWindowTitle(QCoreApplication.translate("MenuFileSettings", u"SETTINGS", None))
        self.label_print_dest.setText(QCoreApplication.translate("MenuFileSettings", u"Print to", None))
        self.pushButton_options.setText(QCoreApplication.translate("MenuFileSettings", u"Options", None))
#if QT_CONFIG(shortcut)
        self.pushButton_options.setShortcut(QCoreApplication.translate("MenuFileSettings", u"F8", None))
#endif // QT_CONFIG(shortcut)
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_print), QCoreApplication.translate("MenuFileSettings", u"Print", None))
        self.radioButton_run_from_project_dir.setText(QCoreApplication.translate("MenuFileSettings", u"Run Reports From The Project Directory", None))
        self.radioButton_run_from_parent_dir.setText(QCoreApplication.translate("MenuFileSettings", u"Run Reports From The Parent Directory", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_reports), QCoreApplication.translate("MenuFileSettings", u"Reports", None))
        self.pushButton_apply.setText(QCoreApplication.translate("MenuFileSettings", u"Apply", None))
#if QT_CONFIG(shortcut)
        self.pushButton_apply.setShortcut(QCoreApplication.translate("MenuFileSettings", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuFileSettings", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuFileSettings", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuFileSettings", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuFileSettings", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

