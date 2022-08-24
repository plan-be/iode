#pragma once

#include <QWidget>
#include <QMainWindow>

#include "ui_main_window.h"

#include "utils.h"

#include "tabs/comments_model.h"
#include "tabs/equations_model.h"
#include "tabs/identities_model.h"
#include "tabs/lists_model.h"
#include "tabs/scalars_model.h"
#include "tabs/tables_model.h"
#include "tabs/variables_model.h"

#include "menu/workspace/workspace_load.h"
#include "menu/workspace/workspace_save.h"
#include "menu/workspace/workspace_clear.h"

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

public slots:
	// Workspace Menu
	void open_load_workspace_dialog();
	void open_save_workspace_dialog();
	void open_clear_workspace_dialog();

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
