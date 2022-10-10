#pragma once

#include <QWidget>
#include <QList>
#include <QSettings>
#include <QMessageBox>
#include <QMainWindow>

#include "ui_main_window.h"

#include "utils.h"

#include "menu/file/file_import_comments.h"
#include "menu/file/file_import_variables.h"
#include "menu/file/file_export.h"

#include "menu/workspace/workspace_load.h"
#include "menu/workspace/workspace_save.h"
#include "menu/workspace/workspace_clear.h"
#include "menu/workspace/workspace_high_to_low.h"
#include "menu/workspace/workspace_low_to_high.h"

#include "menu/compute/compute_identities.h"
#include "menu/compute/compute_simulation.h"
#include "menu/compute/compute_model.h"
#include "menu/compute/compute_scc_decomposition.h"
#include "menu/compute/compute_scc_simulation.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
	Q_OBJECT

private:
	std::shared_ptr<QString> settings_filepath;
	QSettings* settings;

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

protected:
	void closeEvent(QCloseEvent* event) override;
	void check_vars_sample();

public slots:
	// File Menu
	void open_import_comments_dialog();
	void open_import_variables_dialog();
	void open_export_dialog();

	// Workspace Menu
	void open_load_workspace_dialog();
	void open_save_workspace_dialog();
	void open_clear_workspace_dialog();
	void open_high_to_low_dialog();
	void open_low_to_high_dialog();

	// Compute Menu
	void open_compute_identities_dialog();
	void open_compute_simulation_dialog();
	void open_compute_model_dialog();
	void open_compute_scc_decomposition_dialog();
	void open_compute_scc_simulation_dialog();

	// Help Menu
	void about();
	void open_release_notes();
	void open_iode_home();
	void open_iode_manual();
};
