#include "qiode_file_explorer.h"


QIodeFileExplorer::QIodeFileExplorer(QWidget* parent): QTreeView(parent)
{
    proxyModel = new QiodeFileExplorerProxyModel(this);
    fileSystemModel = new QFileSystemModel(this);
    proxyModel->setSourceModel(fileSystemModel);

    // view only files with extension listed in C++ API 
    QStringList filters;
    IodeFileExtension file_ext;
    for (int i=0; i <= I_REPORTS_FILE; i++)
    {
        file_ext = vFileExtensions[i];
        filters << "*" + QString::fromStdString(file_ext.ext);
        if (file_ext.ascii != "") filters << "*" + QString::fromStdString(file_ext.ascii);
    }
    fileSystemModel->setNameFilters(filters);
    fileSystemModel->setNameFilterDisables(false);

    // set model editable
    fileSystemModel->setReadOnly(false);

    // attach model to view
    this->setModel(proxyModel);

    // show only Name column (not Size and Date Modified columns)
    for (int i = 1; i < fileSystemModel->columnCount(); ++i) this->hideColumn(i);

    // delegate (for renaming files and directories)
    this->setItemDelegate(new FileDelegate(&cutIndexes, parent));

    // context menu = popup menu displayed when a user right clicks
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    setupContextMenu();

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
    
    // signals and slots
    connect(this, &QIodeFileExplorer::doubleClicked, this, &QIodeFileExplorer::openFile);
    connect(this, &QIodeFileExplorer::customContextMenuRequested, this, &QIodeFileExplorer::onCustomContextMenu);

    // misc
    indexContextMenu = QModelIndex();
}

// Note: do NOT delete tabWidget since it will be destroyed by MainWindow class
QIodeFileExplorer::~QIodeFileExplorer()
{
    saveSettings();

    itemsToPast.clear();

    delete contextMenuDirectory;
    delete contextMenuFile;
    delete contextMenuExplorer;

    delete fileSystemModel;
    delete proxyModel;

    delete project_settings;
}

void QIodeFileExplorer::setupContextMenu()
{
    // --- directory context menu
    contextMenuDirectory = new QMenu(this);

    addAction("New File", "Add new file to directory", &QIodeFileExplorer::createFile, contextMenuDirectory);
    addAction("New Folder", "Add new file to directory", &QIodeFileExplorer::createDir, contextMenuDirectory);
    contextMenuDirectory->addSeparator();
    addAction("Cut", "Cut Directory", &QIodeFileExplorer::cut, contextMenuDirectory, QKeySequence::Cut);
    addAction("Copy", "Copy Directory", &QIodeFileExplorer::copy, contextMenuDirectory, QKeySequence::Copy);
    pasteActionDirectory = addAction("Paste", "Paste", &QIodeFileExplorer::paste, contextMenuDirectory, QKeySequence::Paste);
    contextMenuDirectory->addSeparator();
    addAction("Copy Absolute Path", "Copy Absolute Path To The Clipboard", &QIodeFileExplorer::absolutePath, 
        contextMenuDirectory, QKeySequence(Qt::SHIFT | Qt::ALT | Qt::Key_C));
    contextMenuDirectory->addSeparator();
    addAction("Rename", "Rename Directory", &QIodeFileExplorer::rename, contextMenuDirectory, QKeySequence(Qt::Key_F2));
    addAction("Delete", "Delete Directory", &QIodeFileExplorer::remove, contextMenuDirectory, QKeySequence::Delete);

    // --- file context menu
    contextMenuFile = new QMenu(this);

    addAction("Open", "Open File In A New Tab", &QIodeFileExplorer::openFileSlot, contextMenuFile);
    contextMenuFile->addSeparator();
    addAction("Cut", "Cut File", &QIodeFileExplorer::cut, contextMenuFile, QKeySequence::Cut);
    addAction("Copy", "Copy File", &QIodeFileExplorer::copy, contextMenuFile, QKeySequence::Copy);
    contextMenuFile->addSeparator();
    addAction("Copy Absolute Path", "Copy Absolute Path To The Clipboard", &QIodeFileExplorer::absolutePath, 
        contextMenuFile, QKeySequence(Qt::SHIFT | Qt::ALT | Qt::Key_C));
    contextMenuFile->addSeparator();
    addAction("Rename", "Rename Directory", &QIodeFileExplorer::rename, contextMenuFile, QKeySequence(Qt::Key_F2));
    addAction("Delete", "Delete Directory", &QIodeFileExplorer::remove, contextMenuFile, QKeySequence::Delete);

    // --- file explorer menu (when a user clicks outside range of directories and files)
    contextMenuExplorer = new QMenu(this);

    addAction("New Folder", "Add new file to directory", &QIodeFileExplorer::createDir, contextMenuExplorer, 
        QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_N));
    contextMenuExplorer->addSeparator();
    pasteActionExplorer = addAction("Paste", "Paste", &QIodeFileExplorer::paste, contextMenuExplorer, QKeySequence::Paste);

    disablePaste();
}

void QIodeFileExplorer::loadSettings()
{
    // get list of expanded directories at the moment the application was closed
    project_settings->beginGroup("File_Explorer");
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

void QIodeFileExplorer::saveSettings()
{
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
    project_settings->beginGroup("File_Explorer");
    QVariant dirs = QVariant::fromValue(dirPathsList);
    project_settings->setValue("expanded_dirs", dirs);
    project_settings->endGroup();
}

void QIodeFileExplorer::updateProjectDir(const QDir& projectDir, std::shared_ptr<QString>& project_settings_filepath)
{
    // save previous settings if any before to switch from project directory
    if (this->project_settings_filepath) saveSettings();
 
    this->project_settings_filepath = project_settings_filepath;
    project_settings = new QSettings(*project_settings_filepath, QSettings::IniFormat);
    
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

    loadSettings();

    indexContextMenu = QModelIndex();

    this->show();
}

void QIodeFileExplorer::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case QKeySequence::Cut :
        cut();
        break;
    case QKeySequence::Copy :
        copy();
        break;
    case QKeySequence::Paste :
        paste();
        break;
    case Qt::Key_F2 :
        rename();
        break;
    case QKeySequence::Delete :
        remove();
        break;
    case QKeySequence::Cancel:
        itemsToPast.clear();
        cutIndexes.clear();
        disablePaste();
    default:
        QTreeView::keyPressEvent(event);
        break;
    }
}

void QIodeFileExplorer::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasText() && this->geometry().contains(event->pos()))
    {
        QTreeView::dragMoveEvent(event);
        event->setDropAction(Qt::MoveAction);
        event->acceptProposedAction();
    }
    else event->ignore();
}

void QIodeFileExplorer::dropEvent(QDropEvent *event)
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
        foreach(const QString& itemPath, itemsPathsList)
        {
            SystemItem itemToDrop(QFileInfo(itemPath), true);
            itemToDrop.paste(dropDir);
        }

        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void QIodeFileExplorer::openFile(const QModelIndex &index)
{
    // do nothing if user double clicked on a folder
    if (proxyModel->isDir(index)) return;

    // get path to the file on which the user double clicked
    QString filePath = proxyModel->filePath(index);

    // load the file
    tabWidget->loadFile(filePath);
}

// ---- SLOTS ----

/**
 * @brief See Qt documentation of onCustomContextMenu.
 *        Overrided method.
 * @note A context Menu is a menu display when the user right clicks somewhere
 * 
 * @param point Position where the user right clicked
 */
void QIodeFileExplorer::onCustomContextMenu(const QPoint& point)
{
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

void QIodeFileExplorer::createFile()
{
    QDir parentDir = getDestinationDir(indexContextMenu);
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
    indexContextMenu = QModelIndex();
}

void QIodeFileExplorer::createDir()
{
    QDir parentDir = getDestinationDir(indexContextMenu);
    if(parentDir.mkdir("NewFolder"))
    {
        QString newDirPath = parentDir.filePath("NewFolder");
        QModelIndex newDirIndex = proxyModel->index(newDirPath);
        // ask user to set a directory name
        this->edit(newDirIndex);
        // user may have pushed escape to discard creating new file
        // if (proxyModel->fileName(newDirIndex) == "NewFolder") proxyModel->remove(newDirIndex);
    }
    indexContextMenu = QModelIndex();
}

void QIodeFileExplorer::absolutePath()
{
    QClipboard* clipboard = QApplication::clipboard();
    QFileInfo fileInfo = proxyModel->fileInfo(indexContextMenu);
    QString absPath = fileInfo.absoluteFilePath();
    clipboard->setText(absPath);
    indexContextMenu = QModelIndex();
}

void QIodeFileExplorer::cut()
{
    itemsToPast = extractListOfItems(true);
    // TODO : put item in gray
    // https://www.qtcentre.org/threads/61716-Set-the-color-of-a-row-in-a-qtreeview
    enablePaste();
    indexContextMenu = QModelIndex();
}

void QIodeFileExplorer::copy()
{
    itemsToPast = extractListOfItems(false);
    enablePaste();
    indexContextMenu = QModelIndex();
}

void QIodeFileExplorer::paste()
{
    // get the destination directory in which the new directory will be created
    QDir destinationDir = getDestinationDir(indexContextMenu);
    // paste directory or file
    if(!itemsToPast.isEmpty())
    {
        foreach(SystemItem item, itemsToPast) item.paste(destinationDir);
    }
    // disable Paste option in context menus
    disablePaste();
    itemsToPast.clear();
    cutIndexes.clear();
    indexContextMenu = QModelIndex();
}

void QIodeFileExplorer::rename()
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
    indexContextMenu = QModelIndex();
}

void QIodeFileExplorer::remove()
{
    QList<SystemItem> itemsToDelete = extractListOfItems(false);
    foreach(SystemItem item, itemsToDelete) item.remove();
    indexContextMenu = QModelIndex();
}

void QIodeFileExplorer::openFileSlot()
{
    openFile(indexContextMenu);
    indexContextMenu = QModelIndex();
}
