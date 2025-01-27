# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'estimation_coefs.ui'
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
from PySide6.QtWidgets import (QApplication, QDialog, QHeaderView, QSizePolicy,
    QTableView, QVBoxLayout, QWidget)

class Ui_EstimationCoefsDialog(object):
    def setupUi(self, EstimationCoefsDialog):
        if not EstimationCoefsDialog.objectName():
            EstimationCoefsDialog.setObjectName(u"EstimationCoefsDialog")
        EstimationCoefsDialog.setWindowModality(Qt.WindowModal)
        EstimationCoefsDialog.resize(600, 320)
        EstimationCoefsDialog.setFocusPolicy(Qt.NoFocus)
        self.verticalLayout = QVBoxLayout(EstimationCoefsDialog)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.tableView_coefs = QTableView(EstimationCoefsDialog)
        self.tableView_coefs.setObjectName(u"tableView_coefs")

        self.verticalLayout.addWidget(self.tableView_coefs)


        self.retranslateUi(EstimationCoefsDialog)

        QMetaObject.connectSlotsByName(EstimationCoefsDialog)
    # setupUi

    def retranslateUi(self, EstimationCoefsDialog):
        EstimationCoefsDialog.setWindowTitle(QCoreApplication.translate("EstimationCoefsDialog", u"Coefficients", None))
    # retranslateUi

