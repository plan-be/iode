#include "file_explorer.h"


IodeFileExplorer::IodeFileExplorer(QWidget* parent): QTreeView(parent)
{
    proxyModel = new QiodeFileExplorerProxyModel(this);
    fileSystemModel = new QFileSystemModel(this);
    proxyModel->setSourceModel(fileSystemModel);

    // set model editable
    fileSystemModel->setReadOnly(false);

    // attach model to view
    this->setModel(proxyModel);

    // show only Name column (not Size and Date Modified columns)
    for (int i = 1; i < fileSystemModel->columnCount(); ++i) this->hideColumn(i);

    // delegate (for renaming files and directories)
    this->setItemDelegate(new FileDelegate(&cutIndexes, &modifiedIndexes, parent));

    // selection -> see main_window.ui
    // Selection mode = ExtendedSelection which means:
    // When the user selects an item in the usual way, the selection is cleared 
    // and the new item selected. However, if the user presses the Ctrl key when clicking on an item, 
    // the clicked item gets toggled and all other items are left untouched. 
    // If the user presses the Shift key while clicking on an item, all items between the current item 
    // and the clicked item are selected or unselected, depending on the state of the clicked item. 
    // Multiple items can be selected by dragging the mouse over them.

    // drag and drop -> see main_window.ui
    // - showDropIndicator = true which means the drop indicator is shown when dragging items and dropping.
    // - dragEnabled = true which means the view (File Explorer) supports dragging of its own items.
    // - acceptDrops = true which means the view (File Explorer) accepts drop of items (file or directory)
    // - dragDropOverwriteMode = true which means the selected data will overwrite the existing item data 
    //   when dropped, while moving the data will clear the item.
    // - dragDropMode = InternalMove which means the view (File Explorer) accepts move (not copy) operations 
    //   only from itself.
    // - defaultDropAction = MoveAction which means the data is moved from the source to the target.
    // More details on https://doc.qt.io/qt-6/model-view-programming.html#using-drag-and-drop-with-item-views
    
    // prepare shortcuts
    newFileShortcut = new QShortcut(QKeySequence::New, this);
    newDirectoryShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_N), this);
    cutShortcut = new QShortcut(QKeySequence::Cut, this);
    copyShortcut = new QShortcut(QKeySequence::Copy, this);
    pasteShortcut = new QShortcut(QKeySequence::Paste, this);
    filepathShortcut = new QShortcut(QKeySequence(Qt::SHIFT | Qt::ALT | Qt::Key_C), this);
    revealExplorerShortcut = new QShortcut(QKeySequence(Qt::SHIFT | Qt::ALT | Qt::Key_R), this);
    renameShortcut = new QShortcut(QKeySequence(Qt::Key_F2), this);
    deleteShortcut = new QShortcut(QKeySequence::Delete, this);
    enterShortcut = new QShortcut(QKeySequence(Qt::Key_Enter), this);
    cancelShortcut = new QShortcut(QKeySequence::Cancel, this);

    // NOTE: By default, shortcuts are defined at the main Window level. 
    //       Thus, a shortcut of a (combination of) key(s) may override the expected behavior 
    //       from another widget dealing with the same (combination of) key(s). 
    //       'setContext(Qt::WidgetWithChildrenShortcut)' makes sure that the shortcut does 
    //       not propagate to other widgets.
    newFileShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    newDirectoryShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    cutShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    copyShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    pasteShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    filepathShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    revealExplorerShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    renameShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    deleteShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    enterShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    cancelShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    // signals and slots
    connect(this, &IodeFileExplorer::doubleClicked, this, &IodeFileExplorer::openFile);
    connect(this, &IodeFileExplorer::customContextMenuRequested, this, &IodeFileExplorer::onCustomContextMenu);

    connect(newFileShortcut, &QShortcut::activated, this, &IodeFileExplorer::createFile);
    connect(newDirectoryShortcut, &QShortcut::activated, this, &IodeFileExplorer::createDir);
    connect(cutShortcut, &QShortcut::activated, this, &IodeFileExplorer::cut);
    connect(copyShortcut, &QShortcut::activated, this, &IodeFileExplorer::copy);
    connect(pasteShortcut, &QShortcut::activated, this, &IodeFileExplorer::paste);
    connect(filepathShortcut, &QShortcut::activated, this, &IodeFileExplorer::absolutePath);
    connect(revealExplorerShortcut, &QShortcut::activated, this, &IodeFileExplorer::revealInFolder);
    connect(renameShortcut, &QShortcut::activated, this, &IodeFileExplorer::rename);
    connect(deleteShortcut, &QShortcut::activated, this, &IodeFileExplorer::remove);
    connect(enterShortcut, &QShortcut::activated, this, [this](){ openFiles(false); });
    connect(cancelShortcut, &QShortcut::activated, this, &IodeFileExplorer::cancel);

    // context menu = popup menu displayed when a user right clicks
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    setupContextMenu();

    // misc
    calledFromContextMenu = false;
    indexContextMenu = QModelIndex();
}

// Note: do NOT delete tabWidget since it will be destroyed by MainWindow class
IodeFileExplorer::~IodeFileExplorer()
{
    saveSettings();

    itemsToPast.clear();
    cutIndexes.clear();
    modifiedIndexes.clear();

    delete contextMenuDirectory;
    delete contextMenuFile;
    delete contextMenuExplorer;

    delete newFileShortcut;
    delete newDirectoryShortcut;
    delete cutShortcut;
    delete copyShortcut;
    delete pasteShortcut;
    delete filepathShortcut;
    delete revealExplorerShortcut;
    delete renameShortcut;
    delete deleteShortcut;
    delete enterShortcut;
    delete cancelShortcut; 

    delete fileSystemModel;
    delete proxyModel;
}

void IodeFileExplorer::setupContextMenu()
{
    QAction* action;

    // --- directory context menu
    contextMenuDirectory = new QMenu(this);

    action = addAction("New File", "Add new file", contextMenuDirectory, newFileShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::createFile);

    action = addAction("New Folder", "Add new subdirectory", contextMenuDirectory, newDirectoryShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::createDir);

    contextMenuDirectory->addSeparator();

    action = addAction("Cut", "Cut Directory", contextMenuDirectory, cutShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::cut);

    action = addAction("Copy", "Copy Directory", contextMenuDirectory, copyShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::copy);

    pasteActionDirectory = addAction("Paste", "Paste", contextMenuDirectory, pasteShortcut->key());
    connect(pasteActionDirectory, &QAction::triggered, this, &IodeFileExplorer::paste);

    contextMenuDirectory->addSeparator();

    action = addAction("Copy Absolute Path", "Copy Absolute Path To The Clipboard", contextMenuDirectory, 
        filepathShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::absolutePath);

    action = addAction("Copy Relative Path", "Copy Relative Path To The Clipboard", contextMenuDirectory);
    connect(action, &QAction::triggered, this, &IodeFileExplorer::relativePath);

    action = addAction("Reveal in File Explorer", "open an OS file explorer and highlights the selected file", 
        contextMenuDirectory, revealExplorerShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::revealInFolder);

    contextMenuDirectory->addSeparator();

    action = addAction("Rename", "Rename Directory", contextMenuDirectory, renameShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::rename);

    action = addAction("Delete", "Delete Directory", contextMenuDirectory, deleteShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::remove);

    // --- file context menu
    contextMenuFile = new QMenu(this);

    action = addAction("Open", "Open File In A New Tab", contextMenuFile);
    connect(action, &QAction::triggered, this, &IodeFileExplorer::openFiles);

    action = addAction("Open As Text", "Open File As Text In A New Tab", contextMenuFile);
    connect(action, &QAction::triggered, this, [this](){ openFiles(true); });

    contextMenuFile->addSeparator();

    action = addAction("Cut", "Cut File", contextMenuFile, cutShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::cut);

    action = addAction("Copy", "Copy File", contextMenuFile, copyShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::copy);

    contextMenuFile->addSeparator();

    action = addAction("Copy Absolute Path", "Copy Absolute Path To The Clipboard", contextMenuFile, 
        filepathShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::absolutePath);
    
    action = addAction("Copy Relative Path", "Copy Relative Path To The Clipboard", contextMenuFile);
    connect(action, &QAction::triggered, this, &IodeFileExplorer::relativePath);
    
    action = addAction("Reveal in File Explorer", "open an OS file explorer and highlights the selected file", 
        contextMenuFile, revealExplorerShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::revealInFolder);

    contextMenuFile->addSeparator();

    action = addAction("Rename", "Rename Directory", contextMenuFile, renameShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::rename);

    action = addAction("Delete", "Delete Directory", contextMenuFile, deleteShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::remove);

    // --- file explorer menu (when a user clicks outside range of directories and files)
    contextMenuExplorer = new QMenu(this);

    action = addAction("New File", "Add new file to project", contextMenuExplorer, newFileShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::createFile);

    action = addAction("New Folder", "Add new directory to project", contextMenuExplorer, newDirectoryShortcut->key());
    connect(action, &QAction::triggered, this, &IodeFileExplorer::createDir);

    contextMenuExplorer->addSeparator();

    pasteActionExplorer = addAction("Paste", "Paste", contextMenuExplorer, pasteShortcut->key());
    connect(pasteActionExplorer, &QAction::triggered, this, &IodeFileExplorer::paste);

    disablePaste();
}

void IodeFileExplorer::loadSettings()
{
    QSettings* project_settings = ProjectSettings::getProjectSettings();
    if(!project_settings)
        return;
    
    // get list of expanded directories at the moment the application was closed
    project_settings->beginGroup("FILE_EXPLORER");
    QStringList dirsToExpand = project_settings->value("expanded_dirs", "").toStringList();
    project_settings->endGroup();

    // expand directories in file explorer (if they still exist)
    QStringList dirsList = projectDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(const QString& dirPath, dirsToExpand)
    {
        if (dirsToExpand.contains(dirPath))
        {
            QModelIndex index = proxyModel->index(dirPath);
            this->expand(index);
        }
    }
}

void IodeFileExplorer::saveSettings()
{
    QSettings* project_settings = ProjectSettings::getProjectSettings();
    if(!project_settings)
        return;

    // build list of expanded directories
    QFileInfoList dirsInfoList = projectDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList dirPathsList;
    QString dirPath;
    foreach(const QFileInfo& dirInfo, dirsInfoList)
    {
        dirPath = dirInfo.absoluteFilePath();
        QModelIndex index = proxyModel->index(dirPath);
        if (this->isExpanded(index)) dirPathsList << dirPath;
    }

    // save list of expanded directories to project settings
    project_settings->beginGroup("FILE_EXPLORER");
    QVariant dirs = QVariant::fromValue(dirPathsList);
    project_settings->setValue("expanded_dirs", dirs);
    project_settings->endGroup();
}

void IodeFileExplorer::updateProjectDir(const QDir& projectDir)
{    
    this->projectDir = projectDir;
    
    // Notes: - see https://doc.qt.io/qt-6/qtwidgets-itemviews-dirview-example.html 
    //        - We need the path to the parent directory of the project directory to be able to display 
    //          the project directory as the root directory.
    //          see https://stackoverflow.com/questions/53430989/pyside-qfilesystemmodel-display-show-root-item 
    QString projectPath = projectDir.absolutePath();
    QString parentPath = QFileInfo(projectPath).dir().absolutePath();
    fileSystemModel->setRootPath(parentPath);
    QModelIndex projectIndex = fileSystemModel->index(projectPath);
    // Warning: the line below shifts all indexes for the Tree view. 
    //          The index of the project (root) directory is now (0, 0).
    //          From now on, we must allways use proxyModel methods.
    proxyModel->setProjectIndex(QPersistentModelIndex(projectIndex));
    QModelIndex parentIndex = proxyModel->index(parentPath);
    this->setRootIndex(parentIndex);
    // shifts projectIndex to be compatible with the new indexes of the Tree view 
    projectIndex = proxyModel->mapFromSource(projectIndex);
    // expand the (root) project directory
    this->expand(projectIndex);

    // update project in the Tab widget
    tabWidget->updateProjectDir(projectPath);

    loadSettings();

    calledFromContextMenu = false;
    indexContextMenu = QModelIndex();

    this->show();
}

void IodeFileExplorer::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasText() && this->geometry().contains(event->pos()))
    {
        QTreeView::dragMoveEvent(event);
        event->setDropAction(Qt::MoveAction);
        event->acceptProposedAction();
    }
    else event->ignore();
}

void IodeFileExplorer::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText())
    {
        // get the destination directory in which the new file will be created
        QModelIndex indexDrop = this->indexAt(event->pos());
        QDir dropDir = getDestinationDir(indexDrop);

        // build list of items (files and directories) to drop
        QString mimeDataText = event->mimeData()->text();
        mimeDataText = mimeDataText.remove("file:///");
        QStringList itemsPathsList = mimeDataText.split("\n");

        // move items (files and directories)
        foreach(const QString& sourcePath, itemsPathsList)
        {
            QFileInfo sourceInfo(sourcePath);
            SystemItem itemToDrop(sourceInfo, true);
            itemToDrop.paste(dropDir);
            emit fileMoved(itemToDrop.absoluteFilePath(), dropDir.filePath(sourceInfo.fileName()));
        }

        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void IodeFileExplorer::openFile(const QModelIndex &index)
{
    // do nothing if user double clicked on a folder
    if (proxyModel->isDir(index)) return;

    // get path to the file on which the user double clicked
    QString filePath = proxyModel->filePath(index);

    // load the file
    tabWidget->loadFile(filePath);
}

// ---- SLOTS ----

void IodeFileExplorer::onCustomContextMenu(const QPoint& point)
{
    QMenu* contextMenuCurrent;
    calledFromContextMenu = true;
    const QPoint globalPoint = this->viewport()->mapToGlobal(point); 
    indexContextMenu = this->indexAt(point);
    if(indexContextMenu.isValid())
    {
        QFileInfo fileInfo = proxyModel->fileInfo(indexContextMenu);
        contextMenuCurrent = fileInfo.isDir() ? contextMenuDirectory : contextMenuFile;
    }
    else contextMenuCurrent = contextMenuExplorer;
    contextMenuCurrent->exec(globalPoint);
}

void IodeFileExplorer::fileContentModified(const QString& filepath, const bool modified)
{
    QModelIndex index = proxyModel->index(filepath);
    if(!index.isValid()) return;
    
    if(modified)
        modifiedIndexes << index;
    else
        modifiedIndexes.removeAll(index);

    this->viewport()->repaint();
}

void IodeFileExplorer::createFile()
{
    QDir parentDir = getDestinationDir();
    QString newFilePath = parentDir.filePath("newfile");
    QFile newFile(newFilePath);
    if(newFile.open(QIODevice::ReadWrite))
    {
        newFile.close();
        QModelIndex newFileIndex = proxyModel->index(newFilePath);
        // ask user to set a filename
        this->edit(newFileIndex);
        // user may have pushed escape to discard creating new file
        // if (proxyModel->fileName(newFileIndex) == "newfile") proxyModel->remove(newFileIndex);
    }
    cleanupSlot();
}

void IodeFileExplorer::createDir()
{
    QDir parentDir = getDestinationDir();
    if(parentDir.mkdir("NewFolder"))
    {
        QString newDirPath = parentDir.filePath("NewFolder");
        QModelIndex newDirIndex = proxyModel->index(newDirPath);
        // ask user to set a directory name
        this->edit(newDirIndex);
        // user may have pushed escape to discard creating new file
        // if (proxyModel->fileName(newDirIndex) == "NewFolder") proxyModel->remove(newDirIndex);
    }
    cleanupSlot();
}

void IodeFileExplorer::absolutePath()
{
    QClipboard* clipboard = QApplication::clipboard();
    QFileInfo fileInfo = proxyModel->fileInfo(indexContextMenu);
    QString absPath = fileInfo.absoluteFilePath();
    clipboard->setText(absPath);
    cleanupSlot();
}

void IodeFileExplorer::relativePath()
{
    QClipboard* clipboard = QApplication::clipboard();
    QFileInfo fileInfo = proxyModel->fileInfo(indexContextMenu);
    QString relPath = projectDir.relativeFilePath(fileInfo.absoluteFilePath());
    clipboard->setText(relPath);
    cleanupSlot();
}

void IodeFileExplorer::revealInFolder()
{
    QList<SystemItem> selectedItems = extractListOfItems();
    if(selectedItems.count() == 0) return;
    SystemItem item = selectedItems.last();
    QFileInfo info = item.fileInfo();
    QString path = item.absoluteFilePath();
#if defined(Q_OS_WIN)
    QStringList args;
    if (!info.isDir())
        args << "/select,";
    args << QDir::toNativeSeparators(path);
    if (QProcess::startDetached("explorer", args)) return;
#elif defined(Q_OS_MAC)
    QStringList args;
    args << "-e";
    args << "tell application \"Finder\"";
    args << "-e";
    args << "activate";
    args << "-e";
    args << "select POSIX file \"" + path + "\"";
    args << "-e";
    args << "end tell";
    args << "-e";
    args << "return";
    if (!QProcess::execute("/usr/bin/osascript", args)) return;
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(info.isDir()? path : info.path()));
#endif

    selectionModel()->clearSelection();
    cleanupSlot();

}

void IodeFileExplorer::cut()
{
    itemsToPast = extractListOfItems(true);
    enablePaste();
    cleanupSlot();
}

void IodeFileExplorer::copy()
{
    itemsToPast = extractListOfItems();
    enablePaste();
    cleanupSlot();
}

void IodeFileExplorer::paste()
{
    // get the destination directory in which the new directory will be created
    QDir destinationDir = getDestinationDir();
    // paste directory or file
    if(!itemsToPast.isEmpty())
    {
        foreach(SystemItem item, itemsToPast)
        {
            item.paste(destinationDir);
            emit fileMoved(item.absoluteFilePath(), destinationDir.filePath(item.fileInfo().fileName()));
        }
    }
    // cleanup everything
    cancel();
}

void IodeFileExplorer::rename()
{
    QModelIndex index;
    // rename activated via context menu
    if(indexContextMenu.isValid()) indexRename = indexContextMenu;
    // rename activated via shortcut
    else
    {
        QModelIndexList selectedIndexes = this->selectedIndexes();
        // no selected items -> do nothing
        if (selectedIndexes.count() == 0) return;
        // rename the last selected index
        indexRename = selectedIndexes[selectedIndexes.count()-1];
    }
    this->edit(indexRename);

    cleanupSlot();
}

void IodeFileExplorer::remove()
{
    QList<SystemItem> itemsToDelete = extractListOfItems();
    if(itemsToDelete.count() > 0)
    {
        QString msg = "Are you sure to delete %1 ?";
        QString arg;
        QMessageBox::StandardButton answer;
        if(itemsToDelete.count() == 1)
        {
            SystemItem item = itemsToDelete[0];
            QString filename = item.fileInfo().fileName();
            arg = item.isDir() ? "the directory '" + filename + "' and its content" : "the file '" + filename + "'";
            answer = QMessageBox::warning(nullptr, "WARNING", msg.arg(arg), QMessageBox::Yes | QMessageBox::No);
            if(answer == QMessageBox::Yes) item.remove();
        }
        else
        {
            bool at_least_one_file = false;
            bool at_least_one_directory = false;
            QStringList filenames;
            foreach(SystemItem item, itemsToDelete)
            {
                filenames << item.fileInfo().fileName();
                if(item.isFile()) at_least_one_file = true;
                if(item.isDir()) at_least_one_directory = true;
            }
            arg = "the following " + QString::number(filenames.count()) + " ";
            if(at_least_one_file) arg+= "files";
            if(at_least_one_file && at_least_one_directory) arg += "/";
            if(at_least_one_directory) arg += "directories and their content";
            arg += ":\n\n";
            arg += filenames.join("\n");
            arg += "\n\n";

            answer = QMessageBox::warning(nullptr, "WARNING", msg.arg(arg), QMessageBox::Yes | QMessageBox::No);
            if(answer == QMessageBox::Yes) foreach(SystemItem item, itemsToDelete) item.remove();
        }
    }
    selectionModel()->clearSelection();
    cleanupSlot();
}

void IodeFileExplorer::openFiles(const bool forceAsText)
{
    if (this->hasFocus())
    {
        QList<SystemItem> filesToOpen = extractListOfItems();
        foreach(SystemItem item, filesToOpen) 
            if (item.isFile()) tabWidget->loadFile(item.absoluteFilePath(), true, false, -1, forceAsText);
        selectionModel()->clearSelection();
        cleanupSlot();
    }
}

void IodeFileExplorer::cancel()
{
    if (this->hasFocus())
    {
        disablePaste();
        itemsToPast.clear();
        cutIndexes.clear();
        selectionModel()->clearSelection();
        cleanupSlot();
    }
}
