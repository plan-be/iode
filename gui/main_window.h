#pragma once

#include <QDir>
#include <QList>
#include <QWidget>
#include <QAction>
#include <QVector>
#include <QSettings>
#include <QShortcut>
#include <QCompleter>
#include <QMessageBox>
#include <QStringList>
#include <QMainWindow>
#include <QStringListModel>
#include <QFileIconProvider>

#include "ui_main_window.h"
#include "main_window_abstract.h"

#include "utils.h"

#include "iode_objs/edit/edit_vars_sample.h"

#include "text_edit/completer.h"

#include "print/file_print_preferences.h"

#include "menu/file/file_import_comments.h"
#include "menu/file/file_import_variables.h"
#include "menu/file/file_export.h"
#include "menu/file/file_settings.h"

#include "menu/file/file_new_project.h"

#include "menu/workspace/workspace_copy_into.h"
#include "menu/workspace/workspace_merge_into.h"
#include "menu/workspace/workspace_describe.h"
#include "menu/workspace/workspace_extrapolate_variables.h"
#include "menu/workspace/workspace_high_to_low.h"
#include "menu/workspace/workspace_low_to_high.h"
#include "menu/workspace/workspace_seasonal_adjustment.h"
#include "menu/workspace/workspace_trend_correction.h"

#include "menu/data/sort_expand_list.h"
#include "menu/data/list_calculus.h"
#include "menu/data/search_text.h"
#include "menu/data/scan_objects.h"
#include "menu/data/file_contents.h"
#include "menu/data/file_compare.h"
#include "menu/data/edit_tables.h"
#include "menu/data/duplicate.h"

#include "menu/compute/compute_identities.h"
#include "menu/compute/compute_simulation.h"
#include "menu/compute/compute_model.h"
#include "menu/compute/compute_scc_decomposition.h"
#include "menu/compute/compute_scc_simulation.h"

#include "menu/print_graph/print_tables.h"
#include "menu/print_graph/print_variables.h"
#include "menu/print_graph/graph_tables.h"
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
class MainWindow : public MainWindowAbstract, public Ui::MainWindow
{
	Q_OBJECT

	bool windowsPrinter;
    std::string outfile;

private:
	QSettings* user_settings;		///< to store user preferences and recently opened projects

	QString projectPath;
	const int MAX_RECENT_PROJECTS = 30;
	QStringList recentProjects;
	QString fontFamily;

	QShortcut* fullScreenShortcut;

public:
	MainWindow(QWidget *parent = Q_NULLPTR, const QVector<QString>& files_to_load = {});
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

	QTextEdit* getOutput() override
	{
		return textEdit_output;
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
	// File Menu
	void create_new_project();
	void open_project();
	void open_recent_project();
	void save_current_tab();
	void save_all_tabs();
	void open_import_comments_dialog();
	void open_import_variables_dialog();
	void open_export_dialog();
	void open_settings();

	// Workspace Menu
	void clear_workspace();
	void open_copy_into_workspace_dialog();
	void open_merge_into_workspace_dialog();
	void open_change_workspace_descriptions_dialog();
	void open_change_variables_sample_dialog();
	void open_extrapolate_variables_dialog();
	void open_high_to_low_dialog();
	void open_low_to_high_dialog();
	void open_seasonal_adjustment_dialog();
	void open_trend_correction_dialog();

	// Data Menu
	void open_sort_and_calculus_dialog();
	void open_list_calculus_dialog();
	void open_file_contents_dialog();
	void open_file_compare_dialog();
	void open_search_text_dialog();
	void open_scan_objects_dialog();
	void open_edit_tables_variables_dialog();
	void open_duplicate_objects_dialog();

	// Compute Menu
	void open_compute_identities_dialog();
	void open_compute_simulation_dialog();
	void open_compute_model_dialog();
	void open_compute_scc_decomposition_dialog();
	void open_compute_scc_simulation_dialog();

	// Print/Graph Menu
	void open_print_tables_dialog();
	void open_print_variables_dialog();
	void open_print_objects_definitions_dialog();
	void open_graphs_tables_dialog();
	void open_graphs_variables_dialog(); 
	void open_graphs_variables_dialog_from_vars_view(
		const QList<QString>& variableNames, const QString& from, const QString& to) override;

	// Help Menu
	void about();
	void open_release_notes();
	void open_iode_home();
	void open_python_api();
	void open_iode_manual();
	void display_keyboard_shortcuts();

	// Main Window
	void computeHash(const bool value) override
	{
		tabWidget_IODE_objs->computeHash(value);
	}

	void update_tab_and_completer(const int iodeType = -1) override;
};
