#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QAction>
#include <QShortcut>
#include <QTabWidget>
#include <QFileSystemWatcher>

#include "utils.h"
#include "util/system_item.h"
#include "tabs/tab_text.h"
#include "tabs/tab_report.h"
#include "tabs/tab_abstract.h"

#include "iode_objs/models/comments_model.h"
#include "iode_objs/models/equations_model.h"
#include "iode_objs/models/identities_model.h"
#include "iode_objs/models/lists_model.h"
#include "iode_objs/models/scalars_model.h"
#include "iode_objs/models/tables_model.h"
#include "iode_objs/models/variables_model.h"

#include "iode_objs/views/comments_view.h"
#include "iode_objs/views/equations_view.h"
#include "iode_objs/views/identities_view.h"
#include "iode_objs/views/lists_view.h"
#include "iode_objs/views/scalars_view.h"
#include "iode_objs/views/tables_view.h"
#include "iode_objs/views/variables_view.h"


/**
 * @brief The present class represents the widget displaying tabs. 
 *        Tabs content can represents:
 * 
 *          - an IODE KDB,
 *          - an IODE report,
 *          - a text file.
 * 
 *        Implemented features:
 *   
 *        - Tab text = prefix (CMT, EQS, ..., REP, TXT) + filename of the loaded file (if any).
 *          If the tab represents an IODE KDB not (yet) linked to any file, 'filename' is empty.
 *        - The tab text include an * if modifications have been made but not saved yet.
 *        - Tab tooltip = absolute path to the loaded file (if any) or "Unsaved <IODE type> Database".
 *        - Only one tab can be open per IODE objects type (Comment, Equation, ..., Variable).
 *        - When switching from another project or quiting the GUI, the absolute paths to all loaded files 
 *          (= tooltips) are saved into a project settings file (with the .ini extension).
 *        - When switching to a previous project or starting the GUI, all previously open tabs are restored 
 *          (by reading list of files in the proejct settings file .ini).
 *        - Users are allowed to close tabs if they represent a report or a text files (i.e. not an IODE objects type).
 *        - Users are allowed to move tabs.
 *        - Open files with corresponding application (.xlsx with Excel, ...)
 *        - Reload file content if modified by another program. 
 *           -> CTRL + S on a tab saves its content (*)
 *           -> CTRL + W on a tab closes it if the tab does not represent an IODE database
 *           -> SHIFT + ALT + C copies the absolute filepath
 *           -> SHIFT + ALT + R reveals file in OS file explorer
 *           -> CTRL + SHIFT + S saves all tabs content (*)
 *           -> CTRL + D on a KDB tab clears the corresponding KDB
 *           -> CTRL + SHIFT + D clears the whole workspace (*) 
 *           -> ALT + [C|E|I|L|S|T|V] open the [Comments | Equations | Identities | Lists | Scalars | Tables | Variables] tab
 *           -> CTRL + [SHIFT] + TAB shift to the next [previous] tab
 * 
 *        (*) defined in main_window.ui
 * 
 */
class IodeAbstractTabWidget: public QTabWidget
{
    Q_OBJECT

protected:
    QStringList filesList;
    QFileSystemWatcher* fileSystemWatcher;

    IodeCompleter* completer;

    QTextEdit* output;

    QString projectDirPath;
    bool overwrite_all;
    bool discard_all;

    int indexContextMenu;
	QMenu* contextMenu;
    QAction* actionClose;
    QAction* actionClear;

    QAction* actionSplitH;
    QAction* actionSplitV;
    QAction* actionUnsplit;

    QShortcut* filepathShortcut;
    QShortcut* revealExplorerShortcut;
    QShortcut* nextTabShortcut;
    QShortcut* previousTabShortcut;
    QShortcut* clearShortcut;
    QShortcut* closeShortcut;

signals:
    void fileContentModified(const QString& filepath, const bool modified);
    void newObjsListDialog(QDialog* dialog);

public:
    IodeAbstractTabWidget(QWidget* parent = nullptr);
    ~IodeAbstractTabWidget();

    /**
     * @brief - reopen all tabs (files) that were open (*)
     *        - display the tab that was displayed (*)
     *        (*) the last time the user quitted the IODE gui.
     */
    virtual void loadSettings() = 0;
    
    /**
     * @brief dump in settings:
     *        - the list of open tabs.
     *        - the index of the currently displayed tab.
     *        
     * @note The list of open tabs is actually the list of the tooltip associated with each tab.
     *       The tooltip is either the path to the corresponding file or a default string in case of unsaved KDB.  
     */
    virtual void saveSettings() = 0;

    /**
     * @brief - Initializes settings object and openedFiles list.
     *        - Reload previously opened files.
     * 
     * TODO: merge setup() method with updateProjectDir()
     */
    virtual void setup() = 0;

	/**
	 * @brief load a file and set corresponding tab text (filename) and tooltip (full absolute path).
	 * 
	 * @param filepath QString path to the file to load.
     * @param displayTab bool whether or not displaying the corresponding tab when finished to load the file 
     *                        (used in loadSettings()).
     * @param forceOverwrite bool whether or not forcing overwrite of corresponding KDB (use in loadSetting()).
     * @param moveToPosition int new position of the corresponding tab. 
     *                           Only used when reloading tabs from a previous session.
     * @param forceAsText bool whether or not to force to open the corresponding file as text (if not an IODE 
     *                         database or report file)
     * 
     * @return int index of the corresponding tab. -1 if failed to load file.
	 */
	virtual int loadFile(const QString& filepath, const bool displayTab=true, const bool forceOverwrite=false, 
        const int moveToPosition=-1, const bool forceAsText=false) = 0;

    /**
     * @brief Reload a file if modified by an external program.
     * 
     * @param QString Filepath to the modified file.
     */
    void reloadFile(const QString& filepath);

    /**
     * @brief save current project to another (new) directory.
     * 
     * @param dir (new) directory to save the current project in.
     * @return bool whether or not all files have been saved, i.e. the user as not clicked on the Discard button.
     */
    bool saveProjectAs(QDir& newProjectDir);

    /**
     * @brief create a new tab corresponding to the newly open file.
     *        Called by loadFile().
     * 
     * @param fileType IodeFileType type of the loaded file (KDB file, .log, .ini, .txt, other).
     * @param fileInfo QFileInfo path to the loaded file.
     * @param forceAsText bool whether or not to force to open the corresponding file as text 
     *                         (if not an IODE database or report file)
     * @return int index of the new tab
     */
    int addNewTab(const IodeFileType fileType, const QFileInfo& fileInfo, const bool forceAsText=false);

    /**
     * @brief show tab corresponding to passed index. 
     *        Update current tab if index is -1.
     * 
     * @param index int index of the tab to show.
     */
	void showTab(int index = -1);

    /**
     * @brief To be called from executed command lines or IODE reports
     * 
     * @param iodeType 
     */
    virtual void openAddDialog(const IodeDatabaseType iodeType) = 0;

    /**
     * @brief To be called from executed command lines or IODE reports
     * 
     * @param iodeType 
     */
    virtual void openEditDialog(const IodeDatabaseType iodeType) = 0;

protected:
    virtual int getIodeObjTabIndex(const IodeDatabaseType iodeType) const = 0;

    /**
     * @brief update the paths (to directories and files) registered in the file system watcher.
     * 
     * @param projectDir QDir new project directory.      
     */
    void resetFileSystemWatcher(const QDir& projectDir);

    /**
     * @brief splits the content of the tab at position index into two views.
     * 
     * @param index 
     * @param orientation
     */
    void splitTab(const int index, Qt::Orientation orientation)
    {
        AbstractTextWidget* tabWidget = static_cast<AbstractTextWidget*>(this->widget(index));
        tabWidget->split(orientation);
    }

    /**
     * @brief unsplits the tab at position index.
     * 
     * @param index 
     * @param orientation
     */
    void unsplitTab(const int index)
    {
        AbstractTextWidget* tabWidget = static_cast<AbstractTextWidget*>(this->widget(index));
        tabWidget->unsplit();
    }

private:
    /**
     * @brief Create a context menu for the tab bar.
     * 
     */
    void setupContextMenu();

    /**
     * @brief Add an action to the context menu.
     * 
     * @param name Name of the action displayed in the context menu
     * @param tooltip 
     * @param shortcut 
     * @return QAction* 
     */
    QAction* addAction(const QString& name, const QString& tooltip, const QKeySequence& shortcut = QKeySequence())
    {
        QAction* action = new QAction(name, this);
        action->setToolTip(tooltip);
        action->setVisible(true);
        if(!shortcut.isEmpty()) action->setShortcut(shortcut);
        return action;
    }

    /**
     * @brief add tab for editing reports.
     * 
     * @param fileInfo QFileInfo path to the report file.
     * 
     * @return int index of the new tab.
     */
    int addReportTab(const QFileInfo& fileInfo);

    /**
     * @brief add tab for editing a text file (.log, .ini, .txt).
     * 
     * @param fileInfo QFileInfo path to the text file.
     * @param forced bool whether the file has been forced to be opened as text tab
     * 
     * @return int index of the new tab.
     */
    int addTextTab(const QFileInfo& fileInfo, const IodeFileType iodeFile, const bool forced = false);

    /**
     * @brief save the content of the tab at a given index.
     *        Ask for a filepath if the tab represents a KDB and the KDB is not linked to any file.
     * 
     * @param index int index of the tab to be saved.
     * 
     * @return bool whether or not the method has succeeded.
     */
    bool saveTabContent(const int index);

public slots:
    /**
     * @brief updates the tab name and tooltip and sends the fileContentModified signal to be 
     *        catched by the File Explorer.
     * 
     * @param filepath 
     * @param modified 
     */
    virtual void tabTextModified(const QString& filepath, const bool modified) = 0;

    /**
     * @brief updates the tab name and tooltip and sends the fileContentModified signal to be 
     *        catched by the File Explorer.
     * 
     * @param iodeType 
     * @param modified 
     */
    virtual void tabDatabaseModified(const IodeDatabaseType iodeType, const bool modified) = 0;

    /**
     * @brief popup a new dialog box containing the list of passed objects.
     * 
     * @param iodeType 
     * @param objNames 
     */
    virtual void showObjectsList(IodeDatabaseType iodeType, const QStringList& objNames) = 0;

    /**
     * @brief compute the hash of each database before/after executing 
     *        a report command or an iode report.
     *        If hash is different after report command/iode report, 
     *        a databaseModified signal is emited.
     * 
     * @param before whether the hash is calculated before or after the 
     *               execution of the report command/iode report.
     */
    virtual void computeHash(const bool before=false) = 0;

    /**
     * @brief Ask to save if modified then close the tab + remove path from the 
     *        system file watcher.
     * 
     * @param index 
     */
    void removeTab(const int index);

    /**
     * @brief show next tab.
     */
    void showNextTab()
    {
        int lastIndex = this->count() - 1;
        if(this->currentIndex() != lastIndex) this->setCurrentIndex(this->currentIndex() + 1);
    }

    /**
     * @brief show previous tab.
     * 
     */
    void showPreviousTab()
    {
        if(this->currentIndex() != 0) this->setCurrentIndex(this->currentIndex() - 1); 
    }

    /**
     * @brief Saves the content of the tab at position index where index is:
     *        - current tab index if method called using a shortcut
     *        - index defined by the context menu if called from a context menu
     * 
     * @param index
     */
    void saveTab();

    /**
     * @brief save all open tabs. 
     *        Ask for a filepath if the tab represents a KDB and the KDB is not linked to any file.
     * 
     * @return bool whether or not the method has succeeded. 
     */
    bool save_all_tabs();
    
    /**
     * @brief Update the path to the current project directory.
     *        Slot method called each time the project directory is changed.
     * 
     * @param projectDirPath Path to the root directory of the File Explorer  
     */
    virtual void updateProjectDir(const QString& projectDirPath) = 0;

    /**
     * @brief Method called when a user moves a file using the File Explorer.
     *        Update the name and tooltip af tab associated with the file if open.
     * 
     * @param oldFilepath Previous filepath
     * @param newFilepath New filepath
     */
    void fileMoved(const QString &oldFilepath, const QString &newFilepath);

    /**
     * @brief Method called when a user renames a file using the File Explorer.
     *        Update the name and tooltip af tab associated with the renamed file if open.
     * 
     * @param path Path to the parent directory
     * @param oldName Name of the file before renaming
     * @param newName Name of the file after renaming
     */
    void fileRenamed(const QString &path, const QString &oldName, const QString &newName)
    {
        QDir parentDir(path);
        QString oldFilepath = parentDir.absoluteFilePath(oldName);
        QString newFilepath = parentDir.absoluteFilePath(newName);
        fileMoved(oldFilepath, newFilepath);
    }

protected slots:
    /**
     * @brief build the list of filepaths associated to open tabs
     * 
     * @return QStringList list of open files (tabs)
     */
    QStringList buildFilesList()
    {
        filesList.clear();
        IodeAbstractWidget* tabWidget;
        for (int i=0; i < this->count(); i++)
        {
            tabWidget = static_cast<IodeAbstractWidget*>(this->widget(i));
            filesList << tabWidget->getFilepath();
        }
        return filesList;
    }

    /**
     * @brief popups a context menu
     * 
     */
    void onCustomContextMenu(const QPoint& point)
    {
        if (point.isNull())
            return;
     
        indexContextMenu = this->tabBar()->tabAt(point);
        IodeAbstractWidget* tabWidget = static_cast<IodeAbstractWidget*>(this->widget(indexContextMenu));
        const QPoint globalPoint = this->tabBar()->mapToGlobal(point);
        
        // REMINDER: By design 
        //           - there is only ONE tab per type of IODE database
        //           - the tabs representing an IODE database CANNOT be closed
        if(tabWidget->getFiletype() <= FILE_VARIABLES)
        {
            actionClose->setVisible(false);
            actionClear->setVisible(true);
        }
        else
        {
            actionClose->setVisible(true);
            actionClear->setVisible(false);
        }

        // Display option Split or Unsplit depending of the tab is already splitted or not
        if(tabWidget->splitted())
        {
            actionSplitH->setVisible(false);
            actionSplitV->setVisible(false);
            actionUnsplit->setVisible(true);
        }
        else
        {
            actionSplitH->setVisible(true);
            actionSplitV->setVisible(true);
            actionUnsplit->setVisible(false);
        }

        contextMenu->exec(globalPoint);
    }

    /**
     * @brief This virtual handler is called AFTER a new tab was added or 
     *        inserted at position index.
     * 
     * @param index 
     */
    void tabInserted(int index)
    {
        buildFilesList();
        IodeAbstractWidget* tabWidget = static_cast<IodeAbstractWidget*>(this->widget(index));
        fileSystemWatcher->addPath(tabWidget->getFilepath());
        QTabWidget::tabInserted(index);
    }

    /**
     * @brief This virtual handler is called AFTER a tab was removed 
     *        from position index
     * 
     * @param index 
     */
    void tabRemoved(int index)
    {
        buildFilesList();
    }

    /**
     * @brief Closes the tab at position index where index is:
     *        - current tab index if method called using a shortcut
     *        - index defined by the context menu if called from a context menu
     * 
     * @param index
     */
    void closeTab();

    /**
     * @brief Clears the IODE database displayed in the tab at position index where index is:
     *        - current tab index if method called using a shortcut
     *        - index defined by the context menu if called from a context menu
     * 
     * @param index
     */
    virtual void clearTab() = 0;

    /**
     * @brief copy the absolute path of the selected path to the selected file
     * 
     */
    void absolutePath();

    /**
     * @brief copy the relative path of the selected path to the selected file
     * 
     */
    void relativePath();

    /**
     * @brief open an OS file explorer and highlight the selected file.
     *        If several files selected, reveal only the last one (as in Visual Studio Code).
     * 
     */
    void revealInFolder();

    void splitTabHorizontally()
    {
        splitTab(indexContextMenu, Qt::Orientation::Horizontal);
        indexContextMenu = -1;
    }

    void splitTabVertically()
    {
        splitTab(indexContextMenu, Qt::Orientation::Vertical);
        indexContextMenu = -1;
    }

    void unsplit()
    {
        unsplitTab(indexContextMenu);
        indexContextMenu = -1;
    }
};
