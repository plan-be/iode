# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'find_and_replace_dialog.ui'
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
    QLineEdit, QPushButton, QSizePolicy, QWidget)
import resources.iode_resource_rc

class Ui_FindAndReplaceDialog(object):
    def setupUi(self, FindAndReplaceDialog):
        if not FindAndReplaceDialog.objectName():
            FindAndReplaceDialog.setObjectName(u"FindAndReplaceDialog")
        FindAndReplaceDialog.resize(290, 72)
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(FindAndReplaceDialog.sizePolicy().hasHeightForWidth())
        FindAndReplaceDialog.setSizePolicy(sizePolicy)
        FindAndReplaceDialog.setMinimumSize(QSize(290, 30))
        FindAndReplaceDialog.setMaximumSize(QSize(290, 72))
        self.gridLayout = QGridLayout(FindAndReplaceDialog)
        self.gridLayout.setObjectName(u"gridLayout")
        self.lineReplace = QLineEdit(FindAndReplaceDialog)
        self.lineReplace.setObjectName(u"lineReplace")
        sizePolicy.setHeightForWidth(self.lineReplace.sizePolicy().hasHeightForWidth())
        self.lineReplace.setSizePolicy(sizePolicy)
        self.lineReplace.setMinimumSize(QSize(130, 20))
        self.lineReplace.setMaximumSize(QSize(130, 20))

        self.gridLayout.addWidget(self.lineReplace, 2, 0, 1, 1)

        self.replaceButton = QPushButton(FindAndReplaceDialog)
        self.replaceButton.setObjectName(u"replaceButton")
        sizePolicy.setHeightForWidth(self.replaceButton.sizePolicy().hasHeightForWidth())
        self.replaceButton.setSizePolicy(sizePolicy)
        self.replaceButton.setMinimumSize(QSize(20, 20))
        self.replaceButton.setMaximumSize(QSize(20, 20))
        icon = QIcon()
        icon.addFile(u":/icons/replace.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.replaceButton.setIcon(icon)
        self.replaceButton.setFlat(True)

        self.gridLayout.addWidget(self.replaceButton, 2, 1, 1, 1)

        self.replaceAllButton = QPushButton(FindAndReplaceDialog)
        self.replaceAllButton.setObjectName(u"replaceAllButton")
        sizePolicy.setHeightForWidth(self.replaceAllButton.sizePolicy().hasHeightForWidth())
        self.replaceAllButton.setSizePolicy(sizePolicy)
        self.replaceAllButton.setMinimumSize(QSize(20, 20))
        self.replaceAllButton.setMaximumSize(QSize(20, 20))
        icon1 = QIcon()
        icon1.addFile(u":/icons/replace_all.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.replaceAllButton.setIcon(icon1)
        self.replaceAllButton.setFlat(True)

        self.gridLayout.addWidget(self.replaceAllButton, 2, 2, 1, 1)

        self.lineSearch = QLineEdit(FindAndReplaceDialog)
        self.lineSearch.setObjectName(u"lineSearch")
        sizePolicy.setHeightForWidth(self.lineSearch.sizePolicy().hasHeightForWidth())
        self.lineSearch.setSizePolicy(sizePolicy)
        self.lineSearch.setMinimumSize(QSize(130, 20))
        self.lineSearch.setMaximumSize(QSize(130, 20))

        self.gridLayout.addWidget(self.lineSearch, 0, 0, 1, 1)

        self.nextButton = QPushButton(FindAndReplaceDialog)
        self.nextButton.setObjectName(u"nextButton")
        sizePolicy.setHeightForWidth(self.nextButton.sizePolicy().hasHeightForWidth())
        self.nextButton.setSizePolicy(sizePolicy)
        self.nextButton.setMinimumSize(QSize(20, 20))
        self.nextButton.setMaximumSize(QSize(20, 20))
        icon2 = QIcon()
        icon2.addFile(u":/icons/arrow_down.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.nextButton.setIcon(icon2)
        self.nextButton.setFlat(True)

        self.gridLayout.addWidget(self.nextButton, 0, 5, 1, 1)

        self.labelNbOccurrences = QLabel(FindAndReplaceDialog)
        self.labelNbOccurrences.setObjectName(u"labelNbOccurrences")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Preferred)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.labelNbOccurrences.sizePolicy().hasHeightForWidth())
        self.labelNbOccurrences.setSizePolicy(sizePolicy1)
        self.labelNbOccurrences.setMinimumSize(QSize(60, 20))
        self.labelNbOccurrences.setMaximumSize(QSize(60, 20))

        self.gridLayout.addWidget(self.labelNbOccurrences, 0, 1, 1, 2)

        self.previousButton = QPushButton(FindAndReplaceDialog)
        self.previousButton.setObjectName(u"previousButton")
        sizePolicy.setHeightForWidth(self.previousButton.sizePolicy().hasHeightForWidth())
        self.previousButton.setSizePolicy(sizePolicy)
        self.previousButton.setMinimumSize(QSize(20, 20))
        self.previousButton.setMaximumSize(QSize(20, 20))
        icon3 = QIcon()
        icon3.addFile(u":/icons/arrow_up.ico", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.previousButton.setIcon(icon3)
        self.previousButton.setFlat(True)

        self.gridLayout.addWidget(self.previousButton, 0, 3, 1, 1)


        self.retranslateUi(FindAndReplaceDialog)
        self.previousButton.clicked.connect(FindAndReplaceDialog.previous)
        self.nextButton.clicked.connect(FindAndReplaceDialog.next)
        self.replaceButton.clicked.connect(FindAndReplaceDialog.replace)
        self.replaceAllButton.clicked.connect(FindAndReplaceDialog.replaceAll)
        self.lineSearch.textChanged.connect(FindAndReplaceDialog.onSearchEdited)

        QMetaObject.connectSlotsByName(FindAndReplaceDialog)
    # setupUi

    def retranslateUi(self, FindAndReplaceDialog):
        FindAndReplaceDialog.setWindowTitle(QCoreApplication.translate("FindAndReplaceDialog", u"Find and Replace", None))
        self.lineReplace.setPlaceholderText(QCoreApplication.translate("FindAndReplaceDialog", u"replace by this text", None))
#if QT_CONFIG(tooltip)
        self.replaceButton.setToolTip(QCoreApplication.translate("FindAndReplaceDialog", u"replace (Enter)", None))
#endif // QT_CONFIG(tooltip)
        self.replaceButton.setText("")
#if QT_CONFIG(shortcut)
        self.replaceButton.setShortcut(QCoreApplication.translate("FindAndReplaceDialog", u"Return", None))
#endif // QT_CONFIG(shortcut)
#if QT_CONFIG(tooltip)
        self.replaceAllButton.setToolTip(QCoreApplication.translate("FindAndReplaceDialog", u"replace all (Ctrl + Alt + Enter)", None))
#endif // QT_CONFIG(tooltip)
        self.replaceAllButton.setText("")
#if QT_CONFIG(shortcut)
        self.replaceAllButton.setShortcut(QCoreApplication.translate("FindAndReplaceDialog", u"Ctrl+Alt+Return", None))
#endif // QT_CONFIG(shortcut)
        self.lineSearch.setPlaceholderText(QCoreApplication.translate("FindAndReplaceDialog", u"text to search", None))
#if QT_CONFIG(tooltip)
        self.nextButton.setToolTip(QCoreApplication.translate("FindAndReplaceDialog", u"next match (Enter)", None))
#endif // QT_CONFIG(tooltip)
        self.nextButton.setText("")
#if QT_CONFIG(shortcut)
        self.nextButton.setShortcut(QCoreApplication.translate("FindAndReplaceDialog", u"Return", None))
#endif // QT_CONFIG(shortcut)
        self.labelNbOccurrences.setText(QCoreApplication.translate("FindAndReplaceDialog", u"No Results", None))
#if QT_CONFIG(tooltip)
        self.previousButton.setToolTip(QCoreApplication.translate("FindAndReplaceDialog", u"previous match (Shift + Enter)", None))
#endif // QT_CONFIG(tooltip)
        self.previousButton.setText("")
#if QT_CONFIG(shortcut)
        self.previousButton.setShortcut(QCoreApplication.translate("FindAndReplaceDialog", u"Shift+Return", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

