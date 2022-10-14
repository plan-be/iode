#pragma once

#include <QObject>
#include <QString>
#include <QTabWidget>
#include <QFileInfo>
#include <QAbstractScrollArea>

#include "utils.h"
#include "util/system_item.h"
#include "iode_objs/widget/iode_obj_widget.h"


class QIodeTabWidget: public QTabWidget
{
    Q_OBJECT

    QSettings* settings;
    std::shared_ptr<QString> project_settings_filepath;

    QString projectDirPath;
    bool overwrite_all;
    bool discard_all;

private:
    /**
     * @brief add tab for editing Iode objects
     * 
     * @param iodeType EnumIodeType type of Iode object to edit
     * @param filepath QFileInfo path to the Iode object file
     * 
     * @return int Index of the new tab
     */
    int addObjectTab(const EnumIodeType iodeType, const QFileInfo& fileInfo);

    /**
     * @brief add tab for editing reports
     * 
     * @param fileInfo QFileInfo path to the report file
     * 
     * @return int Index of the new tab
     */
    int addReportTab(const QFileInfo& fileInfo);

    /**
     * @brief reopen all tabs 
     * 
     */
    void loadSettings();

    /**
     * @brief build list of open files (= tabs)
     * 
     * @return QStringList list of open files
     */
    QStringList buildFilesList()
    {
        QStringList filesList;
        for (int i=0; i < this->count(); i++) filesList << this->tabToolTip(i);
        return filesList;
    }
    
    /**
     * @brief dump list of opened files (= tabs) in settings
     * 
     */
    void saveSettings();

public:
    QIodeTabWidget(QWidget* parent = nullptr);
    ~QIodeTabWidget();

    /**
     * @brief - Initializes settings object and openedFiles list.
     *        - Reload previously opened files.
     * 
     * @param project_settings_filepath shared_ptr<QString> path the settings file
     */
    void setup(std::shared_ptr<QString>& project_settings_filepath);

	/**
	 * @brief load a file and set corresponding tab text (filename) and tooltip (full absolute path).
	 * 
	 * @param filepath QString path to the file to load
	 */
	void loadFile(const QString& filepath);

    void saveFile(const QString& filepath, const bool loop = false);

    /**
     * @brief save current project to another (new) directory
     * 
     * @param dir (New) directory to save the current project in
     * @return bool whether or not all files have been saved, i.e. the user as not clicked on the Discard button
     */
    bool saveProjectAs(QDir& newProjectDir);

    int addNewTab(const EnumIodeFile fileType, const QFileInfo& fileInfo);

	void viewTab(int index = -1);

    void resetFilters();

public slots:
    void removeTab(const int index);
    
    /**
     * @brief Update the path to the current project directory.
     *        Slot method called each time the project directory is changed.
     * 
     * @param projectDirPath Path to the root directory of the File Explorer  
     */
    void updateProjectDir(const QString& projectDirPath) { this->projectDirPath = projectDirPath; }

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
