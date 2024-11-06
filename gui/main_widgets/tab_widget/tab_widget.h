#pragma once

#include <QString>
#include <QVector>
#include <QFileInfo>
#include <QShortcut>
#include <QProgressDialog>
#include <QAbstractScrollArea>

#include "utils.h"
#include "tab_widget_abstract.h"
#include "tabs/iode_objs/tab_database.h"


class IodeTabWidget: public IodeAbstractTabWidget
{
    Q_OBJECT

    CommentsWidget* tabComments;
    EquationsWidget* tabEquations;
    IdentitiesWidget* tabIdentites;
    ListsWidget* tabLists;
    ScalarsWidget* tabScalars;
    TablesWidget* tabTables;
    VariablesWidget* tabVariables;
    QVector<AbstractIodeObjectWidget*> tabIodeObjects;

protected:
    int getIodeObjTabIndex(const IodeType iodeType) const override
    {
        int index; 

        switch (iodeType)
        {
        case COMMENTS:
            index = this->indexOf(tabComments);
            break;
        case EQUATIONS:
            index = this->indexOf(tabEquations);
            break;
        case IDENTITIES:
            index = this->indexOf(tabIdentites);
            break;
        case LISTS:
            index = this->indexOf(tabLists);
            break;
        case SCALARS:
            index = this->indexOf(tabScalars);
            break;
        case TABLES:
            index = this->indexOf(tabTables);
            break;
        case VARIABLES:
            index = this->indexOf(tabVariables);
            break;
        default:
            index = -1;
            break;
        }

        return index;
    }

    AbstractIodeObjectWidget* getIodeObjTab(const IodeType iodeType)
    {
        int index = getIodeObjTabIndex(iodeType);
        if(index < 0)
            return nullptr;

        return static_cast<AbstractIodeObjectWidget*>(this->widget(index));
    }

public:
    IodeTabWidget(QWidget* parent = nullptr);
    ~IodeTabWidget();

    void loadSettings() override;
    
    void saveSettings() override;

    void setup() override;

    int updateObjectTab(const IodeType iodeType);

    int loadFile(const QString& filepath, const bool displayTab=true, bool forceOverwrite=false, 
        const int moveToPosition=-1, const bool forceAsText=false) override;

    /**
     * @brief reset all filters on IODE objects.
     */
    void resetFilters();

    /**
     * @brief reset corresponding filter.
     * 
     * @param iodeType 
     */
    void resetFilter(const IodeType iodeType);

    /**
     * @brief clear all global KDBs and update all KDB tabs.
     * 
     */
    void clearWorkspace();

    /**
     * @brief Get the list of names of selected objects
     * 
     * @param iodeType
     * 
     * @return QStringList 
     */
    QStringList getSelectedObjectsNames(const IodeType iodeType);

    void openAddDialog(const IodeType iodeType) override;

    void openEditDialog(const IodeType iodeType) override;

public slots:
    void tabTextModified(const QString& filepath, const bool modified)
    {
        try
        {
            // get index
            int index = filesList.indexOf(filepath);
            if(index < 0) 
                return;

            // get the tab widget at the given index
            AbstractTextWidget* tabWidget = static_cast<AbstractTextWidget*>(this->widget(index));

            // updates the text and tootip of the tab
            setTabText(index, tabWidget->getTabText());
            setTabToolTip(index, tabWidget->getTooltip());

            // sends a signal to the File Explorer widget
            emit fileContentModified(tabWidget->getFilepath(), modified);
        }
        catch(const std::exception& e)
        {
            QMessageBox::critical(nullptr, "ERROR", QString(e.what()));
        }
    }

    void tabDatabaseModified(const IodeType iodeType, const bool modified)
    {
        try
        {
            // get index
            int index = getIodeObjTabIndex(iodeType);
            if(index < 0) 
                return;

            // get the tab widget at the given index
            AbstractIodeObjectWidget* tabWidget = static_cast<AbstractIodeObjectWidget*>(this->widget(index));

            // updates the text and tooltip of the tab
            setTabText(index, tabWidget->getTabText());
            setTabToolTip(index, tabWidget->getTooltip());

            // sends a signal to the File Explorer widget
            emit fileContentModified(tabWidget->getFilepath(), modified);
        }
        catch(const std::exception& e)
        {
            QMessageBox::critical(nullptr, "ERROR", QString(e.what()));
        }
    }

    void computeHash(const bool before=false) override
    {
        tabComments->computeHash(before);
        tabEquations->computeHash(before);
        tabIdentites->computeHash(before);
        tabLists->computeHash(before);
        tabScalars->computeHash(before);
        tabTables->computeHash(before);
        tabVariables->computeHash(before);
    }

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
        resetFileSystemWatcher(projectDir);
    }

    void clearTab() override;

    void showObjectsList(IodeType iodeType, const QStringList& objNames) override;
};
