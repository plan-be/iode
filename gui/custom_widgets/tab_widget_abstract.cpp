#include "tab_widget_abstract.h"


QIodeAbstractTabWidget::QIodeAbstractTabWidget(QWidget* parent) 
    : QTabWidget(parent), overwrite_all(false), discard_all(false)
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
    nextTabShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Tab), this);
    previousTabShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab), this);
    clearShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), this);

    // ---- file system watcher ----
    fileSystemWatcher = new QFileSystemWatcher(this);
    connect(fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, &QIodeAbstractTabWidget::reloadFile);

    // ---- connect signals to slots  ----
    connect(this, &QTabWidget::currentChanged, this, &QIodeAbstractTabWidget::showTab);
    connect(this, &QTabWidget::tabCloseRequested, this, &QIodeAbstractTabWidget::removeTab);

    connect(nextTabShortcut, &QShortcut::activated, this, &QIodeAbstractTabWidget::showNextTab);
    connect(previousTabShortcut, &QShortcut::activated, this, &QIodeAbstractTabWidget::showPreviousTab);
    connect(clearShortcut, &QShortcut::activated, this, &QIodeAbstractTabWidget::clearCurrentTab);

    // rebuild the list of files (tabs) everytime a tab is moved
    connect(this->tabBar(), &QTabBar::tabMoved, this, &QIodeAbstractTabWidget::buildFilesList);
}

QIodeAbstractTabWidget::~QIodeAbstractTabWidget()
{
    if(fileSystemWatcher) delete fileSystemWatcher;

    delete nextTabShortcut;
    delete previousTabShortcut;
    delete clearShortcut;
}

void QIodeAbstractTabWidget::resetFileSystemWatcher(const QDir& projectDir)
{
    // remove all previously registered directories and files
    fileSystemWatcher->removePaths(fileSystemWatcher->directories());
    fileSystemWatcher->removePaths(fileSystemWatcher->files());
}

int QIodeAbstractTabWidget::addReportTab(const QFileInfo& fileInfo)
{
    QIodeReportWidget* reportWidget = new QIodeReportWidget(fileInfo.absoluteFilePath(), output, completer, this);
    int index = addTab(reportWidget, reportWidget->getTabText());
    setTabToolTip(index, reportWidget->getTooltip());

    connect(reportWidget, &QIodeReportWidget::tabContentModified, this, &QIodeAbstractTabWidget::tabContentModified);
    connect(reportWidget, &QIodeReportWidget::askComputeHash, this, &QIodeAbstractTabWidget::computeHash);
    
    return index;
}

int QIodeAbstractTabWidget::addTextTab(const QFileInfo& fileInfo, const EnumIodeFile iodeFile, const bool forced)
{
    QIodeTextWidget* textWidget = new QIodeTextWidget(iodeFile, fileInfo.absoluteFilePath(), this);
    int index = addTab(textWidget, textWidget->getTabText());
    setTabToolTip(index, textWidget->getTooltip());

    if(forced)
        textWidget->setForcedAsText(true);
    
    connect(textWidget, &QIodeTextWidget::tabContentModified, this, &QIodeAbstractTabWidget::tabContentModified);
    
    return index;
}

int QIodeAbstractTabWidget::addNewTab(const EnumIodeFile fileType, const QFileInfo& fileInfo, const bool forceAsText)
{
    if (fileType <= I_VARIABLES_FILE) return -1;

    QString fullPath = fileInfo.absoluteFilePath();

    // Note: indexOf() returns -1 if not found
    int index = filesList.indexOf(fullPath);

    // check if file already opened
    if (index >= 0) return index;

    // prepare and add the new tab
    if(fileType == I_REPORTS_FILE)
        index = addReportTab(fileInfo);
    else if(forceAsText || QIodeTextWidget::isTextExtension("." + fileInfo.suffix())) 
            index = addTextTab(fileInfo, fileType, forceAsText);
    else
    { 
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
        index = -1;
    }

    if (index < 0) return index;

    return index;
}

void QIodeAbstractTabWidget::removeTab(const int index)
{
    // ask to save if modified
    AbstractTabWidget* tabWidget = static_cast<AbstractTabWidget*>(this->widget(index));
    if(tabWidget->isModified())
    {
        QString filepath = tabWidget->getFilepath();
        QString msg = "Unsaved modifications.\n";
        msg+= "Would you like to save modifications to " + filepath + " before closing the tab?";
        QMessageBox::StandardButton answer = QMessageBox::warning(nullptr, "WARNING", msg, 
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Discard, QMessageBox::Yes);
        if(answer == QMessageBox::Yes) tabWidget->save();
        if(answer == QMessageBox::No) tabContentModified(filepath, false);
        if(answer == QMessageBox::Discard) return;
    }

    // remove the path from the system file watcher
    fileSystemWatcher->removePath(tabWidget->getFilepath());

    // close the tab
    QTabWidget::removeTab(index);
}

void QIodeAbstractTabWidget::showTab(int index)
{
    // get the index of the tab currently visible if not passed to the method
    if (index < 0) index = this->currentIndex();

    // get type of file
    QString filename = this->tabText(index);
    EnumIodeFile fileType = get_iode_file_type(filename.toStdString());

    // update widget if needed
    AbstractTabWidget* tabWidget = static_cast<AbstractTabWidget*>(this->widget(index));
    if(tabWidget) tabWidget->update();

    this->setCurrentIndex(index);
}

void QIodeAbstractTabWidget::reloadFile(const QString& filepath)
{
	// find index of corresponding tab
	int index = filesList.indexOf(filepath);
	if(index < -1)
		return;

	// reload file
	QString filename = QFileInfo(filepath).fileName();    
	AbstractTabWidget* tabWidget = static_cast<AbstractTabWidget*>(this->widget(index));
	tabWidget->load(filepath, true);
}

bool QIodeAbstractTabWidget::saveTabContent(const int index)
{
    try
    {
        AbstractTabWidget* tabWidget = static_cast<AbstractTabWidget*>(this->widget(index));
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

bool QIodeAbstractTabWidget::saveCurrentTab()
{
    int index = currentIndex();
    return saveTabContent(index);
}

bool QIodeAbstractTabWidget::saveAllTabs()
{
    bool success = true;
    for(int index=0; index < this->count(); index++)
        if(!saveTabContent(index)) success = false;
    return success;
}

bool QIodeAbstractTabWidget::saveProjectAs(QDir& newProjectDir)
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

void QIodeAbstractTabWidget::fileMoved(const QString &oldFilepath, const QString &newFilepath)
{
    // check if oldFilePath present in currently open tabs
    int index = filesList.indexOf(oldFilepath);
    if (index >= 0)
    {
        AbstractTabWidget* tabWidget = static_cast<AbstractTabWidget*>(this->widget(index));
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
