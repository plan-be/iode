#pragma once

#include <QObject>
#include <QString>
#include <QUrl>
#include <QDir>
#include <QMap>
#include <QDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QSettings>
#include <QTabWidget>
#include <QMainWindow>
#include <QCoreApplication>
#include <QMainWindow>

#include <exception>
#include <stdexcept>

#include "utils.h"

#include "ui_main_window.h"

#include "./views/comments_view.h"
#include "./models/comments_model.h"

#include "menu/file/file_import.h"
#include "menu/file/file_export.h"
#include "menu/file/file_load_settings.h"
#include "menu/file/file_save_settings.h"

#include "menu/workspace/workspace_load.h"
#include "menu/workspace/workspace_save.h"
#include "menu/workspace/workspace_clear.h"
#include "menu/workspace/workspace_copy_into.h"
#include "menu/workspace/workspace_merge_into.h"
#include "menu/workspace/workspace_change_descriptions.h"
#include "menu/workspace/workspace_change_variables_sample.h"
#include "menu/workspace/workspace_extrapolate_variables.h"
#include "menu/workspace/workspace_high_to_low.h"
#include "menu/workspace/workspace_low_to_high.h"
#include "menu/workspace/workspace_seasonal_adjustment.h"
#include "menu/workspace/workspace_trend_correction.h"

#include "menu/data/data_object_editor.h"
#include "menu/data/data_duplicate_objects.h"

#include "menu/compute/compute_simulation.h"
#include "menu/compute/compute_model.h"
#include "menu/compute/compute_scc_decomposition.h"
#include "menu/compute/compute_scc_simulation.h"
#include "menu/compute/compute_identities.h"

#include "menu/print/print_tables.h"
#include "menu/print/print_variables.h"
#include "menu/print/print_objects_definitions.h"
#include "menu/print/print_graphs_tables.h"
#include "menu/print/print_graphs_variables.h"
#include "menu/print/print_A2M_file.h"
#include "menu/print/print_report.h"

#include "menu/report/report_edit.h"
#include "menu/report/report_execute.h"
#include "menu/report/report_command_line.h"


class MainWindow : public QMainWindow
{
	Q_OBJECT

	QSettings* settings;
	Ui::MainWindow* window;
	QTabWidget* tabs;


	// Comments
	CommentsView* commentsView;
	CommentsModel* commentsModel;

public:
	MainWindow();
	~MainWindow();

private:
	void init_iode_api();
	void end_iode_api();
	void updateCurrentTab(int index=-1);

protected:
	void closeEvent(QCloseEvent* event) override;

public slots:
	void about();
	void open_release_notes();
	void open_iode_home();
	void open_iode_manual();

	void open_import_file_dialog();
	void open_export_file_dialog();
	void open_load_settings_dialog();
	void open_save_settings_dialog();
	void open_reset_settings_dialog();

	void open_load_workspace_dialog();
	void open_save_workspace_dialog();
	void open_clear_workspace_dialog();
	void open_copy_into_workspace_dialog();
	void open_merge_into_workspace_dialog();
	void open_change_workspace_descriptions_dialog();
	void open_change_variables_sample_dialog();
	void open_extrapolate_variables_dialog();
	void open_high_to_low_dialog();
	void open_low_to_high_dialog();
	void open_seasonal_adjustment_dialog();
	void open_trend_correction_dialog();

	void open_object_editor_dialog();
	void open_duplicate_objects_dialog();

	void open_compute_simulation_dialog();
	void open_compute_model_dialog();
	void open_compute_scc_decomposition_dialog();
	void open_compute_scc_simulation_dialog();
	void open_compute_identities_dialog();

	void open_print_tables_dialog();
	void open_print_variables_dialog();
	void open_print_objects_definitions_dialog();
	void open_graphs_tables_dialog();
	void open_graphs_variables_dialog();
	void open_print_A2M_file_dialog();
	void open_print_report_dialog();

	void open_edit_report_dialog();
	void open_execute_report_dialog();
	void open_execute_report_command_line_dialog();

};
