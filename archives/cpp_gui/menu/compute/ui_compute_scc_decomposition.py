# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'compute_scc_decomposition.ui'
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
from PySide6.QtWidgets import (QApplication, QDialog, QFrame, QGridLayout,
    QLabel, QLineEdit, QPushButton, QSizePolicy,
    QSpacerItem, QSpinBox, QWidget)

from text_edit.complete_text_edit import IodeAutoCompleteTextEdit

class Ui_MenuComputeSCCDecomposition(object):
    def setupUi(self, MenuComputeSCCDecomposition):
        if not MenuComputeSCCDecomposition.objectName():
            MenuComputeSCCDecomposition.setObjectName(u"MenuComputeSCCDecomposition")
        MenuComputeSCCDecomposition.resize(600, 300)
        MenuComputeSCCDecomposition.setMinimumSize(QSize(600, 300))
        MenuComputeSCCDecomposition.setMaximumSize(QSize(600, 300))
        icon = QIcon()
        icon.addFile(u"icons/Iodenew.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        MenuComputeSCCDecomposition.setWindowIcon(icon)
        self.gridLayout = QGridLayout(MenuComputeSCCDecomposition)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_equations_list = QLabel(MenuComputeSCCDecomposition)
        self.label_equations_list.setObjectName(u"label_equations_list")
        self.label_equations_list.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_equations_list, 0, 0, 3, 1)

        self.textEdit_equations_list = IodeAutoCompleteTextEdit(MenuComputeSCCDecomposition)
        self.textEdit_equations_list.setObjectName(u"textEdit_equations_list")

        self.gridLayout.addWidget(self.textEdit_equations_list, 0, 1, 4, 5)

        self.OKButton = QPushButton(MenuComputeSCCDecomposition)
        self.OKButton.setObjectName(u"OKButton")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.OKButton.sizePolicy().hasHeightForWidth())
        self.OKButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.OKButton, 0, 6, 1, 1)

        self.cancelButton = QPushButton(MenuComputeSCCDecomposition)
        self.cancelButton.setObjectName(u"cancelButton")
        sizePolicy.setHeightForWidth(self.cancelButton.sizePolicy().hasHeightForWidth())
        self.cancelButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.cancelButton, 1, 6, 1, 1)

        self.helpButton = QPushButton(MenuComputeSCCDecomposition)
        self.helpButton.setObjectName(u"helpButton")
        sizePolicy.setHeightForWidth(self.helpButton.sizePolicy().hasHeightForWidth())
        self.helpButton.setSizePolicy(sizePolicy)

        self.gridLayout.addWidget(self.helpButton, 2, 6, 1, 1)

        self.verticalSpacer = QSpacerItem(20, 189, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 3, 6, 7, 1)

        self.label_triangulation_iterations = QLabel(MenuComputeSCCDecomposition)
        self.label_triangulation_iterations.setObjectName(u"label_triangulation_iterations")

        self.gridLayout.addWidget(self.label_triangulation_iterations, 4, 0, 1, 1)

        self.spinBox_triangulation_iterations = QSpinBox(MenuComputeSCCDecomposition)
        self.spinBox_triangulation_iterations.setObjectName(u"spinBox_triangulation_iterations")
        self.spinBox_triangulation_iterations.setMinimumSize(QSize(60, 0))
        self.spinBox_triangulation_iterations.setMaximumSize(QSize(60, 16777215))
        self.spinBox_triangulation_iterations.setValue(5)

        self.gridLayout.addWidget(self.spinBox_triangulation_iterations, 4, 1, 1, 1)

        self.horizontalSpacer = QSpacerItem(232, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 4, 2, 1, 2)

        self.label_2 = QLabel(MenuComputeSCCDecomposition)
        self.label_2.setObjectName(u"label_2")

        self.gridLayout.addWidget(self.label_2, 4, 4, 1, 1)

        self.line = QFrame(MenuComputeSCCDecomposition)
        self.line.setObjectName(u"line")
        self.line.setFrameShape(QFrame.Shape.HLine)
        self.line.setFrameShadow(QFrame.Shadow.Sunken)

        self.gridLayout.addWidget(self.line, 5, 0, 1, 6)

        self.label_save_title = QLabel(MenuComputeSCCDecomposition)
        self.label_save_title.setObjectName(u"label_save_title")
        self.label_save_title.setTextFormat(Qt.MarkdownText)

        self.gridLayout.addWidget(self.label_save_title, 6, 0, 1, 1)

        self.label_pre_recursive = QLabel(MenuComputeSCCDecomposition)
        self.label_pre_recursive.setObjectName(u"label_pre_recursive")

        self.gridLayout.addWidget(self.label_pre_recursive, 7, 0, 1, 1)

        self.lineEdit_pre_recursive_list_name = QLineEdit(MenuComputeSCCDecomposition)
        self.lineEdit_pre_recursive_list_name.setObjectName(u"lineEdit_pre_recursive_list_name")
        self.lineEdit_pre_recursive_list_name.setMinimumSize(QSize(180, 0))
        self.lineEdit_pre_recursive_list_name.setMaximumSize(QSize(180, 16777215))

        self.gridLayout.addWidget(self.lineEdit_pre_recursive_list_name, 7, 1, 1, 2)

        self.horizontalSpacer_2 = QSpacerItem(174, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_2, 7, 3, 1, 3)

        self.label_inter_recursive = QLabel(MenuComputeSCCDecomposition)
        self.label_inter_recursive.setObjectName(u"label_inter_recursive")

        self.gridLayout.addWidget(self.label_inter_recursive, 8, 0, 1, 1)

        self.lineEdit_inter_recursive_list_name = QLineEdit(MenuComputeSCCDecomposition)
        self.lineEdit_inter_recursive_list_name.setObjectName(u"lineEdit_inter_recursive_list_name")
        self.lineEdit_inter_recursive_list_name.setMinimumSize(QSize(180, 0))
        self.lineEdit_inter_recursive_list_name.setMaximumSize(QSize(180, 16777215))

        self.gridLayout.addWidget(self.lineEdit_inter_recursive_list_name, 8, 1, 1, 2)

        self.horizontalSpacer_3 = QSpacerItem(174, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_3, 8, 3, 1, 3)

        self.label_post_recursive = QLabel(MenuComputeSCCDecomposition)
        self.label_post_recursive.setObjectName(u"label_post_recursive")

        self.gridLayout.addWidget(self.label_post_recursive, 9, 0, 1, 1)

        self.lineEdit_post_recursive_list_name = QLineEdit(MenuComputeSCCDecomposition)
        self.lineEdit_post_recursive_list_name.setObjectName(u"lineEdit_post_recursive_list_name")
        self.lineEdit_post_recursive_list_name.setMinimumSize(QSize(180, 0))
        self.lineEdit_post_recursive_list_name.setMaximumSize(QSize(180, 16777215))

        self.gridLayout.addWidget(self.lineEdit_post_recursive_list_name, 9, 1, 1, 2)

        self.horizontalSpacer_4 = QSpacerItem(174, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_4, 9, 3, 1, 3)


        self.retranslateUi(MenuComputeSCCDecomposition)
        self.helpButton.clicked.connect(MenuComputeSCCDecomposition.help)
        self.cancelButton.clicked.connect(MenuComputeSCCDecomposition.reject)
        self.OKButton.clicked.connect(MenuComputeSCCDecomposition.compute)

        QMetaObject.connectSlotsByName(MenuComputeSCCDecomposition)
    # setupUi

    def retranslateUi(self, MenuComputeSCCDecomposition):
        MenuComputeSCCDecomposition.setWindowTitle(QCoreApplication.translate("MenuComputeSCCDecomposition", u"SCC DECOMPOSITION", None))
        self.label_equations_list.setText(QCoreApplication.translate("MenuComputeSCCDecomposition", u"Equations List\n"
"\n"
"(empty for\n"
"all equations)", None))
        self.OKButton.setText(QCoreApplication.translate("MenuComputeSCCDecomposition", u"OK", None))
#if QT_CONFIG(shortcut)
        self.OKButton.setShortcut(QCoreApplication.translate("MenuComputeSCCDecomposition", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.cancelButton.setText(QCoreApplication.translate("MenuComputeSCCDecomposition", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.cancelButton.setShortcut(QCoreApplication.translate("MenuComputeSCCDecomposition", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.helpButton.setText(QCoreApplication.translate("MenuComputeSCCDecomposition", u"Help", None))
#if QT_CONFIG(shortcut)
        self.helpButton.setShortcut(QCoreApplication.translate("MenuComputeSCCDecomposition", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label_triangulation_iterations.setText(QCoreApplication.translate("MenuComputeSCCDecomposition", u"Triangulation Iterations", None))
        self.label_2.setText("")
        self.label_save_title.setText(QCoreApplication.translate("MenuComputeSCCDecomposition", u"### Save Results In Lists", None))
        self.label_pre_recursive.setText(QCoreApplication.translate("MenuComputeSCCDecomposition", u"Pre-recursive list name", None))
        self.lineEdit_pre_recursive_list_name.setText(QCoreApplication.translate("MenuComputeSCCDecomposition", u"_PRE", None))
        self.label_inter_recursive.setText(QCoreApplication.translate("MenuComputeSCCDecomposition", u"recursive list name", None))
        self.lineEdit_inter_recursive_list_name.setText(QCoreApplication.translate("MenuComputeSCCDecomposition", u"_INTER", None))
        self.label_post_recursive.setText(QCoreApplication.translate("MenuComputeSCCDecomposition", u"Post-recursive list name", None))
        self.lineEdit_post_recursive_list_name.setText(QCoreApplication.translate("MenuComputeSCCDecomposition", u"_POST", None))
    # retranslateUi

