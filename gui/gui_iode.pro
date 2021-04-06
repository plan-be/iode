# CONFIG: https://doc.qt.io/qt-5/qmake-variable-reference.html#config

# The target is a Qt application or library and requires the Qt library and header files. 
# The proper include and library paths for the Qt library will automatically be added to the project
CONFIG += qt
# The project is prepared to be built in both debug and release modes.
CONFIG += debug_and_release
# Enables support for the use of precompiled headers in projects.
CONFIG += precompile_header
# The compiler should output as many warnings as possible.
CONFIG += warn_on

# https://doc.qt.io/qt-5/qmake-variable-reference.html#template
TEMPLATE = app
# TEMPLATE = lib

# directory in which the target executable is placed
DESTDIR = ./exe

# name of the executable for the application
TARGET = gui_iode

# list of used Qt modules
QT += core
QT += gui
QT += widgets

# specify the compilation DEFINES variables
DEFINES += WINDOWS_IGNORE_PACKING_MISMATCH
DEFINES += VC 
DEFINES += DOS 
DEFINES += DOSWIN 
DEFINES += DOSW32 
DEFINES += REALD 
DEFINES += WINDOWS 
DEFINES += IODEWIN 
DEFINES += NOEMS 
DEFINES += NOEMF

# set root project directory as starting dir for all includes
INCLUDEPATH += ..
INCLUDEPATH += ../api
INCLUDEPATH += ../scr4

# specify libs to be linked with the application
LIBS += -L"../api/vc32" -liodeapi
LIBS += -L"../scr4/vc32" -ls4iode
LIBS += ws2_32.lib
LIBS += gdi32.lib
LIBS += user32.lib
LIBS += advapi32.lib
LIBS += shell32.lib
LIBS += comdlg32.lib
LIBS += comctl32.lib 
LIBS += Winspool.lib
LIBS += legacy_stdio_definitions.lib
LIBS += odbc32.lib
LIBS += odbccp32.lib

# list of UI files for the application (created using Qt Designer).
# Main Window
FORMS += main_window.ui
# File menu
FORMS += menu/file/file_import.ui
FORMS += menu/file/file_export.ui
FORMS += menu/file/file_load_settings.ui
FORMS += menu/file/file_save_settings.ui
# Workspace menu
FORMS += menu/workspace/workspace_load.ui
FORMS += menu/workspace/workspace_save.ui
FORMS += menu/workspace/workspace_clear.ui
FORMS += menu/workspace/workspace_copy_into.ui
FORMS += menu/workspace/workspace_merge_into.ui
FORMS += menu/workspace/workspace_change_descriptions.ui
FORMS += menu/workspace/workspace_change_variables_sample.ui
FORMS += menu/workspace/workspace_extrapolate_variables.ui
FORMS += menu/workspace/workspace_high_to_low.ui
FORMS += menu/workspace/workspace_low_to_high.ui
FORMS += menu/workspace/workspace_seasonal_adjustment.ui
FORMS += menu/workspace/workspace_trend_correction.ui
# Data menu
FORMS += menu/data/data_object_editor.ui
FORMS += menu/data/data_duplicate_objects.ui
# Compute menu
FORMS += menu/compute/compute_simulation.ui
FORMS += menu/compute/compute_model.ui
FORMS += menu/compute/compute_scc_decomposition.ui
FORMS += menu/compute/compute_scc_simulation.ui
FORMS += menu/compute/compute_identities.ui
# Print/Graph menu
FORMS += menu/print/print_tables.ui
FORMS += menu/print/print_variables.ui
FORMS += menu/print/print_objects_definitions.ui
FORMS += menu/print/print_graphs_tables.ui
FORMS += menu/print/print_graphs_variables.ui
FORMS += menu/print/print_A2M_file.ui
FORMS += menu/print/print_report.ui
# Report menu
FORMS += menu/report/report_edit.ui
FORMS += menu/report/report_execute.ui
FORMS += menu/report/report_command_line.ui

# HEADERS
HEADERS += main_window.h
HEADERS += utils.h
HEADERS += bridge.h

# Custom widgets
HEADERS += custom_widgets/qsampleedit.h
HEADERS += custom_widgets/qfilechooser.h

# Models
HEADERS += models/comments_model.h

# Views
HEADERS += views/comments_view.h 

# File menu
HEADERS += menu/file/file_import.h
HEADERS += menu/file/file_export.h
HEADERS += menu/file/file_load_settings.h
HEADERS += menu/file/file_save_settings.h
# Workspace menu
HEADERS += menu/workspace/workspace_load.h
HEADERS += menu/workspace/workspace_save.h
HEADERS += menu/workspace/workspace_clear.h
HEADERS += menu/workspace/workspace_copy_into.h
HEADERS += menu/workspace/workspace_merge_into.h
HEADERS += menu/workspace/workspace_change_descriptions.h
HEADERS += menu/workspace/workspace_change_variables_sample.h
HEADERS += menu/workspace/workspace_extrapolate_variables.h
HEADERS += menu/workspace/workspace_high_to_low.h
HEADERS += menu/workspace/workspace_low_to_high.h
HEADERS += menu/workspace/workspace_seasonal_adjustment.h
HEADERS += menu/workspace/workspace_trend_correction.h
# Data menu
HEADERS += menu/data/data_object_editor.h
HEADERS += menu/data/data_duplicate_objects.h
# Compute menu
HEADERS += menu/compute/compute_simulation.h
HEADERS += menu/compute/compute_model.h
HEADERS += menu/compute/compute_scc_decomposition.h
HEADERS += menu/compute/compute_scc_simulation.h
HEADERS += menu/compute/compute_identities.h
# Print/Graph menu
HEADERS += menu/print/print_tables.h
HEADERS += menu/print/print_variables.h
HEADERS += menu/print/print_objects_definitions.h
HEADERS += menu/print/print_graphs_tables.h
HEADERS += menu/print/print_graphs_variables.h
HEADERS += menu/print/print_A2M_file.h
HEADERS += menu/print/print_report.h
# Report menu
HEADERS += menu/report/report_edit.h
HEADERS += menu/report/report_execute.h
HEADERS += menu/report/report_command_line.h


PRECOMPILED_HEADER += 

# SOURCES
# Legacy
SOURCES += legacy/k_errorv.c
SOURCES += legacy/s_iode.c
SOURCES += legacy/sb_gui.c

# Main
SOURCES += main.cpp
SOURCES += main_window.cpp

# Models
SOURCES += models/comments_model.cpp

# Views
SOURCES += views/comments_view.cpp

# File menu
SOURCES += menu/file/file_import.cpp
SOURCES += menu/file/file_export.cpp
SOURCES += menu/file/file_load_settings.cpp
SOURCES += menu/file/file_save_settings.cpp
# Workspace menu
SOURCES += menu/workspace/workspace_load.cpp
SOURCES += menu/workspace/workspace_save.cpp
SOURCES += menu/workspace/workspace_clear.cpp
SOURCES += menu/workspace/workspace_copy_into.cpp
SOURCES += menu/workspace/workspace_merge_into.cpp
SOURCES += menu/workspace/workspace_change_descriptions.cpp
SOURCES += menu/workspace/workspace_change_variables_sample.cpp
SOURCES += menu/workspace/workspace_extrapolate_variables.cpp
SOURCES += menu/workspace/workspace_high_to_low.cpp
SOURCES += menu/workspace/workspace_low_to_high.cpp
SOURCES += menu/workspace/workspace_seasonal_adjustment.cpp
SOURCES += menu/workspace/workspace_trend_correction.cpp
# Data menu
SOURCES += menu/data/data_object_editor.cpp
SOURCES += menu/data/data_duplicate_objects.cpp
# Compute menu
SOURCES += menu/compute/compute_simulation.cpp
SOURCES += menu/compute/compute_model.cpp
SOURCES += menu/compute/compute_scc_decomposition.cpp
SOURCES += menu/compute/compute_scc_simulation.cpp
SOURCES += menu/compute/compute_identities.cpp
# Print/Graph menu
SOURCES += menu/print/print_tables.cpp
SOURCES += menu/print/print_variables.cpp
SOURCES += menu/print/print_objects_definitions.cpp
SOURCES += menu/print/print_graphs_tables.cpp
SOURCES += menu/print/print_graphs_variables.cpp
SOURCES += menu/print/print_A2M_file.cpp
SOURCES += menu/print/print_report.cpp
# Report menu
SOURCES += menu/report/report_edit.cpp
SOURCES += menu/report/report_execute.cpp
SOURCES += menu/report/report_command_line.cpp
