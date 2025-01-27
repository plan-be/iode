# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'file_print_preferences.ui'
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
from PySide6.QtWidgets import (QApplication, QCheckBox, QComboBox, QDialog,
    QGridLayout, QGroupBox, QLabel, QLineEdit,
    QPushButton, QSizePolicy, QSpacerItem, QSpinBox,
    QTabWidget, QTextEdit, QWidget)

class Ui_FilePrintPreferences(object):
    def setupUi(self, FilePrintPreferences):
        if not FilePrintPreferences.objectName():
            FilePrintPreferences.setObjectName(u"FilePrintPreferences")
        FilePrintPreferences.resize(960, 467)
        FilePrintPreferences.setSizeGripEnabled(True)
        self.gridLayout_20 = QGridLayout(FilePrintPreferences)
        self.gridLayout_20.setObjectName(u"gridLayout_20")
        self.pushButton_cancel = QPushButton(FilePrintPreferences)
        self.pushButton_cancel.setObjectName(u"pushButton_cancel")

        self.gridLayout_20.addWidget(self.pushButton_cancel, 1, 1, 1, 1)

        self.tabWidget = QTabWidget(FilePrintPreferences)
        self.tabWidget.setObjectName(u"tabWidget")
        self.Printer = QWidget()
        self.Printer.setObjectName(u"Printer")
        self.gridLayout_13 = QGridLayout(self.Printer)
        self.gridLayout_13.setObjectName(u"gridLayout_13")
        self.groupBox_general = QGroupBox(self.Printer)
        self.groupBox_general.setObjectName(u"groupBox_general")
        self.gridLayout_4 = QGridLayout(self.groupBox_general)
        self.gridLayout_4.setObjectName(u"gridLayout_4")
        self.label_font_family = QLabel(self.groupBox_general)
        self.label_font_family.setObjectName(u"label_font_family")

        self.gridLayout_4.addWidget(self.label_font_family, 4, 0, 1, 1)

        self.checkBox_black_white = QCheckBox(self.groupBox_general)
        self.checkBox_black_white.setObjectName(u"checkBox_black_white")

        self.gridLayout_4.addWidget(self.checkBox_black_white, 0, 0, 1, 2)

        self.spinBox_font_size = QSpinBox(self.groupBox_general)
        self.spinBox_font_size.setObjectName(u"spinBox_font_size")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.spinBox_font_size.sizePolicy().hasHeightForWidth())
        self.spinBox_font_size.setSizePolicy(sizePolicy)
        self.spinBox_font_size.setMinimumSize(QSize(50, 0))
        self.spinBox_font_size.setMaximumSize(QSize(50, 16777215))
        self.spinBox_font_size.setMaximum(80)
        self.spinBox_font_size.setValue(10)

        self.gridLayout_4.addWidget(self.spinBox_font_size, 2, 1, 1, 1)

        self.comboBox_font_family = QComboBox(self.groupBox_general)
        self.comboBox_font_family.setObjectName(u"comboBox_font_family")

        self.gridLayout_4.addWidget(self.comboBox_font_family, 4, 1, 1, 2)

        self.horizontalSpacer = QSpacerItem(205, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_4.addItem(self.horizontalSpacer, 0, 2, 1, 1)

        self.horizontalSpacer_2 = QSpacerItem(205, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_4.addItem(self.horizontalSpacer_2, 1, 2, 1, 1)

        self.spinBox_font_incr = QSpinBox(self.groupBox_general)
        self.spinBox_font_incr.setObjectName(u"spinBox_font_incr")
        sizePolicy.setHeightForWidth(self.spinBox_font_incr.sizePolicy().hasHeightForWidth())
        self.spinBox_font_incr.setSizePolicy(sizePolicy)
        self.spinBox_font_incr.setMinimumSize(QSize(50, 0))
        self.spinBox_font_incr.setMaximumSize(QSize(50, 16777215))
        self.spinBox_font_incr.setMaximum(20)
        self.spinBox_font_incr.setValue(2)

        self.gridLayout_4.addWidget(self.spinBox_font_incr, 3, 1, 1, 1)

        self.label_font_incr = QLabel(self.groupBox_general)
        self.label_font_incr.setObjectName(u"label_font_incr")

        self.gridLayout_4.addWidget(self.label_font_incr, 3, 0, 1, 1)

        self.checkBox_para_numbering = QCheckBox(self.groupBox_general)
        self.checkBox_para_numbering.setObjectName(u"checkBox_para_numbering")

        self.gridLayout_4.addWidget(self.checkBox_para_numbering, 1, 0, 1, 2)

        self.horizontalSpacer_3 = QSpacerItem(205, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_4.addItem(self.horizontalSpacer_3, 2, 2, 1, 1)

        self.horizontalSpacer_4 = QSpacerItem(205, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_4.addItem(self.horizontalSpacer_4, 3, 2, 1, 1)

        self.label_font_size = QLabel(self.groupBox_general)
        self.label_font_size.setObjectName(u"label_font_size")

        self.gridLayout_4.addWidget(self.label_font_size, 2, 0, 1, 1)


        self.gridLayout_13.addWidget(self.groupBox_general, 0, 0, 1, 1)

        self.groupBox_page_setup = QGroupBox(self.Printer)
        self.groupBox_page_setup.setObjectName(u"groupBox_page_setup")
        self.gridLayout_6 = QGridLayout(self.groupBox_page_setup)
        self.gridLayout_6.setObjectName(u"gridLayout_6")
        self.label_margin_left = QLabel(self.groupBox_page_setup)
        self.label_margin_left.setObjectName(u"label_margin_left")

        self.gridLayout_6.addWidget(self.label_margin_left, 0, 0, 1, 1)

        self.spinBox_margin_left = QSpinBox(self.groupBox_page_setup)
        self.spinBox_margin_left.setObjectName(u"spinBox_margin_left")
        sizePolicy.setHeightForWidth(self.spinBox_margin_left.sizePolicy().hasHeightForWidth())
        self.spinBox_margin_left.setSizePolicy(sizePolicy)
        self.spinBox_margin_left.setMinimumSize(QSize(50, 0))
        self.spinBox_margin_left.setMaximumSize(QSize(50, 16777215))
        self.spinBox_margin_left.setMaximum(20)
        self.spinBox_margin_left.setValue(10)

        self.gridLayout_6.addWidget(self.spinBox_margin_left, 0, 1, 1, 1)

        self.label_margin_top = QLabel(self.groupBox_page_setup)
        self.label_margin_top.setObjectName(u"label_margin_top")

        self.gridLayout_6.addWidget(self.label_margin_top, 0, 2, 1, 1)

        self.spinBox_margin_top = QSpinBox(self.groupBox_page_setup)
        self.spinBox_margin_top.setObjectName(u"spinBox_margin_top")
        sizePolicy.setHeightForWidth(self.spinBox_margin_top.sizePolicy().hasHeightForWidth())
        self.spinBox_margin_top.setSizePolicy(sizePolicy)
        self.spinBox_margin_top.setMinimumSize(QSize(50, 0))
        self.spinBox_margin_top.setMaximumSize(QSize(50, 16777215))
        self.spinBox_margin_top.setMaximum(20)
        self.spinBox_margin_top.setValue(10)

        self.gridLayout_6.addWidget(self.spinBox_margin_top, 0, 3, 1, 1)

        self.horizontalSpacer_6 = QSpacerItem(539, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_6.addItem(self.horizontalSpacer_6, 0, 4, 1, 1)

        self.label_margin_right = QLabel(self.groupBox_page_setup)
        self.label_margin_right.setObjectName(u"label_margin_right")

        self.gridLayout_6.addWidget(self.label_margin_right, 1, 0, 1, 1)

        self.spinBox_margin_right = QSpinBox(self.groupBox_page_setup)
        self.spinBox_margin_right.setObjectName(u"spinBox_margin_right")
        sizePolicy.setHeightForWidth(self.spinBox_margin_right.sizePolicy().hasHeightForWidth())
        self.spinBox_margin_right.setSizePolicy(sizePolicy)
        self.spinBox_margin_right.setMinimumSize(QSize(50, 0))
        self.spinBox_margin_right.setMaximumSize(QSize(50, 16777215))
        self.spinBox_margin_right.setMaximum(20)
        self.spinBox_margin_right.setValue(10)

        self.gridLayout_6.addWidget(self.spinBox_margin_right, 1, 1, 1, 1)

        self.label_margin_bottom = QLabel(self.groupBox_page_setup)
        self.label_margin_bottom.setObjectName(u"label_margin_bottom")

        self.gridLayout_6.addWidget(self.label_margin_bottom, 1, 2, 1, 1)

        self.spinBox_margin_bottom = QSpinBox(self.groupBox_page_setup)
        self.spinBox_margin_bottom.setObjectName(u"spinBox_margin_bottom")
        sizePolicy.setHeightForWidth(self.spinBox_margin_bottom.sizePolicy().hasHeightForWidth())
        self.spinBox_margin_bottom.setSizePolicy(sizePolicy)
        self.spinBox_margin_bottom.setMinimumSize(QSize(50, 0))
        self.spinBox_margin_bottom.setMaximumSize(QSize(50, 16777215))
        self.spinBox_margin_bottom.setMaximum(20)
        self.spinBox_margin_bottom.setValue(10)

        self.gridLayout_6.addWidget(self.spinBox_margin_bottom, 1, 3, 1, 1)

        self.horizontalSpacer_7 = QSpacerItem(539, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_6.addItem(self.horizontalSpacer_7, 1, 4, 1, 1)

        self.checkBox_prompt_printer = QCheckBox(self.groupBox_page_setup)
        self.checkBox_prompt_printer.setObjectName(u"checkBox_prompt_printer")

        self.gridLayout_6.addWidget(self.checkBox_prompt_printer, 2, 0, 1, 2)

        self.horizontalSpacer_5 = QSpacerItem(682, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_6.addItem(self.horizontalSpacer_5, 2, 2, 1, 3)

        self.label_page_header = QLabel(self.groupBox_page_setup)
        self.label_page_header.setObjectName(u"label_page_header")

        self.gridLayout_6.addWidget(self.label_page_header, 3, 0, 1, 1)

        self.lineEdit_page_header = QLineEdit(self.groupBox_page_setup)
        self.lineEdit_page_header.setObjectName(u"lineEdit_page_header")

        self.gridLayout_6.addWidget(self.lineEdit_page_header, 3, 1, 1, 4)

        self.label_page_footer = QLabel(self.groupBox_page_setup)
        self.label_page_footer.setObjectName(u"label_page_footer")

        self.gridLayout_6.addWidget(self.label_page_footer, 4, 0, 1, 1)

        self.lineEdit_page_footer = QLineEdit(self.groupBox_page_setup)
        self.lineEdit_page_footer.setObjectName(u"lineEdit_page_footer")

        self.gridLayout_6.addWidget(self.lineEdit_page_footer, 4, 1, 1, 4)


        self.gridLayout_13.addWidget(self.groupBox_page_setup, 1, 0, 1, 2)

        self.groupBox_tables_graphics = QGroupBox(self.Printer)
        self.groupBox_tables_graphics.setObjectName(u"groupBox_tables_graphics")
        self.gridLayout_5 = QGridLayout(self.groupBox_tables_graphics)
        self.gridLayout_5.setObjectName(u"gridLayout_5")
        self.checkBox_table_color = QCheckBox(self.groupBox_tables_graphics)
        self.checkBox_table_color.setObjectName(u"checkBox_table_color")

        self.gridLayout_5.addWidget(self.checkBox_table_color, 0, 0, 1, 1)

        self.label_table_font_size = QLabel(self.groupBox_tables_graphics)
        self.label_table_font_size.setObjectName(u"label_table_font_size")

        self.gridLayout_5.addWidget(self.label_table_font_size, 0, 1, 1, 1)

        self.spinBox_table_font_size = QSpinBox(self.groupBox_tables_graphics)
        self.spinBox_table_font_size.setObjectName(u"spinBox_table_font_size")
        sizePolicy.setHeightForWidth(self.spinBox_table_font_size.sizePolicy().hasHeightForWidth())
        self.spinBox_table_font_size.setSizePolicy(sizePolicy)
        self.spinBox_table_font_size.setMinimumSize(QSize(50, 0))
        self.spinBox_table_font_size.setMaximumSize(QSize(50, 16777215))
        self.spinBox_table_font_size.setMaximum(80)
        self.spinBox_table_font_size.setValue(10)

        self.gridLayout_5.addWidget(self.spinBox_table_font_size, 0, 2, 1, 1)

        self.horizontalSpacer_8 = QSpacerItem(174, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_5.addItem(self.horizontalSpacer_8, 0, 3, 1, 1)

        self.checkBox_table_shading = QCheckBox(self.groupBox_tables_graphics)
        self.checkBox_table_shading.setObjectName(u"checkBox_table_shading")

        self.gridLayout_5.addWidget(self.checkBox_table_shading, 1, 0, 1, 1)

        self.label_table_font_family = QLabel(self.groupBox_tables_graphics)
        self.label_table_font_family.setObjectName(u"label_table_font_family")

        self.gridLayout_5.addWidget(self.label_table_font_family, 1, 1, 1, 1)

        self.checkBox_table_break = QCheckBox(self.groupBox_tables_graphics)
        self.checkBox_table_break.setObjectName(u"checkBox_table_break")

        self.gridLayout_5.addWidget(self.checkBox_table_break, 2, 0, 1, 1)

        self.label_table_border_width = QLabel(self.groupBox_tables_graphics)
        self.label_table_border_width.setObjectName(u"label_table_border_width")

        self.gridLayout_5.addWidget(self.label_table_border_width, 2, 1, 1, 1)

        self.spinBox_table_border_width = QSpinBox(self.groupBox_tables_graphics)
        self.spinBox_table_border_width.setObjectName(u"spinBox_table_border_width")
        sizePolicy.setHeightForWidth(self.spinBox_table_border_width.sizePolicy().hasHeightForWidth())
        self.spinBox_table_border_width.setSizePolicy(sizePolicy)
        self.spinBox_table_border_width.setMinimumSize(QSize(50, 0))
        self.spinBox_table_border_width.setMaximumSize(QSize(50, 16777215))
        self.spinBox_table_border_width.setMaximum(10)
        self.spinBox_table_border_width.setValue(0)

        self.gridLayout_5.addWidget(self.spinBox_table_border_width, 2, 2, 1, 1)

        self.horizontalSpacer_9 = QSpacerItem(174, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_5.addItem(self.horizontalSpacer_9, 2, 3, 1, 1)

        self.checkBox_table_new_page = QCheckBox(self.groupBox_tables_graphics)
        self.checkBox_table_new_page.setObjectName(u"checkBox_table_new_page")

        self.gridLayout_5.addWidget(self.checkBox_table_new_page, 3, 0, 1, 1)

        self.horizontalSpacer_10 = QSpacerItem(338, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_5.addItem(self.horizontalSpacer_10, 3, 1, 1, 3)

        self.checkBox_graph_new_page = QCheckBox(self.groupBox_tables_graphics)
        self.checkBox_graph_new_page.setObjectName(u"checkBox_graph_new_page")

        self.gridLayout_5.addWidget(self.checkBox_graph_new_page, 4, 0, 1, 1)

        self.horizontalSpacer_11 = QSpacerItem(338, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_5.addItem(self.horizontalSpacer_11, 4, 1, 1, 3)

        self.comboBox_table_font_family = QComboBox(self.groupBox_tables_graphics)
        self.comboBox_table_font_family.setObjectName(u"comboBox_table_font_family")

        self.gridLayout_5.addWidget(self.comboBox_table_font_family, 1, 2, 1, 2)


        self.gridLayout_13.addWidget(self.groupBox_tables_graphics, 0, 1, 1, 1)

        self.tabWidget.addTab(self.Printer, "")
        self.A2M = QWidget()
        self.A2M.setObjectName(u"A2M")
        self.gridLayout_14 = QGridLayout(self.A2M)
        self.gridLayout_14.setObjectName(u"gridLayout_14")
        self.groupBox_A2M_control_characters = QGroupBox(self.A2M)
        self.groupBox_A2M_control_characters.setObjectName(u"groupBox_A2M_control_characters")
        self.gridLayout = QGridLayout(self.groupBox_A2M_control_characters)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_A2M_escape_character = QLabel(self.groupBox_A2M_control_characters)
        self.label_A2M_escape_character.setObjectName(u"label_A2M_escape_character")

        self.gridLayout.addWidget(self.label_A2M_escape_character, 0, 0, 1, 1)

        self.lineEdit_A2M_escape_character = QLineEdit(self.groupBox_A2M_control_characters)
        self.lineEdit_A2M_escape_character.setObjectName(u"lineEdit_A2M_escape_character")
        sizePolicy.setHeightForWidth(self.lineEdit_A2M_escape_character.sizePolicy().hasHeightForWidth())
        self.lineEdit_A2M_escape_character.setSizePolicy(sizePolicy)
        self.lineEdit_A2M_escape_character.setMinimumSize(QSize(20, 0))
        self.lineEdit_A2M_escape_character.setMaximumSize(QSize(20, 16777215))
        self.lineEdit_A2M_escape_character.setMaxLength(1)

        self.gridLayout.addWidget(self.lineEdit_A2M_escape_character, 0, 1, 1, 1)

        self.label_A2M_define_character = QLabel(self.groupBox_A2M_control_characters)
        self.label_A2M_define_character.setObjectName(u"label_A2M_define_character")

        self.gridLayout.addWidget(self.label_A2M_define_character, 0, 2, 1, 1)

        self.lineEdit_A2M_define_character = QLineEdit(self.groupBox_A2M_control_characters)
        self.lineEdit_A2M_define_character.setObjectName(u"lineEdit_A2M_define_character")
        sizePolicy.setHeightForWidth(self.lineEdit_A2M_define_character.sizePolicy().hasHeightForWidth())
        self.lineEdit_A2M_define_character.setSizePolicy(sizePolicy)
        self.lineEdit_A2M_define_character.setMinimumSize(QSize(20, 0))
        self.lineEdit_A2M_define_character.setMaximumSize(QSize(20, 16777215))
        self.lineEdit_A2M_define_character.setMaxLength(1)

        self.gridLayout.addWidget(self.lineEdit_A2M_define_character, 0, 3, 1, 1)

        self.horizontalSpacer_37 = QSpacerItem(552, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_37, 0, 4, 1, 1)

        self.label_A2M_command_character = QLabel(self.groupBox_A2M_control_characters)
        self.label_A2M_command_character.setObjectName(u"label_A2M_command_character")

        self.gridLayout.addWidget(self.label_A2M_command_character, 1, 0, 1, 1)

        self.lineEdit_A2M_command_character = QLineEdit(self.groupBox_A2M_control_characters)
        self.lineEdit_A2M_command_character.setObjectName(u"lineEdit_A2M_command_character")
        sizePolicy.setHeightForWidth(self.lineEdit_A2M_command_character.sizePolicy().hasHeightForWidth())
        self.lineEdit_A2M_command_character.setSizePolicy(sizePolicy)
        self.lineEdit_A2M_command_character.setMinimumSize(QSize(20, 0))
        self.lineEdit_A2M_command_character.setMaximumSize(QSize(20, 16777215))
        self.lineEdit_A2M_command_character.setMaxLength(1)

        self.gridLayout.addWidget(self.lineEdit_A2M_command_character, 1, 1, 1, 1)

        self.label_A2M_cell_separator = QLabel(self.groupBox_A2M_control_characters)
        self.label_A2M_cell_separator.setObjectName(u"label_A2M_cell_separator")

        self.gridLayout.addWidget(self.label_A2M_cell_separator, 1, 2, 1, 1)

        self.lineEdit_A2M_cell_separator = QLineEdit(self.groupBox_A2M_control_characters)
        self.lineEdit_A2M_cell_separator.setObjectName(u"lineEdit_A2M_cell_separator")
        sizePolicy.setHeightForWidth(self.lineEdit_A2M_cell_separator.sizePolicy().hasHeightForWidth())
        self.lineEdit_A2M_cell_separator.setSizePolicy(sizePolicy)
        self.lineEdit_A2M_cell_separator.setMinimumSize(QSize(20, 0))
        self.lineEdit_A2M_cell_separator.setMaximumSize(QSize(20, 16777215))
        self.lineEdit_A2M_cell_separator.setMaxLength(1)

        self.gridLayout.addWidget(self.lineEdit_A2M_cell_separator, 1, 3, 1, 1)

        self.horizontalSpacer_38 = QSpacerItem(552, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_38, 1, 4, 1, 1)


        self.gridLayout_14.addWidget(self.groupBox_A2M_control_characters, 0, 0, 1, 1)

        self.groupBox_A2M_startup_options = QGroupBox(self.A2M)
        self.groupBox_A2M_startup_options.setObjectName(u"groupBox_A2M_startup_options")
        self.gridLayout_2 = QGridLayout(self.groupBox_A2M_startup_options)
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.label_A2M_default_paragraph = QLabel(self.groupBox_A2M_startup_options)
        self.label_A2M_default_paragraph.setObjectName(u"label_A2M_default_paragraph")

        self.gridLayout_2.addWidget(self.label_A2M_default_paragraph, 0, 0, 1, 1)

        self.lineEdit_A2M_default_paragraph = QLineEdit(self.groupBox_A2M_startup_options)
        self.lineEdit_A2M_default_paragraph.setObjectName(u"lineEdit_A2M_default_paragraph")
        self.lineEdit_A2M_default_paragraph.setMaxLength(30)

        self.gridLayout_2.addWidget(self.lineEdit_A2M_default_paragraph, 0, 1, 1, 2)

        self.horizontalSpacer_15 = QSpacerItem(554, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_2.addItem(self.horizontalSpacer_15, 0, 3, 1, 1)

        self.checkBox_A2M_preserve_spaces = QCheckBox(self.groupBox_A2M_startup_options)
        self.checkBox_A2M_preserve_spaces.setObjectName(u"checkBox_A2M_preserve_spaces")

        self.gridLayout_2.addWidget(self.checkBox_A2M_preserve_spaces, 1, 0, 1, 1)

        self.horizontalSpacer_12 = QSpacerItem(692, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_2.addItem(self.horizontalSpacer_12, 1, 1, 1, 3)

        self.checkBox_A2M_preserve_linefeed = QCheckBox(self.groupBox_A2M_startup_options)
        self.checkBox_A2M_preserve_linefeed.setObjectName(u"checkBox_A2M_preserve_linefeed")

        self.gridLayout_2.addWidget(self.checkBox_A2M_preserve_linefeed, 2, 0, 1, 1)

        self.horizontalSpacer_13 = QSpacerItem(692, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_2.addItem(self.horizontalSpacer_13, 2, 1, 1, 3)

        self.checkBox_A2M_append_to_existing_file = QCheckBox(self.groupBox_A2M_startup_options)
        self.checkBox_A2M_append_to_existing_file.setObjectName(u"checkBox_A2M_append_to_existing_file")

        self.gridLayout_2.addWidget(self.checkBox_A2M_append_to_existing_file, 3, 0, 1, 2)

        self.horizontalSpacer_14 = QSpacerItem(138, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_2.addItem(self.horizontalSpacer_14, 3, 2, 1, 2)


        self.gridLayout_14.addWidget(self.groupBox_A2M_startup_options, 1, 0, 1, 1)

        self.groupBox_A2M_graphs = QGroupBox(self.A2M)
        self.groupBox_A2M_graphs.setObjectName(u"groupBox_A2M_graphs")
        self.gridLayout_3 = QGridLayout(self.groupBox_A2M_graphs)
        self.gridLayout_3.setObjectName(u"gridLayout_3")
        self.label_A2M_graph_width = QLabel(self.groupBox_A2M_graphs)
        self.label_A2M_graph_width.setObjectName(u"label_A2M_graph_width")

        self.gridLayout_3.addWidget(self.label_A2M_graph_width, 0, 0, 1, 1)

        self.spinBox_A2M_graph_width = QSpinBox(self.groupBox_A2M_graphs)
        self.spinBox_A2M_graph_width.setObjectName(u"spinBox_A2M_graph_width")
        self.spinBox_A2M_graph_width.setMaximum(100000)
        self.spinBox_A2M_graph_width.setValue(160)

        self.gridLayout_3.addWidget(self.spinBox_A2M_graph_width, 0, 1, 1, 1)

        self.label_A2M_graph_background_color = QLabel(self.groupBox_A2M_graphs)
        self.label_A2M_graph_background_color.setObjectName(u"label_A2M_graph_background_color")

        self.gridLayout_3.addWidget(self.label_A2M_graph_background_color, 0, 2, 1, 1)

        self.comboBox_A2M_graph_background_color = QComboBox(self.groupBox_A2M_graphs)
        self.comboBox_A2M_graph_background_color.setObjectName(u"comboBox_A2M_graph_background_color")
        self.comboBox_A2M_graph_background_color.setMinimumSize(QSize(120, 0))
        self.comboBox_A2M_graph_background_color.setMaximumSize(QSize(120, 16777215))

        self.gridLayout_3.addWidget(self.comboBox_A2M_graph_background_color, 0, 3, 1, 1)

        self.label_A2M_graph_box = QLabel(self.groupBox_A2M_graphs)
        self.label_A2M_graph_box.setObjectName(u"label_A2M_graph_box")

        self.gridLayout_3.addWidget(self.label_A2M_graph_box, 0, 4, 1, 1)

        self.spinBox_A2M_graph_box = QSpinBox(self.groupBox_A2M_graphs)
        self.spinBox_A2M_graph_box.setObjectName(u"spinBox_A2M_graph_box")
        self.spinBox_A2M_graph_box.setMaximum(100)

        self.gridLayout_3.addWidget(self.spinBox_A2M_graph_box, 0, 5, 1, 1)

        self.horizontalSpacer_39 = QSpacerItem(373, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_3.addItem(self.horizontalSpacer_39, 0, 6, 1, 1)

        self.label_A2M_graph_height = QLabel(self.groupBox_A2M_graphs)
        self.label_A2M_graph_height.setObjectName(u"label_A2M_graph_height")

        self.gridLayout_3.addWidget(self.label_A2M_graph_height, 1, 0, 1, 1)

        self.spinBox_A2M_graph_height = QSpinBox(self.groupBox_A2M_graphs)
        self.spinBox_A2M_graph_height.setObjectName(u"spinBox_A2M_graph_height")
        self.spinBox_A2M_graph_height.setMaximum(100000)
        self.spinBox_A2M_graph_height.setValue(100)

        self.gridLayout_3.addWidget(self.spinBox_A2M_graph_height, 1, 1, 1, 1)

        self.label_A2M_graph_background_brush = QLabel(self.groupBox_A2M_graphs)
        self.label_A2M_graph_background_brush.setObjectName(u"label_A2M_graph_background_brush")

        self.gridLayout_3.addWidget(self.label_A2M_graph_background_brush, 1, 2, 1, 1)

        self.spinBox_A2M_graph_background_brush = QSpinBox(self.groupBox_A2M_graphs)
        self.spinBox_A2M_graph_background_brush.setObjectName(u"spinBox_A2M_graph_background_brush")
        self.spinBox_A2M_graph_background_brush.setMaximum(100)

        self.gridLayout_3.addWidget(self.spinBox_A2M_graph_background_brush, 1, 3, 1, 1)

        self.horizontalSpacer_16 = QSpacerItem(446, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_3.addItem(self.horizontalSpacer_16, 1, 4, 1, 3)


        self.gridLayout_14.addWidget(self.groupBox_A2M_graphs, 2, 0, 1, 1)

        self.tabWidget.addTab(self.A2M, "")
        self.MIF = QWidget()
        self.MIF.setObjectName(u"MIF")
        self.gridLayout_18 = QGridLayout(self.MIF)
        self.gridLayout_18.setObjectName(u"gridLayout_18")
        self.groupBox_MIF_font = QGroupBox(self.MIF)
        self.groupBox_MIF_font.setObjectName(u"groupBox_MIF_font")
        self.gridLayout_15 = QGridLayout(self.groupBox_MIF_font)
        self.gridLayout_15.setObjectName(u"gridLayout_15")
        self.label_MIF_font_family = QLabel(self.groupBox_MIF_font)
        self.label_MIF_font_family.setObjectName(u"label_MIF_font_family")

        self.gridLayout_15.addWidget(self.label_MIF_font_family, 2, 0, 1, 1)

        self.horizontalSpacer_17 = QSpacerItem(545, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_15.addItem(self.horizontalSpacer_17, 1, 2, 1, 2)

        self.label_MIF_font_size = QLabel(self.groupBox_MIF_font)
        self.label_MIF_font_size.setObjectName(u"label_MIF_font_size")

        self.gridLayout_15.addWidget(self.label_MIF_font_size, 0, 0, 1, 1)

        self.horizontalSpacer_40 = QSpacerItem(395, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_15.addItem(self.horizontalSpacer_40, 2, 3, 1, 1)

        self.spinBox_MIF_font_incr = QSpinBox(self.groupBox_MIF_font)
        self.spinBox_MIF_font_incr.setObjectName(u"spinBox_MIF_font_incr")
        sizePolicy.setHeightForWidth(self.spinBox_MIF_font_incr.sizePolicy().hasHeightForWidth())
        self.spinBox_MIF_font_incr.setSizePolicy(sizePolicy)
        self.spinBox_MIF_font_incr.setMinimumSize(QSize(50, 0))
        self.spinBox_MIF_font_incr.setMaximumSize(QSize(50, 16777215))
        self.spinBox_MIF_font_incr.setMaximum(20)
        self.spinBox_MIF_font_incr.setValue(2)

        self.gridLayout_15.addWidget(self.spinBox_MIF_font_incr, 1, 1, 1, 1)

        self.horizontalSpacer_18 = QSpacerItem(545, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_15.addItem(self.horizontalSpacer_18, 0, 2, 1, 2)

        self.label_MIF_font_incr = QLabel(self.groupBox_MIF_font)
        self.label_MIF_font_incr.setObjectName(u"label_MIF_font_incr")

        self.gridLayout_15.addWidget(self.label_MIF_font_incr, 1, 0, 1, 1)

        self.spinBox_MIF_font_size = QSpinBox(self.groupBox_MIF_font)
        self.spinBox_MIF_font_size.setObjectName(u"spinBox_MIF_font_size")
        sizePolicy.setHeightForWidth(self.spinBox_MIF_font_size.sizePolicy().hasHeightForWidth())
        self.spinBox_MIF_font_size.setSizePolicy(sizePolicy)
        self.spinBox_MIF_font_size.setMinimumSize(QSize(50, 0))
        self.spinBox_MIF_font_size.setMaximumSize(QSize(50, 16777215))
        self.spinBox_MIF_font_size.setMaximum(80)
        self.spinBox_MIF_font_size.setValue(10)

        self.gridLayout_15.addWidget(self.spinBox_MIF_font_size, 0, 1, 1, 1)

        self.comboBox_MIF_font_family = QComboBox(self.groupBox_MIF_font)
        self.comboBox_MIF_font_family.setObjectName(u"comboBox_MIF_font_family")
        sizePolicy.setHeightForWidth(self.comboBox_MIF_font_family.sizePolicy().hasHeightForWidth())
        self.comboBox_MIF_font_family.setSizePolicy(sizePolicy)
        self.comboBox_MIF_font_family.setMinimumSize(QSize(200, 0))
        self.comboBox_MIF_font_family.setMaximumSize(QSize(200, 16777215))

        self.gridLayout_15.addWidget(self.comboBox_MIF_font_family, 2, 1, 1, 2)


        self.gridLayout_18.addWidget(self.groupBox_MIF_font, 0, 0, 1, 1)

        self.groupBox_MIF_miscellaneous = QGroupBox(self.MIF)
        self.groupBox_MIF_miscellaneous.setObjectName(u"groupBox_MIF_miscellaneous")
        self.gridLayout_17 = QGridLayout(self.groupBox_MIF_miscellaneous)
        self.gridLayout_17.setObjectName(u"gridLayout_17")
        self.checkBox_MIF_image_ref_in_text = QCheckBox(self.groupBox_MIF_miscellaneous)
        self.checkBox_MIF_image_ref_in_text.setObjectName(u"checkBox_MIF_image_ref_in_text")
        self.checkBox_MIF_image_ref_in_text.setMinimumSize(QSize(120, 0))
        self.checkBox_MIF_image_ref_in_text.setMaximumSize(QSize(120, 16777215))

        self.gridLayout_17.addWidget(self.checkBox_MIF_image_ref_in_text, 0, 0, 1, 1)

        self.verticalSpacer_2 = QSpacerItem(20, 158, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout_17.addItem(self.verticalSpacer_2, 1, 0, 1, 1)


        self.gridLayout_18.addWidget(self.groupBox_MIF_miscellaneous, 0, 1, 1, 1)

        self.groupBox_MIF_table_options = QGroupBox(self.MIF)
        self.groupBox_MIF_table_options.setObjectName(u"groupBox_MIF_table_options")
        self.gridLayout_16 = QGridLayout(self.groupBox_MIF_table_options)
        self.gridLayout_16.setObjectName(u"gridLayout_16")
        self.checkBox_MIF_table_use_color = QCheckBox(self.groupBox_MIF_table_options)
        self.checkBox_MIF_table_use_color.setObjectName(u"checkBox_MIF_table_use_color")

        self.gridLayout_16.addWidget(self.checkBox_MIF_table_use_color, 0, 0, 1, 1)

        self.label_MIF_table_width = QLabel(self.groupBox_MIF_table_options)
        self.label_MIF_table_width.setObjectName(u"label_MIF_table_width")

        self.gridLayout_16.addWidget(self.label_MIF_table_width, 0, 1, 1, 1)

        self.spinBox_MIF_table_width = QSpinBox(self.groupBox_MIF_table_options)
        self.spinBox_MIF_table_width.setObjectName(u"spinBox_MIF_table_width")
        sizePolicy.setHeightForWidth(self.spinBox_MIF_table_width.sizePolicy().hasHeightForWidth())
        self.spinBox_MIF_table_width.setSizePolicy(sizePolicy)
        self.spinBox_MIF_table_width.setMinimumSize(QSize(50, 0))
        self.spinBox_MIF_table_width.setMaximumSize(QSize(50, 16777215))
        self.spinBox_MIF_table_width.setMaximum(10000)
        self.spinBox_MIF_table_width.setValue(165)

        self.gridLayout_16.addWidget(self.spinBox_MIF_table_width, 0, 2, 1, 1)

        self.horizontalSpacer_19 = QSpacerItem(521, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_16.addItem(self.horizontalSpacer_19, 0, 3, 1, 2)

        self.checkBox_MIF_table_split_tables = QCheckBox(self.groupBox_MIF_table_options)
        self.checkBox_MIF_table_split_tables.setObjectName(u"checkBox_MIF_table_split_tables")

        self.gridLayout_16.addWidget(self.checkBox_MIF_table_split_tables, 1, 0, 1, 1)

        self.label_MIF_table_first_col_width = QLabel(self.groupBox_MIF_table_options)
        self.label_MIF_table_first_col_width.setObjectName(u"label_MIF_table_first_col_width")

        self.gridLayout_16.addWidget(self.label_MIF_table_first_col_width, 1, 1, 1, 1)

        self.spinBox_MIF_table_first_col_width = QSpinBox(self.groupBox_MIF_table_options)
        self.spinBox_MIF_table_first_col_width.setObjectName(u"spinBox_MIF_table_first_col_width")
        sizePolicy.setHeightForWidth(self.spinBox_MIF_table_first_col_width.sizePolicy().hasHeightForWidth())
        self.spinBox_MIF_table_first_col_width.setSizePolicy(sizePolicy)
        self.spinBox_MIF_table_first_col_width.setMinimumSize(QSize(50, 0))
        self.spinBox_MIF_table_first_col_width.setMaximumSize(QSize(50, 16777215))
        self.spinBox_MIF_table_first_col_width.setMaximum(10000)
        self.spinBox_MIF_table_first_col_width.setValue(60)

        self.gridLayout_16.addWidget(self.spinBox_MIF_table_first_col_width, 1, 2, 1, 1)

        self.horizontalSpacer_20 = QSpacerItem(521, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_16.addItem(self.horizontalSpacer_20, 1, 3, 1, 2)

        self.checkBox_MIF_table_outside_borders = QCheckBox(self.groupBox_MIF_table_options)
        self.checkBox_MIF_table_outside_borders.setObjectName(u"checkBox_MIF_table_outside_borders")

        self.gridLayout_16.addWidget(self.checkBox_MIF_table_outside_borders, 2, 0, 1, 1)

        self.label_MIF_table_other_col_width = QLabel(self.groupBox_MIF_table_options)
        self.label_MIF_table_other_col_width.setObjectName(u"label_MIF_table_other_col_width")

        self.gridLayout_16.addWidget(self.label_MIF_table_other_col_width, 2, 1, 1, 1)

        self.spinBox_MIF_table_other_col_width = QSpinBox(self.groupBox_MIF_table_options)
        self.spinBox_MIF_table_other_col_width.setObjectName(u"spinBox_MIF_table_other_col_width")
        sizePolicy.setHeightForWidth(self.spinBox_MIF_table_other_col_width.sizePolicy().hasHeightForWidth())
        self.spinBox_MIF_table_other_col_width.setSizePolicy(sizePolicy)
        self.spinBox_MIF_table_other_col_width.setMinimumSize(QSize(50, 0))
        self.spinBox_MIF_table_other_col_width.setMaximumSize(QSize(50, 16777215))
        self.spinBox_MIF_table_other_col_width.setMaximum(10000)
        self.spinBox_MIF_table_other_col_width.setValue(15)

        self.gridLayout_16.addWidget(self.spinBox_MIF_table_other_col_width, 2, 2, 1, 1)

        self.horizontalSpacer_21 = QSpacerItem(521, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_16.addItem(self.horizontalSpacer_21, 2, 3, 1, 2)

        self.checkBox_MIF_table_vertical_lines = QCheckBox(self.groupBox_MIF_table_options)
        self.checkBox_MIF_table_vertical_lines.setObjectName(u"checkBox_MIF_table_vertical_lines")

        self.gridLayout_16.addWidget(self.checkBox_MIF_table_vertical_lines, 3, 0, 1, 1)

        self.label_MIF_table_font_size = QLabel(self.groupBox_MIF_table_options)
        self.label_MIF_table_font_size.setObjectName(u"label_MIF_table_font_size")

        self.gridLayout_16.addWidget(self.label_MIF_table_font_size, 3, 1, 1, 1)

        self.spinBox_MIF_table_font_size = QSpinBox(self.groupBox_MIF_table_options)
        self.spinBox_MIF_table_font_size.setObjectName(u"spinBox_MIF_table_font_size")
        sizePolicy.setHeightForWidth(self.spinBox_MIF_table_font_size.sizePolicy().hasHeightForWidth())
        self.spinBox_MIF_table_font_size.setSizePolicy(sizePolicy)
        self.spinBox_MIF_table_font_size.setMinimumSize(QSize(50, 0))
        self.spinBox_MIF_table_font_size.setMaximumSize(QSize(50, 16777215))
        self.spinBox_MIF_table_font_size.setMaximum(100)
        self.spinBox_MIF_table_font_size.setValue(8)

        self.gridLayout_16.addWidget(self.spinBox_MIF_table_font_size, 3, 2, 1, 1)

        self.horizontalSpacer_22 = QSpacerItem(521, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_16.addItem(self.horizontalSpacer_22, 3, 3, 1, 2)

        self.checkBox_MIF_table_horizontal_lines = QCheckBox(self.groupBox_MIF_table_options)
        self.checkBox_MIF_table_horizontal_lines.setObjectName(u"checkBox_MIF_table_horizontal_lines")

        self.gridLayout_16.addWidget(self.checkBox_MIF_table_horizontal_lines, 4, 0, 1, 1)

        self.label_MIF_table_font_family = QLabel(self.groupBox_MIF_table_options)
        self.label_MIF_table_font_family.setObjectName(u"label_MIF_table_font_family")

        self.gridLayout_16.addWidget(self.label_MIF_table_font_family, 4, 1, 1, 1)

        self.comboBox_MIF_table_font_family = QComboBox(self.groupBox_MIF_table_options)
        self.comboBox_MIF_table_font_family.setObjectName(u"comboBox_MIF_table_font_family")
        sizePolicy.setHeightForWidth(self.comboBox_MIF_table_font_family.sizePolicy().hasHeightForWidth())
        self.comboBox_MIF_table_font_family.setSizePolicy(sizePolicy)
        self.comboBox_MIF_table_font_family.setMinimumSize(QSize(200, 0))
        self.comboBox_MIF_table_font_family.setMaximumSize(QSize(200, 16777215))

        self.gridLayout_16.addWidget(self.comboBox_MIF_table_font_family, 4, 2, 1, 2)

        self.horizontalSpacer_46 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_16.addItem(self.horizontalSpacer_46, 4, 4, 1, 1)


        self.gridLayout_18.addWidget(self.groupBox_MIF_table_options, 1, 0, 1, 2)

        self.tabWidget.addTab(self.MIF, "")
        self.RTF = QWidget()
        self.RTF.setObjectName(u"RTF")
        self.gridLayout_19 = QGridLayout(self.RTF)
        self.gridLayout_19.setObjectName(u"gridLayout_19")
        self.groupBox_general_2 = QGroupBox(self.RTF)
        self.groupBox_general_2.setObjectName(u"groupBox_general_2")
        self.gridLayout_7 = QGridLayout(self.groupBox_general_2)
        self.gridLayout_7.setObjectName(u"gridLayout_7")
        self.checkBox_RTF_paragraph_numbers = QCheckBox(self.groupBox_general_2)
        self.checkBox_RTF_paragraph_numbers.setObjectName(u"checkBox_RTF_paragraph_numbers")

        self.gridLayout_7.addWidget(self.checkBox_RTF_paragraph_numbers, 0, 0, 1, 2)

        self.horizontalSpacer_23 = QSpacerItem(239, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_7.addItem(self.horizontalSpacer_23, 0, 2, 1, 1)

        self.label_RTF_font_size = QLabel(self.groupBox_general_2)
        self.label_RTF_font_size.setObjectName(u"label_RTF_font_size")

        self.gridLayout_7.addWidget(self.label_RTF_font_size, 1, 0, 1, 1)

        self.spinBox_RTF_font_size = QSpinBox(self.groupBox_general_2)
        self.spinBox_RTF_font_size.setObjectName(u"spinBox_RTF_font_size")
        sizePolicy.setHeightForWidth(self.spinBox_RTF_font_size.sizePolicy().hasHeightForWidth())
        self.spinBox_RTF_font_size.setSizePolicy(sizePolicy)
        self.spinBox_RTF_font_size.setMinimumSize(QSize(50, 0))
        self.spinBox_RTF_font_size.setMaximumSize(QSize(50, 16777215))
        self.spinBox_RTF_font_size.setMaximum(80)
        self.spinBox_RTF_font_size.setValue(10)

        self.gridLayout_7.addWidget(self.spinBox_RTF_font_size, 1, 1, 1, 1)

        self.horizontalSpacer_25 = QSpacerItem(239, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_7.addItem(self.horizontalSpacer_25, 1, 2, 1, 1)

        self.label_RTF_font_incr = QLabel(self.groupBox_general_2)
        self.label_RTF_font_incr.setObjectName(u"label_RTF_font_incr")

        self.gridLayout_7.addWidget(self.label_RTF_font_incr, 2, 0, 1, 1)

        self.spinBox_RTF_font_incr = QSpinBox(self.groupBox_general_2)
        self.spinBox_RTF_font_incr.setObjectName(u"spinBox_RTF_font_incr")
        sizePolicy.setHeightForWidth(self.spinBox_RTF_font_incr.sizePolicy().hasHeightForWidth())
        self.spinBox_RTF_font_incr.setSizePolicy(sizePolicy)
        self.spinBox_RTF_font_incr.setMinimumSize(QSize(50, 0))
        self.spinBox_RTF_font_incr.setMaximumSize(QSize(50, 16777215))
        self.spinBox_RTF_font_incr.setMaximum(20)
        self.spinBox_RTF_font_incr.setValue(2)

        self.gridLayout_7.addWidget(self.spinBox_RTF_font_incr, 2, 1, 1, 1)

        self.horizontalSpacer_26 = QSpacerItem(239, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_7.addItem(self.horizontalSpacer_26, 2, 2, 1, 1)

        self.label_RTF_font_family = QLabel(self.groupBox_general_2)
        self.label_RTF_font_family.setObjectName(u"label_RTF_font_family")

        self.gridLayout_7.addWidget(self.label_RTF_font_family, 3, 0, 1, 1)

        self.comboBox_RTF_font_family = QComboBox(self.groupBox_general_2)
        self.comboBox_RTF_font_family.setObjectName(u"comboBox_RTF_font_family")

        self.gridLayout_7.addWidget(self.comboBox_RTF_font_family, 3, 1, 1, 2)


        self.gridLayout_19.addWidget(self.groupBox_general_2, 0, 0, 1, 1)

        self.groupBox_RTF_table_options = QGroupBox(self.RTF)
        self.groupBox_RTF_table_options.setObjectName(u"groupBox_RTF_table_options")
        self.gridLayout_8 = QGridLayout(self.groupBox_RTF_table_options)
        self.gridLayout_8.setObjectName(u"gridLayout_8")
        self.checkBox_RTF_table_use_color = QCheckBox(self.groupBox_RTF_table_options)
        self.checkBox_RTF_table_use_color.setObjectName(u"checkBox_RTF_table_use_color")

        self.gridLayout_8.addWidget(self.checkBox_RTF_table_use_color, 0, 0, 1, 1)

        self.label_RTF_table_width = QLabel(self.groupBox_RTF_table_options)
        self.label_RTF_table_width.setObjectName(u"label_RTF_table_width")

        self.gridLayout_8.addWidget(self.label_RTF_table_width, 0, 1, 1, 1)

        self.spinBox_RTF_table_width = QSpinBox(self.groupBox_RTF_table_options)
        self.spinBox_RTF_table_width.setObjectName(u"spinBox_RTF_table_width")
        sizePolicy.setHeightForWidth(self.spinBox_RTF_table_width.sizePolicy().hasHeightForWidth())
        self.spinBox_RTF_table_width.setSizePolicy(sizePolicy)
        self.spinBox_RTF_table_width.setMinimumSize(QSize(50, 0))
        self.spinBox_RTF_table_width.setMaximumSize(QSize(50, 16777215))
        self.spinBox_RTF_table_width.setMaximum(10000)
        self.spinBox_RTF_table_width.setValue(165)

        self.gridLayout_8.addWidget(self.spinBox_RTF_table_width, 0, 2, 1, 1)

        self.horizontalSpacer_24 = QSpacerItem(118, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_8.addItem(self.horizontalSpacer_24, 0, 3, 1, 1)

        self.checkBox_RTF_table_split_tables = QCheckBox(self.groupBox_RTF_table_options)
        self.checkBox_RTF_table_split_tables.setObjectName(u"checkBox_RTF_table_split_tables")

        self.gridLayout_8.addWidget(self.checkBox_RTF_table_split_tables, 1, 0, 1, 1)

        self.label_RTF_table_first_col_width = QLabel(self.groupBox_RTF_table_options)
        self.label_RTF_table_first_col_width.setObjectName(u"label_RTF_table_first_col_width")

        self.gridLayout_8.addWidget(self.label_RTF_table_first_col_width, 1, 1, 1, 1)

        self.spinBox_RTF_table_first_col_width = QSpinBox(self.groupBox_RTF_table_options)
        self.spinBox_RTF_table_first_col_width.setObjectName(u"spinBox_RTF_table_first_col_width")
        sizePolicy.setHeightForWidth(self.spinBox_RTF_table_first_col_width.sizePolicy().hasHeightForWidth())
        self.spinBox_RTF_table_first_col_width.setSizePolicy(sizePolicy)
        self.spinBox_RTF_table_first_col_width.setMinimumSize(QSize(50, 0))
        self.spinBox_RTF_table_first_col_width.setMaximumSize(QSize(50, 16777215))
        self.spinBox_RTF_table_first_col_width.setMaximum(10000)
        self.spinBox_RTF_table_first_col_width.setValue(60)

        self.gridLayout_8.addWidget(self.spinBox_RTF_table_first_col_width, 1, 2, 1, 1)

        self.horizontalSpacer_27 = QSpacerItem(118, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_8.addItem(self.horizontalSpacer_27, 1, 3, 1, 1)

        self.checkBox_RTF_table_outside_borders = QCheckBox(self.groupBox_RTF_table_options)
        self.checkBox_RTF_table_outside_borders.setObjectName(u"checkBox_RTF_table_outside_borders")

        self.gridLayout_8.addWidget(self.checkBox_RTF_table_outside_borders, 2, 0, 1, 1)

        self.label_RTF_table_other_col_width = QLabel(self.groupBox_RTF_table_options)
        self.label_RTF_table_other_col_width.setObjectName(u"label_RTF_table_other_col_width")

        self.gridLayout_8.addWidget(self.label_RTF_table_other_col_width, 2, 1, 1, 1)

        self.spinBox_RTF_table_other_col_width = QSpinBox(self.groupBox_RTF_table_options)
        self.spinBox_RTF_table_other_col_width.setObjectName(u"spinBox_RTF_table_other_col_width")
        sizePolicy.setHeightForWidth(self.spinBox_RTF_table_other_col_width.sizePolicy().hasHeightForWidth())
        self.spinBox_RTF_table_other_col_width.setSizePolicy(sizePolicy)
        self.spinBox_RTF_table_other_col_width.setMinimumSize(QSize(50, 0))
        self.spinBox_RTF_table_other_col_width.setMaximumSize(QSize(50, 16777215))
        self.spinBox_RTF_table_other_col_width.setMaximum(10000)
        self.spinBox_RTF_table_other_col_width.setValue(15)

        self.gridLayout_8.addWidget(self.spinBox_RTF_table_other_col_width, 2, 2, 1, 1)

        self.horizontalSpacer_28 = QSpacerItem(118, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_8.addItem(self.horizontalSpacer_28, 2, 3, 1, 1)

        self.checkBox_RTF_table_vertical_lines = QCheckBox(self.groupBox_RTF_table_options)
        self.checkBox_RTF_table_vertical_lines.setObjectName(u"checkBox_RTF_table_vertical_lines")

        self.gridLayout_8.addWidget(self.checkBox_RTF_table_vertical_lines, 3, 0, 1, 1)

        self.label_RTF_table_font_size = QLabel(self.groupBox_RTF_table_options)
        self.label_RTF_table_font_size.setObjectName(u"label_RTF_table_font_size")

        self.gridLayout_8.addWidget(self.label_RTF_table_font_size, 3, 1, 1, 1)

        self.spinBox_RTF_table_font_size = QSpinBox(self.groupBox_RTF_table_options)
        self.spinBox_RTF_table_font_size.setObjectName(u"spinBox_RTF_table_font_size")
        sizePolicy.setHeightForWidth(self.spinBox_RTF_table_font_size.sizePolicy().hasHeightForWidth())
        self.spinBox_RTF_table_font_size.setSizePolicy(sizePolicy)
        self.spinBox_RTF_table_font_size.setMinimumSize(QSize(50, 0))
        self.spinBox_RTF_table_font_size.setMaximumSize(QSize(50, 16777215))
        self.spinBox_RTF_table_font_size.setMaximum(100)
        self.spinBox_RTF_table_font_size.setValue(8)

        self.gridLayout_8.addWidget(self.spinBox_RTF_table_font_size, 3, 2, 1, 1)

        self.horizontalSpacer_29 = QSpacerItem(118, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_8.addItem(self.horizontalSpacer_29, 3, 3, 1, 1)

        self.checkBox_RTF_table_horizontal_lines = QCheckBox(self.groupBox_RTF_table_options)
        self.checkBox_RTF_table_horizontal_lines.setObjectName(u"checkBox_RTF_table_horizontal_lines")

        self.gridLayout_8.addWidget(self.checkBox_RTF_table_horizontal_lines, 4, 0, 1, 1)

        self.label_RTF_table_font_family = QLabel(self.groupBox_RTF_table_options)
        self.label_RTF_table_font_family.setObjectName(u"label_RTF_table_font_family")

        self.gridLayout_8.addWidget(self.label_RTF_table_font_family, 4, 1, 1, 1)

        self.comboBox_RTF_table_font_family = QComboBox(self.groupBox_RTF_table_options)
        self.comboBox_RTF_table_font_family.setObjectName(u"comboBox_RTF_table_font_family")

        self.gridLayout_8.addWidget(self.comboBox_RTF_table_font_family, 4, 2, 1, 2)


        self.gridLayout_19.addWidget(self.groupBox_RTF_table_options, 0, 1, 1, 1)

        self.groupBox_RTF_help_generation = QGroupBox(self.RTF)
        self.groupBox_RTF_help_generation.setObjectName(u"groupBox_RTF_help_generation")
        self.gridLayout_9 = QGridLayout(self.groupBox_RTF_help_generation)
        self.gridLayout_9.setObjectName(u"gridLayout_9")
        self.label_RTF_help_title = QLabel(self.groupBox_RTF_help_generation)
        self.label_RTF_help_title.setObjectName(u"label_RTF_help_title")

        self.gridLayout_9.addWidget(self.label_RTF_help_title, 0, 0, 1, 1)

        self.lineEdit_RTF_help_title = QLineEdit(self.groupBox_RTF_help_generation)
        self.lineEdit_RTF_help_title.setObjectName(u"lineEdit_RTF_help_title")
        self.lineEdit_RTF_help_title.setMinimumSize(QSize(500, 0))
        self.lineEdit_RTF_help_title.setMaximumSize(QSize(500, 16777215))

        self.gridLayout_9.addWidget(self.lineEdit_RTF_help_title, 0, 1, 1, 3)

        self.horizontalSpacer_41 = QSpacerItem(187, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_9.addItem(self.horizontalSpacer_41, 0, 4, 1, 1)

        self.label_RTF_copyright = QLabel(self.groupBox_RTF_help_generation)
        self.label_RTF_copyright.setObjectName(u"label_RTF_copyright")

        self.gridLayout_9.addWidget(self.label_RTF_copyright, 1, 0, 1, 1)

        self.lineEdit_RTF_copyright = QLineEdit(self.groupBox_RTF_help_generation)
        self.lineEdit_RTF_copyright.setObjectName(u"lineEdit_RTF_copyright")
        self.lineEdit_RTF_copyright.setMinimumSize(QSize(500, 0))
        self.lineEdit_RTF_copyright.setMaximumSize(QSize(500, 16777215))

        self.gridLayout_9.addWidget(self.lineEdit_RTF_copyright, 1, 1, 1, 3)

        self.horizontalSpacer_42 = QSpacerItem(187, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_9.addItem(self.horizontalSpacer_42, 1, 4, 1, 1)

        self.checkBox_RTF_prepare_for_HCW = QCheckBox(self.groupBox_RTF_help_generation)
        self.checkBox_RTF_prepare_for_HCW.setObjectName(u"checkBox_RTF_prepare_for_HCW")

        self.gridLayout_9.addWidget(self.checkBox_RTF_prepare_for_HCW, 2, 0, 1, 1)

        self.checkBox_RTF_compress_help = QCheckBox(self.groupBox_RTF_help_generation)
        self.checkBox_RTF_compress_help.setObjectName(u"checkBox_RTF_compress_help")

        self.gridLayout_9.addWidget(self.checkBox_RTF_compress_help, 2, 1, 1, 1)

        self.checkBox_RTF_generate_TOC = QCheckBox(self.groupBox_RTF_help_generation)
        self.checkBox_RTF_generate_TOC.setObjectName(u"checkBox_RTF_generate_TOC")

        self.gridLayout_9.addWidget(self.checkBox_RTF_generate_TOC, 2, 2, 1, 1)

        self.horizontalSpacer_43 = QSpacerItem(355, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_9.addItem(self.horizontalSpacer_43, 2, 3, 1, 2)


        self.gridLayout_19.addWidget(self.groupBox_RTF_help_generation, 1, 0, 1, 2)

        self.tabWidget.addTab(self.RTF, "")
        self.HTML = QWidget()
        self.HTML.setObjectName(u"HTML")
        self.gridLayout_12 = QGridLayout(self.HTML)
        self.gridLayout_12.setObjectName(u"gridLayout_12")
        self.groupBox_HTML_font = QGroupBox(self.HTML)
        self.groupBox_HTML_font.setObjectName(u"groupBox_HTML_font")
        self.gridLayout_10 = QGridLayout(self.groupBox_HTML_font)
        self.gridLayout_10.setObjectName(u"gridLayout_10")
        self.label_HTML_font_size = QLabel(self.groupBox_HTML_font)
        self.label_HTML_font_size.setObjectName(u"label_HTML_font_size")

        self.gridLayout_10.addWidget(self.label_HTML_font_size, 0, 0, 1, 1)

        self.spinBox_HTML_font_size = QSpinBox(self.groupBox_HTML_font)
        self.spinBox_HTML_font_size.setObjectName(u"spinBox_HTML_font_size")
        sizePolicy.setHeightForWidth(self.spinBox_HTML_font_size.sizePolicy().hasHeightForWidth())
        self.spinBox_HTML_font_size.setSizePolicy(sizePolicy)
        self.spinBox_HTML_font_size.setMinimumSize(QSize(50, 0))
        self.spinBox_HTML_font_size.setMaximumSize(QSize(50, 16777215))
        self.spinBox_HTML_font_size.setMaximum(80)
        self.spinBox_HTML_font_size.setValue(10)

        self.gridLayout_10.addWidget(self.spinBox_HTML_font_size, 0, 1, 1, 1)

        self.horizontalSpacer_30 = QSpacerItem(270, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_10.addItem(self.horizontalSpacer_30, 0, 2, 1, 1)

        self.label_HTML_font_incr = QLabel(self.groupBox_HTML_font)
        self.label_HTML_font_incr.setObjectName(u"label_HTML_font_incr")

        self.gridLayout_10.addWidget(self.label_HTML_font_incr, 1, 0, 1, 1)

        self.spinBox_HTML_font_incr = QSpinBox(self.groupBox_HTML_font)
        self.spinBox_HTML_font_incr.setObjectName(u"spinBox_HTML_font_incr")
        sizePolicy.setHeightForWidth(self.spinBox_HTML_font_incr.sizePolicy().hasHeightForWidth())
        self.spinBox_HTML_font_incr.setSizePolicy(sizePolicy)
        self.spinBox_HTML_font_incr.setMinimumSize(QSize(50, 0))
        self.spinBox_HTML_font_incr.setMaximumSize(QSize(50, 16777215))
        self.spinBox_HTML_font_incr.setMaximum(20)
        self.spinBox_HTML_font_incr.setValue(2)

        self.gridLayout_10.addWidget(self.spinBox_HTML_font_incr, 1, 1, 1, 1)

        self.horizontalSpacer_31 = QSpacerItem(270, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_10.addItem(self.horizontalSpacer_31, 1, 2, 1, 1)

        self.label_HTML_font_family = QLabel(self.groupBox_HTML_font)
        self.label_HTML_font_family.setObjectName(u"label_HTML_font_family")

        self.gridLayout_10.addWidget(self.label_HTML_font_family, 2, 0, 1, 1)

        self.comboBox_HTML_font_family = QComboBox(self.groupBox_HTML_font)
        self.comboBox_HTML_font_family.setObjectName(u"comboBox_HTML_font_family")

        self.gridLayout_10.addWidget(self.comboBox_HTML_font_family, 2, 1, 1, 2)


        self.gridLayout_12.addWidget(self.groupBox_HTML_font, 0, 0, 1, 1)

        self.groupBox_HTML_table_border_width_table_options = QGroupBox(self.HTML)
        self.groupBox_HTML_table_border_width_table_options.setObjectName(u"groupBox_HTML_table_border_width_table_options")
        self.gridLayout_11 = QGridLayout(self.groupBox_HTML_table_border_width_table_options)
        self.gridLayout_11.setObjectName(u"gridLayout_11")
        self.checkBox_HTML_table_use_color = QCheckBox(self.groupBox_HTML_table_border_width_table_options)
        self.checkBox_HTML_table_use_color.setObjectName(u"checkBox_HTML_table_use_color")

        self.gridLayout_11.addWidget(self.checkBox_HTML_table_use_color, 0, 0, 1, 1)

        self.horizontalSpacer_32 = QSpacerItem(268, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_11.addItem(self.horizontalSpacer_32, 0, 1, 1, 2)

        self.label_HTML_table_border_width = QLabel(self.groupBox_HTML_table_border_width_table_options)
        self.label_HTML_table_border_width.setObjectName(u"label_HTML_table_border_width")

        self.gridLayout_11.addWidget(self.label_HTML_table_border_width, 1, 0, 1, 1)

        self.spinBox_HTML_table_border_width = QSpinBox(self.groupBox_HTML_table_border_width_table_options)
        self.spinBox_HTML_table_border_width.setObjectName(u"spinBox_HTML_table_border_width")
        sizePolicy.setHeightForWidth(self.spinBox_HTML_table_border_width.sizePolicy().hasHeightForWidth())
        self.spinBox_HTML_table_border_width.setSizePolicy(sizePolicy)
        self.spinBox_HTML_table_border_width.setMinimumSize(QSize(50, 0))
        self.spinBox_HTML_table_border_width.setMaximumSize(QSize(50, 16777215))
        self.spinBox_HTML_table_border_width.setMaximum(100)
        self.spinBox_HTML_table_border_width.setValue(0)

        self.gridLayout_11.addWidget(self.spinBox_HTML_table_border_width, 1, 1, 1, 1)

        self.horizontalSpacer_34 = QSpacerItem(198, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_11.addItem(self.horizontalSpacer_34, 1, 2, 1, 1)

        self.label_HTML_table_font_size = QLabel(self.groupBox_HTML_table_border_width_table_options)
        self.label_HTML_table_font_size.setObjectName(u"label_HTML_table_font_size")

        self.gridLayout_11.addWidget(self.label_HTML_table_font_size, 2, 0, 1, 1)

        self.spinBox_HTML_table_font_size = QSpinBox(self.groupBox_HTML_table_border_width_table_options)
        self.spinBox_HTML_table_font_size.setObjectName(u"spinBox_HTML_table_font_size")
        sizePolicy.setHeightForWidth(self.spinBox_HTML_table_font_size.sizePolicy().hasHeightForWidth())
        self.spinBox_HTML_table_font_size.setSizePolicy(sizePolicy)
        self.spinBox_HTML_table_font_size.setMinimumSize(QSize(50, 0))
        self.spinBox_HTML_table_font_size.setMaximumSize(QSize(50, 16777215))
        self.spinBox_HTML_table_font_size.setMaximum(100)
        self.spinBox_HTML_table_font_size.setValue(8)

        self.gridLayout_11.addWidget(self.spinBox_HTML_table_font_size, 2, 1, 1, 1)

        self.horizontalSpacer_33 = QSpacerItem(198, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_11.addItem(self.horizontalSpacer_33, 2, 2, 1, 1)

        self.label_HTML_table_font_family = QLabel(self.groupBox_HTML_table_border_width_table_options)
        self.label_HTML_table_font_family.setObjectName(u"label_HTML_table_font_family")

        self.gridLayout_11.addWidget(self.label_HTML_table_font_family, 3, 0, 1, 1)

        self.comboBox_HTML_table_font_family = QComboBox(self.groupBox_HTML_table_border_width_table_options)
        self.comboBox_HTML_table_font_family.setObjectName(u"comboBox_HTML_table_font_family")

        self.gridLayout_11.addWidget(self.comboBox_HTML_table_font_family, 3, 1, 1, 2)


        self.gridLayout_12.addWidget(self.groupBox_HTML_table_border_width_table_options, 0, 1, 1, 1)

        self.groupBox_HTML_generation = QGroupBox(self.HTML)
        self.groupBox_HTML_generation.setObjectName(u"groupBox_HTML_generation")
        self.gridLayout_21 = QGridLayout(self.groupBox_HTML_generation)
        self.gridLayout_21.setObjectName(u"gridLayout_21")
        self.checkBox_HTML_paragraph_numbers = QCheckBox(self.groupBox_HTML_generation)
        self.checkBox_HTML_paragraph_numbers.setObjectName(u"checkBox_HTML_paragraph_numbers")

        self.gridLayout_21.addWidget(self.checkBox_HTML_paragraph_numbers, 0, 0, 1, 2)

        self.horizontalSpacer_35 = QSpacerItem(638, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_21.addItem(self.horizontalSpacer_35, 0, 2, 1, 3)

        self.checkBox_HTML_generate_TOC = QCheckBox(self.groupBox_HTML_generation)
        self.checkBox_HTML_generate_TOC.setObjectName(u"checkBox_HTML_generate_TOC")

        self.gridLayout_21.addWidget(self.checkBox_HTML_generate_TOC, 1, 0, 1, 3)

        self.horizontalSpacer_36 = QSpacerItem(536, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_21.addItem(self.horizontalSpacer_36, 1, 3, 1, 2)

        self.label_HTML_body_tag = QLabel(self.groupBox_HTML_generation)
        self.label_HTML_body_tag.setObjectName(u"label_HTML_body_tag")

        self.gridLayout_21.addWidget(self.label_HTML_body_tag, 2, 0, 1, 1)

        self.textEdit_HTML_body_tag = QTextEdit(self.groupBox_HTML_generation)
        self.textEdit_HTML_body_tag.setObjectName(u"textEdit_HTML_body_tag")
        self.textEdit_HTML_body_tag.setMinimumSize(QSize(300, 0))
        self.textEdit_HTML_body_tag.setMaximumSize(QSize(300, 16777215))

        self.gridLayout_21.addWidget(self.textEdit_HTML_body_tag, 2, 1, 2, 3)

        self.horizontalSpacer_44 = QSpacerItem(434, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_21.addItem(self.horizontalSpacer_44, 3, 4, 1, 1)

        self.label_HTML_title = QLabel(self.groupBox_HTML_generation)
        self.label_HTML_title.setObjectName(u"label_HTML_title")

        self.gridLayout_21.addWidget(self.label_HTML_title, 4, 0, 1, 1)

        self.lineEdit_HTML_title = QLineEdit(self.groupBox_HTML_generation)
        self.lineEdit_HTML_title.setObjectName(u"lineEdit_HTML_title")
        self.lineEdit_HTML_title.setMinimumSize(QSize(300, 0))
        self.lineEdit_HTML_title.setMaximumSize(QSize(300, 16777215))

        self.gridLayout_21.addWidget(self.lineEdit_HTML_title, 4, 1, 1, 3)

        self.horizontalSpacer_45 = QSpacerItem(434, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout_21.addItem(self.horizontalSpacer_45, 4, 4, 1, 1)


        self.gridLayout_12.addWidget(self.groupBox_HTML_generation, 1, 0, 1, 2)

        self.tabWidget.addTab(self.HTML, "")

        self.gridLayout_20.addWidget(self.tabWidget, 0, 0, 4, 1)

        self.verticalSpacer = QSpacerItem(20, 356, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout_20.addItem(self.verticalSpacer, 3, 1, 1, 1)

        self.pushButton_OK = QPushButton(FilePrintPreferences)
        self.pushButton_OK.setObjectName(u"pushButton_OK")

        self.gridLayout_20.addWidget(self.pushButton_OK, 0, 1, 1, 1)

        self.pushButton_help = QPushButton(FilePrintPreferences)
        self.pushButton_help.setObjectName(u"pushButton_help")

        self.gridLayout_20.addWidget(self.pushButton_help, 2, 1, 1, 1)


        self.retranslateUi(FilePrintPreferences)
        self.pushButton_OK.clicked.connect(FilePrintPreferences.set_print_preferences)
        self.pushButton_cancel.clicked.connect(FilePrintPreferences.reject)
        self.pushButton_help.clicked.connect(FilePrintPreferences.help)

        self.tabWidget.setCurrentIndex(0)
        self.comboBox_A2M_graph_background_color.setCurrentIndex(-1)


        QMetaObject.connectSlotsByName(FilePrintPreferences)
    # setupUi

    def retranslateUi(self, FilePrintPreferences):
        FilePrintPreferences.setWindowTitle(QCoreApplication.translate("FilePrintPreferences", u"Print Preferences", None))
        self.pushButton_cancel.setText(QCoreApplication.translate("FilePrintPreferences", u"Cancel", None))
#if QT_CONFIG(shortcut)
        self.pushButton_cancel.setShortcut(QCoreApplication.translate("FilePrintPreferences", u"Esc", None))
#endif // QT_CONFIG(shortcut)
        self.groupBox_general.setTitle(QCoreApplication.translate("FilePrintPreferences", u"General", None))
        self.label_font_family.setText(QCoreApplication.translate("FilePrintPreferences", u"Font Family", None))
        self.checkBox_black_white.setText(QCoreApplication.translate("FilePrintPreferences", u"Black/White", None))
        self.label_font_incr.setText(QCoreApplication.translate("FilePrintPreferences", u"Font Incr", None))
        self.checkBox_para_numbering.setText(QCoreApplication.translate("FilePrintPreferences", u"Para Numbering", None))
        self.label_font_size.setText(QCoreApplication.translate("FilePrintPreferences", u"Font Size", None))
        self.groupBox_page_setup.setTitle(QCoreApplication.translate("FilePrintPreferences", u"Page Setup", None))
        self.label_margin_left.setText(QCoreApplication.translate("FilePrintPreferences", u"Left Margin", None))
        self.label_margin_top.setText(QCoreApplication.translate("FilePrintPreferences", u"Top Margin", None))
        self.label_margin_right.setText(QCoreApplication.translate("FilePrintPreferences", u"Right Margin", None))
        self.label_margin_bottom.setText(QCoreApplication.translate("FilePrintPreferences", u"Bottom Margin", None))
        self.checkBox_prompt_printer.setText(QCoreApplication.translate("FilePrintPreferences", u"Prompt Printer", None))
        self.label_page_header.setText(QCoreApplication.translate("FilePrintPreferences", u"Page Header", None))
        self.label_page_footer.setText(QCoreApplication.translate("FilePrintPreferences", u"Page Footer", None))
        self.groupBox_tables_graphics.setTitle(QCoreApplication.translate("FilePrintPreferences", u"Tables and Graphics", None))
        self.checkBox_table_color.setText(QCoreApplication.translate("FilePrintPreferences", u"Table Color", None))
        self.label_table_font_size.setText(QCoreApplication.translate("FilePrintPreferences", u"Table Font Size", None))
        self.checkBox_table_shading.setText(QCoreApplication.translate("FilePrintPreferences", u"Table Shading", None))
        self.label_table_font_family.setText(QCoreApplication.translate("FilePrintPreferences", u"Table Font Family", None))
        self.checkBox_table_break.setText(QCoreApplication.translate("FilePrintPreferences", u"Break In Tables", None))
        self.label_table_border_width.setText(QCoreApplication.translate("FilePrintPreferences", u"Table Border Width", None))
        self.checkBox_table_new_page.setText(QCoreApplication.translate("FilePrintPreferences", u"New Page/Table", None))
        self.checkBox_graph_new_page.setText(QCoreApplication.translate("FilePrintPreferences", u"New Page/Graph", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.Printer), QCoreApplication.translate("FilePrintPreferences", u"Printer", None))
        self.groupBox_A2M_control_characters.setTitle(QCoreApplication.translate("FilePrintPreferences", u"Control Characters", None))
        self.label_A2M_escape_character.setText(QCoreApplication.translate("FilePrintPreferences", u"Escape Character", None))
        self.lineEdit_A2M_escape_character.setText(QCoreApplication.translate("FilePrintPreferences", u"\\", None))
        self.label_A2M_define_character.setText(QCoreApplication.translate("FilePrintPreferences", u"Define Character", None))
        self.lineEdit_A2M_define_character.setText(QCoreApplication.translate("FilePrintPreferences", u"&", None))
        self.label_A2M_command_character.setText(QCoreApplication.translate("FilePrintPreferences", u"Command Character", None))
        self.lineEdit_A2M_command_character.setText(QCoreApplication.translate("FilePrintPreferences", u".", None))
        self.label_A2M_cell_separator.setText(QCoreApplication.translate("FilePrintPreferences", u"Cell Separator", None))
        self.lineEdit_A2M_cell_separator.setText(QCoreApplication.translate("FilePrintPreferences", u"@", None))
        self.groupBox_A2M_startup_options.setTitle(QCoreApplication.translate("FilePrintPreferences", u"Startup Options", None))
        self.label_A2M_default_paragraph.setText(QCoreApplication.translate("FilePrintPreferences", u"Default Paragraph", None))
        self.lineEdit_A2M_default_paragraph.setText(QCoreApplication.translate("FilePrintPreferences", u"par_1", None))
        self.checkBox_A2M_preserve_spaces.setText(QCoreApplication.translate("FilePrintPreferences", u"Preserve Spaces", None))
        self.checkBox_A2M_preserve_linefeed.setText(QCoreApplication.translate("FilePrintPreferences", u"Preserve Linefeed", None))
        self.checkBox_A2M_append_to_existing_file.setText(QCoreApplication.translate("FilePrintPreferences", u"Append To Existing File", None))
        self.groupBox_A2M_graphs.setTitle(QCoreApplication.translate("FilePrintPreferences", u"Graphs", None))
        self.label_A2M_graph_width.setText(QCoreApplication.translate("FilePrintPreferences", u"Width (mm)", None))
        self.label_A2M_graph_background_color.setText(QCoreApplication.translate("FilePrintPreferences", u"Background Color", None))
        self.label_A2M_graph_box.setText(QCoreApplication.translate("FilePrintPreferences", u"Box", None))
        self.label_A2M_graph_height.setText(QCoreApplication.translate("FilePrintPreferences", u"Height (mm)", None))
        self.label_A2M_graph_background_brush.setText(QCoreApplication.translate("FilePrintPreferences", u"Background Brush", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.A2M), QCoreApplication.translate("FilePrintPreferences", u"A2M", None))
        self.groupBox_MIF_font.setTitle(QCoreApplication.translate("FilePrintPreferences", u"Font", None))
        self.label_MIF_font_family.setText(QCoreApplication.translate("FilePrintPreferences", u"Font Family", None))
        self.label_MIF_font_size.setText(QCoreApplication.translate("FilePrintPreferences", u"Font Size", None))
        self.label_MIF_font_incr.setText(QCoreApplication.translate("FilePrintPreferences", u"Font Incr", None))
        self.groupBox_MIF_miscellaneous.setTitle(QCoreApplication.translate("FilePrintPreferences", u"Miscellaneous", None))
        self.checkBox_MIF_image_ref_in_text.setText(QCoreApplication.translate("FilePrintPreferences", u"Image refs in text  ", None))
        self.groupBox_MIF_table_options.setTitle(QCoreApplication.translate("FilePrintPreferences", u"Table Options", None))
        self.checkBox_MIF_table_use_color.setText(QCoreApplication.translate("FilePrintPreferences", u"Use color", None))
        self.label_MIF_table_width.setText(QCoreApplication.translate("FilePrintPreferences", u"Table Width (mm)", None))
        self.checkBox_MIF_table_split_tables.setText(QCoreApplication.translate("FilePrintPreferences", u"Split tables", None))
        self.label_MIF_table_first_col_width.setText(QCoreApplication.translate("FilePrintPreferences", u"First Column Width", None))
        self.checkBox_MIF_table_outside_borders.setText(QCoreApplication.translate("FilePrintPreferences", u"Outside Borders", None))
        self.label_MIF_table_other_col_width.setText(QCoreApplication.translate("FilePrintPreferences", u"Other Columns Width", None))
        self.checkBox_MIF_table_vertical_lines.setText(QCoreApplication.translate("FilePrintPreferences", u"Vertical Lines", None))
        self.label_MIF_table_font_size.setText(QCoreApplication.translate("FilePrintPreferences", u"Table Font Size", None))
        self.checkBox_MIF_table_horizontal_lines.setText(QCoreApplication.translate("FilePrintPreferences", u"Horizontal Lines", None))
        self.label_MIF_table_font_family.setText(QCoreApplication.translate("FilePrintPreferences", u"Table Font Family", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.MIF), QCoreApplication.translate("FilePrintPreferences", u"MIF", None))
        self.groupBox_general_2.setTitle(QCoreApplication.translate("FilePrintPreferences", u"General", None))
        self.checkBox_RTF_paragraph_numbers.setText(QCoreApplication.translate("FilePrintPreferences", u"Paragraph Numbers", None))
        self.label_RTF_font_size.setText(QCoreApplication.translate("FilePrintPreferences", u"Font Size", None))
        self.label_RTF_font_incr.setText(QCoreApplication.translate("FilePrintPreferences", u"Font Incr", None))
        self.label_RTF_font_family.setText(QCoreApplication.translate("FilePrintPreferences", u"Font Family", None))
        self.groupBox_RTF_table_options.setTitle(QCoreApplication.translate("FilePrintPreferences", u"Table Options", None))
        self.checkBox_RTF_table_use_color.setText(QCoreApplication.translate("FilePrintPreferences", u"Use color", None))
        self.label_RTF_table_width.setText(QCoreApplication.translate("FilePrintPreferences", u"Table Width (mm)", None))
        self.checkBox_RTF_table_split_tables.setText(QCoreApplication.translate("FilePrintPreferences", u"Split tables", None))
        self.label_RTF_table_first_col_width.setText(QCoreApplication.translate("FilePrintPreferences", u"First Column Width", None))
        self.checkBox_RTF_table_outside_borders.setText(QCoreApplication.translate("FilePrintPreferences", u"Outside Borders", None))
        self.label_RTF_table_other_col_width.setText(QCoreApplication.translate("FilePrintPreferences", u"Other Columns Width", None))
        self.checkBox_RTF_table_vertical_lines.setText(QCoreApplication.translate("FilePrintPreferences", u"Vertical Lines", None))
        self.label_RTF_table_font_size.setText(QCoreApplication.translate("FilePrintPreferences", u"Table Font Size", None))
        self.checkBox_RTF_table_horizontal_lines.setText(QCoreApplication.translate("FilePrintPreferences", u"Horizontal Lines", None))
        self.label_RTF_table_font_family.setText(QCoreApplication.translate("FilePrintPreferences", u"Table Font Family", None))
        self.groupBox_RTF_help_generation.setTitle(QCoreApplication.translate("FilePrintPreferences", u"Help Generation", None))
        self.label_RTF_help_title.setText(QCoreApplication.translate("FilePrintPreferences", u"Help Title", None))
        self.label_RTF_copyright.setText(QCoreApplication.translate("FilePrintPreferences", u"Copyright", None))
        self.checkBox_RTF_prepare_for_HCW.setText(QCoreApplication.translate("FilePrintPreferences", u"Prepare For HCW", None))
        self.checkBox_RTF_compress_help.setText(QCoreApplication.translate("FilePrintPreferences", u"Compress Help", None))
        self.checkBox_RTF_generate_TOC.setText(QCoreApplication.translate("FilePrintPreferences", u"Generate Table Of Content", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.RTF), QCoreApplication.translate("FilePrintPreferences", u"RTF", None))
        self.groupBox_HTML_font.setTitle(QCoreApplication.translate("FilePrintPreferences", u"Font", None))
        self.label_HTML_font_size.setText(QCoreApplication.translate("FilePrintPreferences", u"Font Size", None))
        self.label_HTML_font_incr.setText(QCoreApplication.translate("FilePrintPreferences", u"Font Incr", None))
        self.label_HTML_font_family.setText(QCoreApplication.translate("FilePrintPreferences", u"Font Family", None))
        self.groupBox_HTML_table_border_width_table_options.setTitle(QCoreApplication.translate("FilePrintPreferences", u"Table Options", None))
        self.checkBox_HTML_table_use_color.setText(QCoreApplication.translate("FilePrintPreferences", u"Use color", None))
        self.label_HTML_table_border_width.setText(QCoreApplication.translate("FilePrintPreferences", u"Border Width", None))
        self.label_HTML_table_font_size.setText(QCoreApplication.translate("FilePrintPreferences", u"Table Font Size", None))
        self.label_HTML_table_font_family.setText(QCoreApplication.translate("FilePrintPreferences", u"Table Font Family", None))
        self.groupBox_HTML_generation.setTitle(QCoreApplication.translate("FilePrintPreferences", u"HTML Generation", None))
        self.checkBox_HTML_paragraph_numbers.setText(QCoreApplication.translate("FilePrintPreferences", u"Paragraph Numbering", None))
        self.checkBox_HTML_generate_TOC.setText(QCoreApplication.translate("FilePrintPreferences", u"Generate Table Of Content", None))
        self.label_HTML_body_tag.setText(QCoreApplication.translate("FilePrintPreferences", u"Body Tag", None))
        self.label_HTML_title.setText(QCoreApplication.translate("FilePrintPreferences", u"Title", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.HTML), QCoreApplication.translate("FilePrintPreferences", u"HTML", None))
        self.pushButton_OK.setText(QCoreApplication.translate("FilePrintPreferences", u"OK", None))
#if QT_CONFIG(shortcut)
        self.pushButton_OK.setShortcut(QCoreApplication.translate("FilePrintPreferences", u"F10", None))
#endif // QT_CONFIG(shortcut)
        self.pushButton_help.setText(QCoreApplication.translate("FilePrintPreferences", u"Help", None))
#if QT_CONFIG(shortcut)
        self.pushButton_help.setShortcut(QCoreApplication.translate("FilePrintPreferences", u"F1", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi

