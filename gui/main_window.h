#pragma once

#include <QWidget>
#include <QMainWindow>

#include "ui_main_window.h"

#include "utils.h"

#include "iode_objs/models/comments_model.h"
#include "iode_objs/models/equations_model.h"
#include "iode_objs/models/identities_model.h"
#include "iode_objs/models/lists_model.h"
#include "iode_objs/models/scalars_model.h"
#include "iode_objs/models/tables_model.h"
#include "iode_objs/models/variables_model.h"

#include "menu/file/file_print_preferences.h"

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

	CommentsModel* commentsModel;
	EquationsModel* equationsModel;
	IdentitiesModel* identitiesModel;
	ListsModel* listsModel;
	ScalarsModel* scalarsModel;
	TablesModel* tablesModel;
	VariablesModel* variablesModel;

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

private:
	void resetFilter();
	void viewTab(int index = -1);

protected:
	void closeEvent(QCloseEvent* event) override;
	void check_vars_sample();

public slots:

	void open_print_setup_dialog();

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
