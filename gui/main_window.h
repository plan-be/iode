#pragma once

#include <QMainWindow>
#include <QSettings>

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


class MainWindow : public QMainWindow, public Ui::MainWindow
{
	Q_OBJECT

private:
	QSettings* settings;

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
	void init_iode_api();
	void end_iode_api();
	void updateCurrentTab(int index = -1);

protected:
	void closeEvent(QCloseEvent* event) override;

public slots:
	// Workspace Menu
	void open_load_workspace_dialog();
	void open_save_workspace_dialog();

	// Help Menu
	void about();
	void open_release_notes();
	void open_iode_home();
	void open_iode_manual();
};
