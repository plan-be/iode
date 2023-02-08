#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QTabWidget>
#include <QFileInfo>
#include <QShortcut>
#include <QProgressDialog>
#include <QAbstractScrollArea>

#include "utils.h"
#include "util/system_item.h"
#include "tabs/qiode_tab_database.h"
#include "tabs/qiode_tab_text.h"


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
 *        - CTRL + S on a tab saves its content.
 *        - CTRL + SHIFT + S saves all tabs content.
 *        - CTRL + D on a KDB tab clears the corresponding KDB.
 *        - CTRL + SHIFT + D clears the whole workspace. 
 *        - ALT + [C|E|I|L|S|T|V] open the [Comments | Equations | Identities | Lists | Scalars | Tables | Variables] tab.
 *        - Ctrl+[shift]+Tab shift to the next [previous] tab.
 * 
 */
class QIodeTabWidget: public QTabWidget
{
    Q_OBJECT

    QSettings* settings;
    std::shared_ptr<QString> project_settings_filepath;

    QString projectDirPath;
    bool overwrite_all;
    bool discard_all;

    QVector<QString> tabPrefix;

    QShortcut* nextTabShortcut;
    QShortcut* previousTabShortcut;
    QShortcut* clearShortcut;

    QIodeCommentsWidget* tabComments;
    QIodeEquationsWidget* tabEquations;
    QIodeIdentitiesWidget* tabIdentites;
    QIodeListsWidget* tabLists;
    QIodeScalarsWidget* tabScalars;
    QIodeTablesWidget* tabTables;
    QIodeVariablesWidget* tabVariables;

private:
    /**
     * @brief build list of open files (= tabs)
     * 
     * @return QStringList list of open files
     */
    QStringList buildFilesList()
    {
        QStringList filesList;
        QString filepath;
        for (int i=0; i < this->count(); i++)
        {
            filepath = this->tabToolTip(i).trimmed();
            if(!filepath.isEmpty()) filesList << this->tabToolTip(i);
        }
        return filesList;
    }

    /**
     * @brief Set the default tab name and tooltip for a tab associated with a KDB. 
     * 
     * @param index int index of the tab.
     * @param iodeType EnumIodeType type of the IODE objects.
     * @param filepath QString path to the associated file (if any).
     */
    void setTabNameTooltip(const int index, const EnumIodeFile filetype, const QString& filepath)
    {
        if (filepath.isEmpty() || filepath == QString(I_DEFAULT_FILENAME))
        {
            EnumIodeType iodeType = (EnumIodeType) filetype;
            QString default_filename = QString(I_DEFAULT_FILENAME) + QString::fromStdString(vFileExtensions[filetype].ext);
            // Note: the * is to tell that the content of the KDB has not been saved in file
            this->setTabText(index, tabPrefix[filetype] + default_filename + "*");
            this->setTabToolTip(index, "Unsaved " + QString::fromStdString(vIodeTypes[iodeType]) + " Database");
        }
        else
        {
            QFileInfo fileInfo(filepath);
            this->setTabText(index, tabPrefix[filetype] + fileInfo.fileName());
            this->setTabToolTip(index, fileInfo.absoluteFilePath());
        }
    }

    /**
     * @brief return whether or not the tooltip is associated with a tab representing an unsaved KDB.
     * 
     * @param QString tooltip of the tab. 
     * @return bool whether or not the tooltip is associated with a tab representing an unsaved KDB.
     */
    bool isUnsavedKDB(const QString& tooltip)
    {
        return tooltip.startsWith("Unsaved");
    }

    /**
     * @brief return the IODE type corresponding to the passed tooltip.
     * 
     * @param QString tooltip of the tab.
     * @return int IODE type corresponding to the passed tooltip. -1 if undefined IODE type.
     */
    int extractIodeTypeFromDefaultTooltip(const QString& tooltip)
    {
        // See setDefaultTabNameTooltip()
        QString iodeTypeString = tooltip.split(" ")[1];
        return get_iode_type(iodeTypeString.toStdString());
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
     * 
     * @return int index of the new tab.
     */
    int addTextTab(const QFileInfo& fileInfo, const EnumIodeFile iodeFile);

    /**
     * @brief - reopen all tabs (files) that were open (*)
     *        - display the tab that was displayed (*)
     *        (*) the last time the user quitted the IODE gui.
     */
    void loadSettings();
    
    /**
     * @brief dump in settings:
     *        - the list of open tabs.
     *        - the index of the currently displayed tab.
     *        
     * @note The list of open tabs is actually the list of the tooltip associated with each tab.
     *       The tooltip is either the path to the corresponding file or a default string in case of unsaved KDB.  
     */
    void saveSettings();

    /**
     * @brief save the content of the tab at a given index.
     *        Ask for a filepath if the tab represents a KDB and the KDB is not linked to any file.
     * 
     * @param index int index of the tab to be saved.
     * 
     * @return bool whether or not the method has succeeded.
     */
    bool saveTabContent(const int index);

public:
    QIodeTabWidget(QWidget* parent = nullptr);
    ~QIodeTabWidget();

    /**
     * @brief - Initializes settings object and openedFiles list.
     *        - Reload previously opened files.
     * 
     * @param project_settings_filepath shared_ptr<QString> path the settings file
     * 
     * TODO: merge setup() method with updateProjectDir()
     */
    void setup(std::shared_ptr<QString>& project_settings_filepath);

	/**
	 * @brief load a file and set corresponding tab text (filename) and tooltip (full absolute path).
	 * 
	 * @param filepath QString path to the file to load.
     * @param displayTab bool whether or not displaying the corresponding tab when finished to load the file 
     *                        (used in loadSettings()).
     * @param forceOverwrite bool whether or not forcing overwrite of corresponding KDB (use in loadSetting()).
     * @param moveToPosition int new position of the corresponding tab. 
     *                           Only used when reloading tabs from a previous session.
     * 
     * @return int index of the corresponding tab. -1 if failed to load file.
	 */
	int loadFile(const QString& filepath, const bool displayTab=true, const bool forceOverwrite=false, const int moveToPosition=-1);

    /**
     * @brief save current project to another (new) directory.
     * 
     * @param dir (new) directory to save the current project in.
     * @return bool whether or not all files have been saved, i.e. the user as not clicked on the Discard button.
     */
    bool saveProjectAs(QDir& newProjectDir);

    /**
     * @brief update a tab representing a IODE KDB :
     *        - update tab text and tooltip,
     *        - reset the names filter.
     * 
     *        Called when loading a KDB file and when clearing a KDB.
     * 
     * @param iodeType EnumIodeType type of IODE objects to edit.
     * 
     * @return int Index of the tab.
     */
    int updateObjectTab(const EnumIodeType iodeType);

    /**
     * @brief create a new tab corresponding to the newly open file.
     *        Called by loadFile().
     * 
     * @param fileType EnumIodeFile type of the loaded file (KDB file, .log, .ini, .txt, other).
     * @param fileInfo QFileInfo path to the loaded file.
     * @return int index of the new tab
     */
    int addNewTab(const EnumIodeFile fileType, const QFileInfo& fileInfo);

    /**
     * @brief show tab corresponding to passed index. 
     *        Update current tab if index is -1.
     * 
     * @param index int index of the tab to show.
     */
	void showTab(int index = -1);

    /**
     * @brief reset all filters on IODE objects.
     */
    void resetFilters();

    /**
     * @brief reset corresponding filter.
     * 
     * @param iodeType 
     */
    void resetFilter(const EnumIodeType iodeType);

    /**
     * @brief clear all global KDBs and update all KDB tabs.
     * 
     */
    void clearWorkspace();

    VariablesView* getVariablesView() const
    {
        return tabVariables->get_view();
    }

public slots:
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
     * @brief save the content of the current tab.
     *        Ask for a filepath if the tab represents a KDB and the KDB is not linked to any file.
     * 
     * @return bool whether or not the method has succeeded.
     */
    bool saveCurrentTab();

    /**
     * @brief save all open tabs. 
     *        Ask for a filepath if the tab represents a KDB and the KDB is not linked to any file.
     * 
     * @return bool whether or not the method has succeeded. 
     */
    bool saveAllTabs();

    /**
     * @brief clear KDB of the current open tab if the later represents a KDB.
     * 
     */
    void clearCurrentTab();
    
    /**
     * @brief Update the path to the current project directory.
     *        Slot method called each time the project directory is changed.
     * 
     * @param projectDirPath Path to the root directory of the File Explorer  
     */
    void updateProjectDir(const QString& projectDirPath) 
    { 
        this->projectDirPath = projectDirPath;
        // associate new project directory to each KDB tab + clear global KDB
        QDir projectDir(projectDirPath);
        tabComments->setProjectDir(projectDir);
        tabEquations->setProjectDir(projectDir);
        tabIdentites->setProjectDir(projectDir);
        tabLists->setProjectDir(projectDir);
        tabScalars->setProjectDir(projectDir);
        tabTables->setProjectDir(projectDir);
        tabVariables->setProjectDir(projectDir);
    }

    /**
     * @brief Method called when a user rename a file using the File Explorer.
     *        Update the name and tooltip af tab associated with the renamed file if open.
     * 
     * @param path Path to the parent directory
     * @param oldName Name of the file before renaming
     * @param newName Name of the file after renaming
     */
    void fileRenamed(const QString &path, const QString &oldName, const QString &newName);
};
