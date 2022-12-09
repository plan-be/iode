#pragma once

#include <QWidget>
#include <QDir>
#include <QList>
#include <QAction>
#include <QSettings>
#include <QCompleter>
#include <QStringList>
#include <QStringListModel>
#include <QMessageBox>
#include <QFileIconProvider>
#include <QMainWindow>

#include "ui_main_window.h"

#include "utils.h"

#include "iode_objs/edit/edit_vars_sample.h"

#include "plot/plot.h"

#include "custom_widgets/qiode_completer.h"

#include "menu/file/file_import_comments.h"
#include "menu/file/file_import_variables.h"
#include "menu/file/file_export.h"

#include "menu/file/file_new_project.h"

#include "menu/workspace/workspace_copy_into.h"
#include "menu/workspace/workspace_merge_into.h"
#include "menu/workspace/workspace_describe.h"
#include "menu/workspace/workspace_high_to_low.h"
#include "menu/workspace/workspace_low_to_high.h"

#include "menu/compute/compute_identities.h"
#include "menu/compute/compute_simulation.h"
#include "menu/compute/compute_model.h"
#include "menu/compute/compute_scc_decomposition.h"
#include "menu/compute/compute_scc_simulation.h"

#include "menu/print_graph/graph_variables.h"


/**
 * @brief (...)
 * 
 *        Implemented features:
 * 
 *        - CTRL + ALT + N creates a new project.
 *        - CTRL + O opens an existing project.
 *        - CTRL + ALT + S saves current project to another directory.
 *        - CTRL + S saves the current tab content.
 *        - CTRL + SHIFT + S saves all tabs content.
 *        - CTRL + SHIFT + D clears the whole workspace. 
 * 
 */
class MainWindow : public QMainWindow, public Ui::MainWindow
{
	Q_OBJECT

private:
	std::shared_ptr<QString> project_settings_filepath;
	QSettings* project_settings;	///< to store per project (= directory) settings
	QSettings* user_settings;		///< to store user preferences and recently opened projects

	QString projectPath;
	const int MAX_RECENT_PROJECTS = 30;
	QStringList recentProjects;
	QString fontFamily;

	QVector<QIodePlotDialog*> plots;

	std::shared_ptr<QIodeCompleter> completer;

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

	/**
	 * @brief update the project path and the content of the File Explorer.
	 *        Ask to save workspace and content of all other tabs before to switch from project directory. 
	 *        If the new project directory contains a project settings file (.ini), reopen tabs listed in 
	 *        the project settings file.
	 * 
	 * @param dirPath QString path to the directory to open as IODE project root.
	 * 
	 * @return bool whether or not the IODE project root has been updated.
	 */
	bool openDirectory(const QString& dirPath);

	/**
	 * @brief displays output
	 * 
	 */
	void display_output(const QString& output)
	{
		textEdit_output->append(output);
	}

protected:
	void closeEvent(QCloseEvent* event) override;
	void check_vars_sample();

private:
	/**
	 * @brief prepare the list of recently opened folders in Menu > Recent Projects.
	 * 
	 */
	void buildRecentProjectsMenu();

	/**
	 * @brief add the path to a project directory to the list of recent opened project dirs.
	 * 
	 * @param projectDir absolute path the project directory
	 */
	void addProjectPathToList(const QDir& projectDir);

	/**
	 * @brief ask the user if (s)he wants to save all tabs content 
	 *        (before to switch from project directory or when closing the GUI).
	 * 
	 * @return QMessageBox::StandardButton Yes, No or Discard.
	 */
	QMessageBox::StandardButton askSaveAllTabs();

public slots:
	void appendPlot(QIodePlotDialog* plotDialog);

	// File Menu
	void createNewProject();
	void open_project();
	void open_recent_project();
	void saveCurrentTab();
	void saveAllTabs();
	void open_import_comments_dialog();
	void open_import_variables_dialog();
	void open_export_dialog();

	// Workspace Menu
	void clear_workspace();
	void open_copy_into_workspace_dialog();
	void open_merge_into_workspace_dialog();
	void open_change_workspace_descriptions_dialog();
	void open_change_variables_sample_dialog();
	void open_high_to_low_dialog();
	void open_low_to_high_dialog();

	// Compute Menu
	void open_compute_identities_dialog();
	void open_compute_simulation_dialog();
	void open_compute_model_dialog();
	void open_compute_scc_decomposition_dialog();
	void open_compute_scc_simulation_dialog();

	// Print/Graph Menu
	void open_graphs_variables_dialog(); 
	void open_graphs_variables_dialog_from_vars_view(
		const QList<QString>& variableNames, const QString& from, const QString& to);

	// Help Menu
	void about();
	void open_release_notes();
	void open_iode_home();
	void open_iode_manual();
	void display_keyboard_shortcuts();

	// Main Window
	void update_current_tab();
};
