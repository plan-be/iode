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
#include <QProcess>

#include "utils.h"
#include "util/system_item.h"
#include "main_widgets/tab_widget/tab_widget.h"
#include "file_explorer_proxy.h"
#include "file_delegate.h"


// TODO : enable drag and drop to move files and directories

/**
 * @brief The present class represents a File Explorer (i.e. File Tree) widget 
 *        which lists the files and subdirectories of an IODE project.
 *        The File Explorer widget is visible on as a left panel in the GUI.
 *
 *        Implemented features:
 *
 *        - Show the root directory of an IODE project at the top of the file tree.
 *        - Ask user to save all modifications before to switch to another IODE project.
 *        - Double click on a file opens a new tab (accepted files are KDB files, IODE Report files and text files).
 *        - Pressing ENTER opens the selected file(s).
 *        - It is possible to create/delete/copy/rename files and/or directories from a context menu (or via shortcuts):
 *           -> CTRL + N creates a new file.
 *           -> CTRL + SHIFT + N creates a new directory. 
 *           -> CTRL + X cuts the selected file(s)/directory(ies).
 *           -> CTRL + C copies the selected file(s)/directory(ies).
 *           -> CTRL + P pastes file(s)/directory(ies).
 *           -> SHIFT + ALT + C copies the absolute filepath.
 *           -> SHIFT + ALT + R reveals file in OS file explorer.
 *           -> F2 puts file/directory in edit mode (rename).
 *           -> Del deletes the selected  file(s)/directory(ies).
 *        - It is possible to move files and/or directories inside the project tree via drag and drop. 
 * 
 */
class IodeFileExplorer : public QTreeView
{
    Q_OBJECT

    QDir projectDir;
    QiodeFileExplorerProxyModel* proxyModel;
    QFileSystemModel* fileSystemModel;

    IodeTabWidget* tabWidget;

    QShortcut* newFileShortcut;
    QShortcut* newDirectoryShortcut;
    QShortcut* cutShortcut;
    QShortcut* copyShortcut;
    QShortcut* pasteShortcut;
    QShortcut* filepathShortcut;
    QShortcut* revealExplorerShortcut;
    QShortcut* renameShortcut;
    QShortcut* deleteShortcut;
    QShortcut* enterShortcut;
    QShortcut* cancelShortcut;

	QMenu* contextMenuDirectory;
	QMenu* contextMenuFile;
    QMenu* contextMenuExplorer;
    QAction* pasteActionDirectory;
    QAction* pasteActionExplorer;

    bool calledFromContextMenu;
    QModelIndex indexContextMenu;
    QModelIndex indexRename;
    QModelIndexList cutIndexes;
    QModelIndexList modifiedIndexes;
    QList<SystemItem> itemsToPast;

private:
	void setupContextMenu();

    QAction* addAction(const QString& name, const QString& tooltip, QMenu* contextMenu, 
        const QKeySequence& shortcut = QKeySequence())
    {
        QAction* action = new QAction(name, this);
        action->setToolTip(tooltip);
        action->setVisible(true);
        if(!shortcut.isEmpty()) action->setShortcut(shortcut);
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
        cutIndexes.clear();

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
     * @brief Guess the destination directory (when pasting or creating file(s) or directory(ies)).
     *        There are 3 possible cases in which this method is called:
     *        - context menu: the user right clicked -> displaying a context menu,
     *        - drag and drop: the user moves file(s) or directory(ies) using the drag and drop,
     *        - keyboard shortcuts: the user pressed a keyboard shortcut to create / paste file(s) or directory(ies).
     * 
     * @param indexDrop QModelIndex index representing where to drop the file(s)/directory(ies).
     * 
     * @return QDir destination directory.
     */
    QDir getDestinationDir(const QModelIndex& indexDrop = QModelIndex())
    {
        QModelIndex index;
        
        // case 1 (context menu): the user right clicked -> displaying a context menu                  
        if (calledFromContextMenu)
        {
            // case a: the user right clicked on the destination directory
            if (indexContextMenu.isValid())
                index = indexContextMenu;
            // case b: the user right clicked below the file tree
            //         -> destination directory = project directory
            else
                return projectDir;
        }
        // case 2 (drag and drop): the user moves file(s) or directory(ies) using the drag and drop
        else if (indexDrop.isValid()) 
            index = indexDrop;
        // case 3 (keyboard shortcuts): the user pressed a keyboard shortcut to create / paste file(s) or directory(ies)
        else
        {
            // in the case of keyboard shortcuts, we check the selected items in the file tree
            QModelIndexList selectedIndexes = this->selectedIndexes();
            // case a: one or several items are selected when pressing the keyboard shortcut -> take the first selected
            //         -> same behavior as in Visual Studio Code
            if (selectedIndexes.count() > 0) 
                index = selectedIndexes[0];
            // case b: no item(s) selected -> assume that the destination directory is the project directory
            else
                return projectDir;
        }

        QFileInfo fileInfo = proxyModel->fileInfo(index);
        // the index points to a directory -> destination directory
        if (fileInfo.isDir()) 
            return QDir(fileInfo.absoluteFilePath());
        // the index points to a file -> destination directory = parent directory of the file
        else 
            return fileInfo.absoluteDir();
    }

    /**
     * @brief to be called after each call to a slot method.
     * 
     */
    void cleanupSlot()
    {
        calledFromContextMenu = false;
        indexContextMenu = QModelIndex();
    }

protected:
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

public: 
    IodeFileExplorer(QWidget* parent = nullptr);
    ~IodeFileExplorer();

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

    void setIodeTabWidget(IodeTabWidget* tabWidget)
    {
        this->tabWidget = tabWidget;
        // to update filepath, name and tooltip of corresponding tab when a file is moved
        connect(this, &IodeFileExplorer::fileMoved, tabWidget, &IodeTabWidget::fileMoved);
        // to update filepath, name and tooltip of corresponding tab when a file is renamed
        connect(fileSystemModel, &QFileSystemModel::fileRenamed, tabWidget, &IodeTabWidget::fileRenamed);
        // to set corresponding file in color when its content is modified (and not yet saved to file)
        connect(tabWidget, &IodeTabWidget::fileContentModified, this, &IodeFileExplorer::fileContentModified);
    }

    /**
     * @brief update the project path and the content of the File Explorer.
     * 
     * @param projectDir directory containing the IODE project to open. 
     * @param project_settings_filepath shared pointer to the project settings file (.ini)
     */
    void updateProjectDir(const QDir& projectDir);

signals:
    void fileMoved(const QString& oldFilepath, const QString& newFilepath);

public slots:
    /**
     * @brief pops up either directory or file context menu depending on the item selected.
     * 
     * @note A context Menu is a menu display when the user right clicks somewhere.
     *       See Qt documentation of onCustomContextMenu.
     *       Overrided method.
     * 
     * @param point Position where the user right clicked
     */
    void onCustomContextMenu(const QPoint& point);

    /**
     * @brief set item in color when a user modifies the content of the associated database 
     *        or text/report file in tabs.
     * 
     * @param filepath 
     * @param modified
     */
    void fileContentModified(const QString& filepath, const bool modified);

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
     * @param forceAsText bool forces non IODE database or report files to be open as text tabs
     */
    void openFiles(const bool forceAsText=false);

    /**
     * @brief deselectes all items + clear Clipboard.
     *  
     */
    void cancel();
};
