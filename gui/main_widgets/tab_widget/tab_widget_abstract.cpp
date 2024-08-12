#include "tab_widget_abstract.h"


IodeAbstractTabWidget::IodeAbstractTabWidget(QWidget* parent) 
    : QTabWidget(parent), overwrite_all(false), discard_all(false), indexContextMenu(-1)
{
    // set name
    this->setObjectName(QString::fromUtf8("tabWidget_IODE_objs"));

    // set policy
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    this->setSizePolicy(sizePolicy);

    // prepare shortcuts
    filepathShortcut = new QShortcut(QKeySequence(Qt::SHIFT | Qt::ALT | Qt::Key_C), this);
    revealExplorerShortcut = new QShortcut(QKeySequence(Qt::SHIFT | Qt::ALT | Qt::Key_R), this);
    nextTabShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Tab), this);
    previousTabShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab), this);
    clearShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), this);
    closeShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_W), this);

    // NOTE: By default, shortcuts are defined at the main Window level. 
    //       Thus, a shortcut of a (combination of) key(s) may override the expected behavior 
    //       from another widget dealing with the same (combination of) key(s). 
    //       'setContext(Qt::WidgetWithChildrenShortcut)' makes sure that the shortcut does 
    //       not propagate to other widgets.
    filepathShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    revealExplorerShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    nextTabShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
    previousTabShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
    clearShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
    closeShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);

    // ---- file system watcher ----
    fileSystemWatcher = new QFileSystemWatcher(this);
    connect(fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, &IodeAbstractTabWidget::reloadFile);

    // ---- connect signals to slots  ----
    connect(this, &QTabWidget::currentChanged, this, &IodeAbstractTabWidget::showTab);
    connect(this, &QTabWidget::tabCloseRequested, this, &IodeAbstractTabWidget::removeTab);

    connect(filepathShortcut, &QShortcut::activated, this, &IodeAbstractTabWidget::absolutePath);
    connect(revealExplorerShortcut, &QShortcut::activated, this, &IodeAbstractTabWidget::revealInFolder);
    connect(nextTabShortcut, &QShortcut::activated, this, &IodeAbstractTabWidget::showNextTab);
    connect(previousTabShortcut, &QShortcut::activated, this, &IodeAbstractTabWidget::showPreviousTab);
    connect(clearShortcut, &QShortcut::activated, this, &IodeAbstractTabWidget::clearTab);
    connect(closeShortcut, &QShortcut::activated, this, &IodeAbstractTabWidget::closeTab);

    // rebuild the list of files (tabs) everytime a tab is moved
    connect(this->tabBar(), &QTabBar::tabMoved, this, &IodeAbstractTabWidget::buildFilesList);

    // popups a context menu when the user right clicks on a tab
    this->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->tabBar(), &QTabBar::customContextMenuRequested, this, &IodeAbstractTabWidget::onCustomContextMenu);

    // setup context menu
    setupContextMenu();
}

IodeAbstractTabWidget::~IodeAbstractTabWidget()
{
    if(fileSystemWatcher) delete fileSystemWatcher;

    delete actionClose;
    delete actionClear;
    delete contextMenu;

    delete filepathShortcut;
    delete revealExplorerShortcut;
    delete nextTabShortcut;
    delete previousTabShortcut;
    delete clearShortcut;
    delete closeShortcut;

    delete actionSplitH;
    delete actionSplitV;
    delete actionUnsplit;
}

void IodeAbstractTabWidget::setupContextMenu()
{
    QAction* action;

    // --- directory context menu
    contextMenu = new QMenu(this);

    // close tab (MUST BE DISABLE FOR TABS REPRESENTING AN IODE DATABASE)
    actionClose = addAction("Close", "Close the tab", closeShortcut->key());
    connect(actionClose, &QAction::triggered, this, &IodeAbstractTabWidget::closeTab);
    contextMenu->addAction(actionClose);

    // save tab
    action = addAction("Save", "Save the tab", QKeySequence(Qt::CTRL | Qt::Key_S));
    connect(action, &QAction::triggered, this, &IodeAbstractTabWidget::saveTab);
    contextMenu->addAction(action);

    // clear tab (ONLY FOR TABS REPRESENTING AN IODE DATABASE)
    actionClear = addAction("Clear", "Clear the correspondig IODE database", clearShortcut->key());
    connect(actionClear, &QAction::triggered, this, &IodeAbstractTabWidget::clearTab);
    contextMenu->addAction(actionClear);

    // separator 
    contextMenu->addSeparator();

    // absolute path
    action = addAction("Copy Absolute Path", "Copy Absolute Path To The Clipboard", filepathShortcut->key());
    connect(action, &QAction::triggered, this, &IodeAbstractTabWidget::absolutePath);
    contextMenu->addAction(action);

    // relative path
    action = addAction("Copy Relative Path", "Copy Relative Path To The Clipboard");
    connect(action, &QAction::triggered, this, &IodeAbstractTabWidget::relativePath);
    contextMenu->addAction(action);

    // reveal in explorer
    action = addAction("Reveal in File Explorer", "open an OS file explorer and highlights the selected file", 
        revealExplorerShortcut->key());
    connect(action, &QAction::triggered, this, &IodeAbstractTabWidget::revealInFolder);
    contextMenu->addAction(action);

    // separator 
    contextMenu->addSeparator();

    // split vertically
    actionSplitV = addAction("Split Right", "Split the tab vertically");
    connect(actionSplitV, &QAction::triggered, this, &IodeAbstractTabWidget::splitTabHorizontally);
    contextMenu->addAction(actionSplitV);

    // split horizontally
    actionSplitH = addAction("Split Down", "Split the tab horizontally");
    connect(actionSplitH, &QAction::triggered, this, &IodeAbstractTabWidget::splitTabVertically);
    contextMenu->addAction(actionSplitH);

    actionUnsplit = addAction("Unsplit", "unsplit the tab content");
    connect(actionUnsplit, &QAction::triggered, this, &IodeAbstractTabWidget::unsplit);
    contextMenu->addAction(actionUnsplit);
}

void IodeAbstractTabWidget::resetFileSystemWatcher(const QDir& projectDir)
{
    // remove all previously registered directories and files
    fileSystemWatcher->removePaths(fileSystemWatcher->directories());
    fileSystemWatcher->removePaths(fileSystemWatcher->files());
}

int IodeAbstractTabWidget::addReportTab(const QFileInfo& fileInfo)
{
    ReportWidget* reportWidget = new ReportWidget(fileInfo.absoluteFilePath(), this);
    int index = addTab(reportWidget, reportWidget->getTabText());
    setTabToolTip(index, reportWidget->getTooltip());

    connect(reportWidget, &ReportWidget::tabTextModified, this, &IodeAbstractTabWidget::tabTextModified);
    connect(reportWidget, &ReportWidget::askComputeHash, this, &IodeAbstractTabWidget::computeHash);
    
    return index;
}

int IodeAbstractTabWidget::addTextTab(const QFileInfo& fileInfo, const IodeFileType iodeFile, const bool forced)
{
    TextWidget* textWidget = new TextWidget(iodeFile, fileInfo.absoluteFilePath(), this);
    int index = addTab(textWidget, textWidget->getTabText());
    setTabToolTip(index, textWidget->getTooltip());

    if(forced)
        textWidget->setForcedAsText(true);
    
    connect(textWidget, &TextWidget::tabTextModified, this, &IodeAbstractTabWidget::tabTextModified);
    
    return index;
}

int IodeAbstractTabWidget::addNewTab(const IodeFileType fileType, const QFileInfo& fileInfo, const bool forceAsText)
{
    if (fileType <= FILE_VARIABLES) 
        return -1;

    QString fullPath = fileInfo.absoluteFilePath();

    // Note: indexOf() returns -1 if not found
    int index = filesList.indexOf(fullPath);

    // check if file already opened
    if (index >= 0) 
        return index;

    // prepare and add the new tab
    if(fileType == FILE_REP)
        index = addReportTab(fileInfo);
    else if(forceAsText || showInTextTabExtensionsList.contains(fileInfo.suffix())) 
            index = addTextTab(fileInfo, fileType, forceAsText);
    else
    { 
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
        index = -1;
    }

    if (index < 0) return index;

    return index;
}

void IodeAbstractTabWidget::removeTab(const int index)
{
    // ask to save if modified
    IodeAbstractWidget* tabWidget = static_cast<IodeAbstractWidget*>(this->widget(index));
    if(tabWidget->isModified())
    {
        QString filepath = tabWidget->getFilepath();
        QString msg = "Unsaved modifications.\n";
        msg+= "Would you like to save modifications to " + filepath + " before closing the tab?";
        QMessageBox::StandardButton answer = QMessageBox::warning(nullptr, "WARNING", msg, 
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Discard, QMessageBox::Yes);
        if(answer == QMessageBox::Yes) tabWidget->save();
        if(answer == QMessageBox::No) tabTextModified(filepath, false);
        if(answer == QMessageBox::Discard) return;
    }

    // remove the path from the system file watcher
    fileSystemWatcher->removePath(tabWidget->getFilepath());

    // close the tab
    QTabWidget::removeTab(index);
}

void IodeAbstractTabWidget::showTab(int index)
{
    // get the index of the tab currently visible if not passed to the method
    if (index < 0) index = this->currentIndex();

    // get type of file
    QString filename = this->tabText(index);
    IodeFileType fileType = get_iode_file_type(filename.toStdString());

    // update widget if needed
    IodeAbstractWidget* tabWidget = static_cast<IodeAbstractWidget*>(this->widget(index));
    if(tabWidget) tabWidget->update();

    this->setCurrentIndex(index);
}

void IodeAbstractTabWidget::reloadFile(const QString& filepath)
{
	// find index of corresponding tab
	int index = filesList.indexOf(filepath);
	if(index < 0)
		return;

	// reload file
	QString filename = QFileInfo(filepath).fileName();    
	IodeAbstractWidget* tabWidget = static_cast<IodeAbstractWidget*>(this->widget(index));
	tabWidget->load(filepath, true);
}

bool IodeAbstractTabWidget::saveTabContent(const int index)
{
    try
    {
        IodeAbstractWidget* tabWidget = static_cast<IodeAbstractWidget*>(this->widget(index));
        QString filepath = tabWidget->save();
        if (filepath.isEmpty()) 
            return false;
        
        setTabText(index, tabWidget->getTabText());
        setTabToolTip(index, tabWidget->getTooltip());

        return true;
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", "Couldn't save the content of " + tabText(index));
        return false;
    }
}

bool IodeAbstractTabWidget::save_all_tabs()
{
    bool success = true;
    for(int index=0; index < this->count(); index++)
        if(!saveTabContent(index)) success = false;
    return success;
}

bool IodeAbstractTabWidget::saveProjectAs(QDir& newProjectDir)
{
    QString newProjectPath = newProjectDir.absolutePath();

    if (!newProjectDir.exists())
    {
        QMessageBox::warning(nullptr, "WARNING", "Directory " + newProjectPath + " does not exist");
        return false;
    }

    SystemItem currentProject(QFileInfo(projectDirPath), false);
    return currentProject.copyTo(newProjectDir);
}

void IodeAbstractTabWidget::fileMoved(const QString &oldFilepath, const QString &newFilepath)
{
    // check if oldFilePath present in currently open tabs
    int index = filesList.indexOf(oldFilepath);
    if (index >= 0)
    {
        IodeAbstractWidget* tabWidget = static_cast<IodeAbstractWidget*>(this->widget(index));
        // update filepath in widget
        if(!tabWidget->updateFilepath(newFilepath))
            return;
        // update name and tooltip of corresponding tab
        setTabText(index, tabWidget->getTabText());
        setTabToolTip(index, tabWidget->getTooltip());
        // update list of open files (tabs)
        filesList[index] = newFilepath;
    }

    // update file system watcher
    fileSystemWatcher->removePath(oldFilepath);
    fileSystemWatcher->addPath(newFilepath);
}

void IodeAbstractTabWidget::closeTab()
{
    int index = (indexContextMenu > 0) ? indexContextMenu : currentIndex();
    AbstractTextWidget* tabWidget = static_cast<AbstractTextWidget*>(this->widget(index));
    IodeFileType fileType = tabWidget->getFiletype();

    // Tabs representing an IODE database file cannot be closed.
    // This is by design.
    if(fileType <= FILE_VARIABLES)
        return;

    removeTab(index);

    indexContextMenu = -1;
}

void IodeAbstractTabWidget::saveTab()
{
    int index = (indexContextMenu > 0) ? indexContextMenu : currentIndex();
    saveTabContent(index);

    indexContextMenu = -1;
}

void IodeAbstractTabWidget::absolutePath()
{
    int index = currentIndex();
    AbstractTextWidget* tabWidget = static_cast<AbstractTextWidget*>(this->widget(index));
    QString filepath = tabWidget->getFilepath();

    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(filepath);

    indexContextMenu = -1;
}

void IodeAbstractTabWidget::relativePath()
{
    int index = currentIndex();
    AbstractTextWidget* tabWidget = static_cast<AbstractTextWidget*>(this->widget(index));
    QString filepath = tabWidget->getFilepath();
    QDir projectDir(projectDirPath); 
    QString relativePath = projectDir.relativeFilePath(filepath);

    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(relativePath);

    indexContextMenu = -1;
}

void IodeAbstractTabWidget::revealInFolder()
{
    int index = currentIndex();
    AbstractTextWidget* tabWidget = static_cast<AbstractTextWidget*>(this->widget(index));
    QString filepath = tabWidget->getFilepath();
    QFileInfo info(filepath);

#if defined(Q_OS_WIN)
    QStringList args;
    args << "/select,";
    args << QDir::toNativeSeparators(filepath);
    if (QProcess::startDetached("explorer", args)) return;
#elif defined(Q_OS_MAC)
    QStringList args;
    args << "-e";
    args << "tell application \"Finder\"";
    args << "-e";
    args << "activate";
    args << "-e";
    args << "select POSIX file \"" + filepath + "\"";
    args << "-e";
    args << "end tell";
    args << "-e";
    args << "return";
    if (!QProcess::execute("/usr/bin/osascript", args)) return;
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(filepath));
#endif

    indexContextMenu = -1;
}
