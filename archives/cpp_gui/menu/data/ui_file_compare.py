# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'file_compare.ui'
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
    QGridLayout, QLabel, QLineEdit, QPushButton,
    QSizePolicy, QSpacerItem, QVBoxLayout, QWidget)

from util.widgets.file_chooser import IodeFileChooser

class Ui_MenuDataFileCompare(object):
    def setupUi(self, MenuDataFileCompare):
        if not MenuDataFileCompare.objectName():
            MenuDataFileCompare.setObjectName(u"MenuDataFileCompare")
        MenuDataFileCompare.resize(509, 263)
        self.gridLayout = QGridLayout(MenuDataFileCompare)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_iode_types = QLabel(MenuDataFileCompare)
        self.label_iode_types.setObjectName(u"label_iode_types")
        self.label_iode_types.setMinimumSize(QSize(70, 20))
        self.label_iode_types.setMaximumSize(QSize(70, 20))
        self.label_iode_types.setAlignment(Qt.AlignLeading|Qt.AlignLeft|Qt.AlignVCenter)

        self.gridLayout.addWidget(self.label_iode_types, 0, 0, 1, 1)

        self.comboBox_iode_types = QComboBox(MenuDataFileCompare)
        self.comboBox_iode_types.setObjectName(u"comboBox_iode_types")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.comboBox_iode_types.sizePolicy().hasHeightForWidth())
        self.comboBox_iode_types.setSizePolicy(sizePolicy)
        self.comboBox_iode_types.setMinimumSize(QSize(150, 20))
        self.comboBox_iode_types.setMaximumSize(QSize(150, 20))

        self.gridLayout.addWidget(self.comboBox_iode_types, 0, 1, 1, 2)

        self.horizontalSpacer = QSpacerItem(123, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer, 0, 3, 1, 1)

        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.pushButton_compare = QPushButton(MenuDataFileCompare)
        self.pushButton_compare.setObjectName(u"pushButton_compare")

        self.verticalLayout.addWidget(self.pushButton_compare)

        self.pushButton_cancel = QPushButton(MenuDataFileCompare)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.verticalLayout.addWidget(self.pushButton_cancel)

        self.pushButton_help = QPushButton(MenuDataFileCompare)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.verticalLayout.addWidget(self.pushButton_help)


        self.gridLayout.addLayout(self.verticalLayout, 0, 4, 3, 1)

        self.label_threshold = QLabel(MenuDataFileCompare)
        self.label_threshold.setObjectName(u"label_threshold")
        self.label_threshold.setMinimumSize(QSize(0, 20))
        self.label_threshold.setMaximumSize(QSize(16777215, 20))

        self.gridLayout.addWidget(self.label_threshold, 1, 0, 1, 2)

        self.lineEdit_threshold = QLineEdit(MenuDataFileCompare)
        self.lineEdit_threshold.setObjectName(u"lineEdit_threshold")

        self.gridLayout.addWidget(self.lineEdit_threshold, 1, 2, 1, 1)

        self.horizontalSpacer_2 = QSpacerItem(123, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_2, 1, 3, 1, 1)

        self.label_input_file = QLabel(MenuDataFileCompare)
        self.label_input_file.setObjectName(u"label_input_file")
        self.label_input_file.setMinimumSize(QSize(70, 0))
        self.label_input_file.setMaximumSize(QSize(70, 16777215))
        self.label_input_file.setAlignment(Qt.AlignLeading|Qt.AlignLeft|Qt.AlignVCenter)

        self.gridLayout.addWidget(self.label_input_file, 2, 0, 1, 1)

        self.fileChooser_input_file = IodeFileChooser(MenuDataFileCompare)
        self.fileChooser_input_file.setObjectName(u"fileChooser_input_file")
        self.fileChooser_input_file.setMinimumSize(QSize(0, 20))

        self.gridLayout.addWidget(self.fileChooser_input_file, 2, 1, 1, 3)

        self.line = QFrame(MenuDataFileCompare)
        self.line.setObjectName(u"line")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.line.sizePolicy().hasHeightForWidth())
        self.line.setSizePolicy(sizePolicy1)
        self.line.setFrameShape(QFrame.Shape.HLine)
        self.line.setFrameShadow(QFrame.Shadow.Sunken)

        self.gridLayout.addWidget(self.line, 3, 0, 1, 4)

        self.verticalSpacer = QSpacerItem(20, 146, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout.addItem(self.verticalSpacer, 3, 4, 6, 1)

        self.label_2 = QLabel(MenuDataFileCompare)
        self.label_2.setObjectName(u"label_2")

        self.gridLayout.addWidget(self.label_2, 4, 0, 1, 2)

        self.label_list_ws_only = QLabel(MenuDataFileCompare)
        self.label_list_ws_only.setObjectName(u"label_list_ws_only")

        self.gridLayout.addWidget(self.label_list_ws_only, 5, 0, 1, 2)

        self.lineEdit_list_ws_only = QLineEdit(MenuDataFileCompare)
        self.lineEdit_list_ws_only.setObjectName(u"lineEdit_list_ws_only")

        self.gridLayout.addWidget(self.lineEdit_list_ws_only, 5, 2, 1, 1)

        self.label_nb_elements_ws_only = QLabel(MenuDataFileCompare)
        self.label_nb_elements_ws_only.setObjectName(u"label_nb_elements_ws_only")

        self.gridLayout.addWidget(self.label_nb_elements_ws_only, 5, 3, 1, 1)

        self.label_list_file_only = QLabel(MenuDataFileCompare)
        self.label_list_file_only.setObjectName(u"label_list_file_only")

        self.gridLayout.addWidget(self.label_list_file_only, 6, 0, 1, 2)

        self.lineEdit_list_file_only = QLineEdit(MenuDataFileCompare)
        self.lineEdit_list_file_only.setObjectName(u"lineEdit_list_file_only")

        self.gridLayout.addWidget(self.lineEdit_list_file_only, 6, 2, 1, 1)

        self.label_nb_elements_file_only = QLabel(MenuDataFileCompare)
        self.label_nb_elements_file_only.setObjectName(u"label_nb_elements_file_only")

        self.gridLayout.addWidget(self.label_nb_elements_file_only, 6, 3, 1, 1)

        self.label_list_both_equal = QLabel(MenuDataFileCompare)
        self.label_list_both_equal.setObjectName(u"label_list_both_equal")

        self.gridLayout.addWidget(self.label_list_both_equal, 7, 0, 1, 2)

        self.lineEdit_list_both_equal = QLineEdit(MenuDataFileCompare)
        self.lineEdit_list_both_equal.setObjectName(u"lineEdit_list_both_equal")

        self.gridLayout.addWidget(self.lineEdit_list_both_equal, 7, 2, 1, 1)

        self.label_nb_elements_both_equal = QLabel(MenuDataFileCompare)
        self.label_nb_elements_both_equal.setObjectName(u"label_nb_elements_both_equal")

        self.gridLayout.addWidget(self.label_nb_elements_both_equal, 7, 3, 1, 1)

        self.label_list_both_different = QLabel(MenuDataFileCompare)
        self.label_list_both_different.setObjectName(u"label_list_both_different")

        self.gridLayout.addWidget(self.label_list_both_different, 8, 0, 1, 2)

        self.lineEdit_list_both_different = QLineEdit(MenuDataFileCompare)
        self.lineEdit_list_both_different.setObjectName(u"lineEdit_list_both_different")

        self.gridLayout.addWidget(self.lineEdit_list_both_different, 8, 2, 1, 1)

        self.label_nb_elements_both_different = QLabel(MenuDataFileCompare)
        self.label_nb_elements_both_different.setObjectName(u"label_nb_elements_both_different")

        self.gridLayout.addWidget(self.label_nb_elements_both_different, 8, 3, 1, 1)


        self.retranslateUi(MenuDataFileCompare)
        self.pushButton_compare.clicked.connect(MenuDataFileCompare.compare)
        self.pushButton_cancel.clicked.connect(MenuDataFileCompare.reject)
        self.pushButton_help.clicked.connect(MenuDataFileCompare.help)

        QMetaObject.connectSlotsByName(MenuDataFileCompare)
    # setupUi

    def retranslateUi(self, MenuDataFileCompare):
        MenuDataFileCompare.setWindowTitle(QCoreApplication.translate("MenuDataFileCompare", u"COMPARE OBJECTS FROM WS WITH FILE", None))
        self.label_iode_types.setText(QCoreApplication.translate("MenuDataFileCompare", u"Compare", None))
        self.pushButton_compare.setText(QCoreApplication.translate("MenuDataFileCompare", u"Compare", None))
#if QT_CONFIG(shortcut)
        self.pushButton_compare.setShortcut(QCoreApplication.translate("MenuDataFileCompare", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_cancel.setText(QCoreApplication.translate("MenuDataFileCompare", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("MenuDataFileCompare", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("MenuDataFileCompare", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("MenuDataFileCompare", u"F1", None))
#endif // QT_CONFIG(shortcut)
        self.label_threshold.setText(QCoreApplication.translate("MenuDataFileCompare", u"Threshold (vars only)", None))
        self.lineEdit_threshold.setText(QCoreApplication.translate("MenuDataFileCompare", u"1e-07", None))
        self.label_input_file.setText(QCoreApplication.translate("MenuDataFileCompare", u"With File", None))
        self.label_2.setText(QCoreApplication.translate("MenuDataFileCompare", u"Save results in lists:", None))
        self.label_list_ws_only.setText(QCoreApplication.translate("MenuDataFileCompare", u"Found in WS only", None))
        self.lineEdit_list_ws_only.setText(QCoreApplication.translate("MenuDataFileCompare", u"OLD", None))
        self.label_nb_elements_ws_only.setText(QCoreApplication.translate("MenuDataFileCompare", u"0 Elements", None))
        self.label_list_file_only.setText(QCoreApplication.translate("MenuDataFileCompare", u"Found in file only", None))
        self.lineEdit_list_file_only.setText(QCoreApplication.translate("MenuDataFileCompare", u"NEW", None))
        self.label_nb_elements_file_only.setText(QCoreApplication.translate("MenuDataFileCompare", u"0 Elements", None))
        self.label_list_both_equal.setText(QCoreApplication.translate("MenuDataFileCompare", u"Found in both, equal", None))
        self.lineEdit_list_both_equal.setText(QCoreApplication.translate("MenuDataFileCompare", u"SAME", None))
        self.label_nb_elements_both_equal.setText(QCoreApplication.translate("MenuDataFileCompare", u"0 Elements", None))
        self.label_list_both_different.setText(QCoreApplication.translate("MenuDataFileCompare", u"Found in both, different", None))
        self.lineEdit_list_both_different.setText(QCoreApplication.translate("MenuDataFileCompare", u"CHANGED", None))
        self.label_nb_elements_both_different.setText(QCoreApplication.translate("MenuDataFileCompare", u"0 Elements", None))
    # retranslateUi

