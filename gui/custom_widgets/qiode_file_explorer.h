#pragma once

#include <Qobject>
#include <QVariant>
#include <QList>
#include <QAction>
#include <QMenu>
#include <QModelIndex>
#include <QModelIndexList>
#include <QDir>
#include <QFile>
#include <QMimeData>
#include <QKeyEvent>
#include <QDropEvent>
#include <QStringList>
#include <QFileInfo>
#include <QClipboard>
#include <QSettings> 
#include <QTreeView>
#include <QSplashScreen>
#include <QMessageBox>
#include <QAbstractItemView>
#include <QFileSystemModel>

#include "utils.h"
#include "util/system_item.h"
#include "qiode_tab_widget.h"
#include "qiode_file_explorer_proxy.h"
#include "file_delegate.h"


// TODO : enable drag and drop to move files and directories
class QIodeFileExplorer : public QTreeView
{
    Q_OBJECT

    std::shared_ptr<QString> project_settings_filepath;
    QSettings* project_settings;

    QDir projectDir;
    QiodeFileExplorerProxyModel* proxyModel;
    QFileSystemModel* fileSystemModel;

    QIodeTabWidget* tabWidget;

    QMenu* contextMenuCurrent;
	QMenu* contextMenuDirectory;
	QMenu* contextMenuFile;
    QMenu* contextMenuExplorer;
    QAction* pasteActionDirectory;
    QAction* pasteActionExplorer;

    QModelIndex indexContextMenu;
    QModelIndex indexRename;
    QModelIndexList cutIndexes;
    QList<SystemItem> itemsToPast;

private:
	void setupContextMenu();

    /**
     * @brief load directories that were expanded when the program has been closed
     * 
     */
    void loadSettings();

    /**
     * @brief save directories that are expanded before to close the program
     * 
     */
    void saveSettings();

    QAction* addAction(const QString& name, const QString& tooltip, void (QIodeFileExplorer::*slotFunction)(), 
        QMenu* contextMenu, const QKeySequence& shortcut = QKeySequence())
    {
        QAction* action = new QAction(name, this);
        action->setToolTip(tooltip);
        action->setVisible(true);
        if(!shortcut.isEmpty()) action->setShortcut(shortcut);
        connect(action, &QAction::triggered, this, slotFunction);
        contextMenu->addAction(action);
        return action;
    }

    void enablePaste()
    {
        pasteActionDirectory->setEnabled(true);
        pasteActionExplorer->setEnabled(true);
    }

    void disablePaste()
    {
        pasteActionDirectory->setEnabled(false);
        pasteActionExplorer->setEnabled(false);
    }

    /**
     * @brief extracts list of items (files and directories) to cut, copy or delete. 
     * 
     * @param cut bool whether of not to delete items after copy.
     */
    QList<SystemItem> extractListOfItems(const bool cut=false)
    {
        QList<SystemItem> items; 

        // get list of currently selected files and directories in the file explorer
        QModelIndexList selectedIndexes = this->selectedIndexes();

        // Notes:
        // - indexContextMenu is the index of the item (file or directory) where the user right click,
        // - indexContextMenu is not valid when the user used a keybord shortcut and not his mouse 
        //   to call cut, copy or delete.

        // case 1: the user right clicked outside the range of selected files and directories
        // -> only the item (file or directory) on which the user right clicked will be cut, copied or deleted
        if(indexContextMenu.isValid() && selectedIndexes.indexOf(indexContextMenu) == -1)
        {
            items << SystemItem(proxyModel->fileInfo(indexContextMenu), cut);
            if (cut) cutIndexes << indexContextMenu;
        }
        // case 2 : the user used a keybord shortcut (indexContextMenu not valid -> no context menu).
        // case 3 : the user right clicked on one of the selected files and directories 
        //          (indexContextMenu is in the selectedIndexes list)
        // -> all selected items will be cut, copied or deleted
        else
        {
            foreach(const QModelIndex& index, selectedIndexes)
            {
                items << SystemItem(proxyModel->fileInfo(index), cut);
                if (cut) cutIndexes << index;
            } 
        }
        
        return items;
    }

    /**
     * @brief function called when the user double clicked on a item in the tree (= file explorer) view
     * 
     * @param index index (row and column) of the item selected 
     */
    void openFile(const QModelIndex &index);

    /**
     * @brief Get the destination directory
     * 
     * @return QDir 
     */
    QDir getDestinationDir(const QModelIndex &index)
    {
        if(index.isValid())
        {
            QFileInfo fileInfo = proxyModel->fileInfo(index);
            if (fileInfo.isDir()) return QDir(fileInfo.absoluteFilePath());
            else return fileInfo.absoluteDir();
        }
        else return projectDir;
    }

protected:
    void keyPressEvent(QKeyEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

public: 
    QIodeFileExplorer(QWidget* parent = nullptr);
    ~QIodeFileExplorer();

    void setIodeTabWidget(QIodeTabWidget* tabWidget)
    {
        this->tabWidget = tabWidget;
        // to update tab name and tooltip of corresponding tab when a file is renamed
        connect(fileSystemModel, &QFileSystemModel::fileRenamed, this->tabWidget, &QIodeTabWidget::fileRenamed);
        // to update the current project dir in the Tabs widget
        connect(fileSystemModel, &QFileSystemModel::rootPathChanged, this->tabWidget, &QIodeTabWidget::updateProjectDir);
    }

    /**
     * @brief update project path and the file explorer model and view.
     * 
     * @param project_settings_filepath shared pointer to the project settings file (.ini)
     */
    void updateProjectDir(const QDir& projectDir, std::shared_ptr<QString>& project_settings_filepath);

public slots:
    /**
     * @brief pops up either directory or file context menu depending on the item selected
     * 
     */
    void onCustomContextMenu(const QPoint& point);

    /**
     * @brief Create a new file
     * 
     */
    void createFile();

    /**
     * @brief Create a new sub directory
     * 
     */
    void createDir();

    /**
     * @brief copy the absolute path of the selected path to the 
     * 
     */
    void absolutePath();

    /**
     * @brief cut file or directory
     * 
     */
    void cut();

    /**
     * @brief copy file or directory
     * 
     */
    void copy();

    /**
     * @brief paste file or directory
     * 
     */
    void paste();

    /**
     * @brief rename file or directory
     * 
     */
    void rename();

    /**
     * @brief Delete file or directory
     * 
     */
    void remove();
    
    /**
     * @brief function called when the user double clicked on a item in the tree (= file explorer) view
     * 
     * @param index index (row and column) of the item selected 
     */
    void openFileSlot();
};
