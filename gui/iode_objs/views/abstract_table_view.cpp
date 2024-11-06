#include "abstract_table_view.h"


IodeAbstractTableView::IodeAbstractTableView(IodeTypes iodeType, BaseDelegate* delegate, QWidget* parent) 
	: QTableView(parent), iodeType(iodeType), delegate(delegate) 
{
    QString stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";

    // size policy
    QSizePolicy sizePolicyTable(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicyTable.setHorizontalStretch(0);
    sizePolicyTable.setVerticalStretch(0);
    sizePolicyTable.setHeightForWidth(sizePolicy().hasHeightForWidth());
    setSizePolicy(sizePolicyTable);
    setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);

    // sorting
    setSortingEnabled(true);

    // headers
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setStretchLastSection(false);

    // stylesheet
    setStyleSheet(stylesheet);

    // delegate
	setItemDelegate(delegate);

    // ---- Selection ----
    // See: - https://doc.qt.io/qt-5/model-view-programming.html#handling-selections-in-item-views
    //      - https://doc.qt.io/qt-5/model-view-programming.html#selections
    
    // -- Behavior --
    // Selecting only rows.
    // See: https://doc.qt.io/qt-5/qabstractitemview.html#SelectionBehavior-enum
    setSelectionBehavior(QTableView::SelectRows);
    // -- Mode --
    // When the user selects an item in the usual way, the selection is cleared and the new item selected. 
    // However, if the user presses the Ctrl key when clicking on an item, the clicked item gets toggled 
    // and all other items are left untouched. 
    // If the user presses the Shift key while clicking on an item, all items between the current item and 
    // the clicked item are selected or unselected, depending on the state of the clicked item. 
    // Multiple items can be selected by dragging the mouse over them.
    // See: https://doc.qt.io/qt-5/qabstractitemview.html#SelectionMode-enum
    setSelectionMode(QTableView::ExtendedSelection);

    // ---- Edit Trigger ----
    // the user must double click to edit a cell
    setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked); 

    // ---- Signals & Slots ----
    deleteShortcut = new QShortcut(QKeySequence::Delete, this);
    // NOTE: Required to avoid confusion when deleting a file/directory from the File Explorer
    deleteShortcut->setContext(Qt::WidgetWithChildrenShortcut);	

    connect(deleteShortcut, &QShortcut::activated, this, &IodeAbstractTableView::removeObjects);

    // for editing IODE object names
    objectNameEdit = new QLineEdit(this);
    QString acceptedLetters;
    if (iodeType == COMMENTS)
        acceptedLetters = "A-Za-z";
    else if (iodeType == SCALARS)
        acceptedLetters = "a-z";
    else
        acceptedLetters = "A-Z";
	QRegularExpression re(QString("[%1_][0-9%1_]{0,%2}").arg(acceptedLetters).arg(QString::number(K_MAX_NAME - 1)));
	QRegularExpressionValidator* validator = new QRegularExpressionValidator(re, this);
	objectNameEdit->setValidator(validator);
    objectNameEdit->setHidden(true);

    connect(this->verticalHeader(), &QHeaderView::sectionDoubleClicked, this, &IodeAbstractTableView::openEditorName);
    connect(objectNameEdit, &QLineEdit::returnPressed, this, &IodeAbstractTableView::editName);
    connect(objectNameEdit, &QLineEdit::editingFinished, this, &IodeAbstractTableView::closeNameEditor);

    // to popup an editing dialog box
    shortcutEditEnter = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Enter), this);
    connect(shortcutEditEnter, &QShortcut::activated, this, &IodeAbstractTableView::edit_obj);
    
    shortcutEditReturn = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return), this);
    connect(shortcutEditReturn, &QShortcut::activated, this, &IodeAbstractTableView::edit_obj);

    // gets objects of the same name or listed in the clec structure
 	sameCmtShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_F1), this);
	sameEqShortcut  = new QShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_F2), this);
	sameIdtShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_F3), this);
	sameLstShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_F4), this);
	sameTblShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_F6), this);
	varsFromClecShortcut     = new QShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_F7), this);
	scalarsFromClecShortcut  = new QShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_F5), this);

    sameCmtShortcut->setContext(Qt::WidgetWithChildrenShortcut);	
    sameEqShortcut->setContext(Qt::WidgetWithChildrenShortcut);	
    sameIdtShortcut->setContext(Qt::WidgetWithChildrenShortcut);	
    sameLstShortcut->setContext(Qt::WidgetWithChildrenShortcut);	
    sameTblShortcut->setContext(Qt::WidgetWithChildrenShortcut);	
    varsFromClecShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    scalarsFromClecShortcut->setContext(Qt::WidgetWithChildrenShortcut);	

    connect(sameCmtShortcut, &QShortcut::activated, this, [this]{ showSameNameObjOrObjsFromClec(COMMENTS); });
    connect(sameEqShortcut,  &QShortcut::activated, this, [this]{ showSameNameObjOrObjsFromClec(EQUATIONS); });
    connect(sameIdtShortcut, &QShortcut::activated, this, [this]{ showSameNameObjOrObjsFromClec(IDENTITIES); });
    connect(sameLstShortcut, &QShortcut::activated, this, [this]{ showSameNameObjOrObjsFromClec(LISTS); });
    connect(sameTblShortcut, &QShortcut::activated, this, [this]{ showSameNameObjOrObjsFromClec(TABLES); });
    connect(varsFromClecShortcut,    &QShortcut::activated, this, [this]{ showSameNameObjOrObjsFromClec(VARIABLES); });
    connect(scalarsFromClecShortcut, &QShortcut::activated, this, [this]{ showSameNameObjOrObjsFromClec(SCALARS); });

    // get list of related objects
	relatedCmtShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F1), this);
	relatedEqShortcut  = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F2), this);
	relatedIdtShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F3), this);
	relatedLstShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F4), this);
	relatedSclShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F5), this);
	relatedTblShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F6), this);
	relatedVarShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F7), this);

    relatedCmtShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    relatedEqShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    relatedIdtShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    relatedLstShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    relatedSclShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    relatedTblShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    relatedVarShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    connect(relatedCmtShortcut, &QShortcut::activated, this, [this]{ showRelatedObjs(COMMENTS); });
    connect(relatedEqShortcut , &QShortcut::activated, this, [this]{ showRelatedObjs(EQUATIONS); });
    connect(relatedIdtShortcut, &QShortcut::activated, this, [this]{ showRelatedObjs(IDENTITIES); });
    connect(relatedLstShortcut, &QShortcut::activated, this, [this]{ showRelatedObjs(LISTS); });
    connect(relatedSclShortcut, &QShortcut::activated, this, [this]{ showRelatedObjs(SCALARS); });
    connect(relatedTblShortcut, &QShortcut::activated, this, [this]{ showRelatedObjs(TABLES); });
    connect(relatedVarShortcut, &QShortcut::activated, this, [this]{ showRelatedObjs(VARIABLES); });
}

IodeAbstractTableView::~IodeAbstractTableView()
{
    delete objectNameEdit;
	delete shortcutEditEnter;
	delete shortcutEditReturn;

    delete deleteShortcut;

	delete sameCmtShortcut;
	delete sameEqShortcut;
	delete sameIdtShortcut;
	delete sameLstShortcut;
	delete scalarsFromClecShortcut;
	delete sameTblShortcut;
	delete varsFromClecShortcut;

    delete relatedCmtShortcut;
    delete relatedEqShortcut;
    delete relatedIdtShortcut;
    delete relatedLstShortcut;
    delete relatedSclShortcut;
    delete relatedTblShortcut;
    delete relatedVarShortcut;
}

void IodeAbstractTableView::keyPressEvent(QKeyEvent* event)
{
    // hides the QLineEdit to edit the name of an IODE object if the user pressed ESCAPE
    if(objectNameEdit->isVisible() && event->key() == Qt::Key_Escape)
    {
        objectNameEdit->setHidden(true);
        objectNameEdit->setText("");
    }

    // scrolls to the first row in which the IODE object name starts with the pressed key
    if(!objectNameEdit->isVisible() && !event->text().isEmpty())
    {   
        QChar c = event->text()[0];
        if((c.isLetter() && event->count() == 1) || c == '_')
        {
            int row = 0;
            while(row < verticalHeader()->count())
            {
                QString name = model()->headerData(row, Qt::Vertical).toString();
                if(name.startsWith(c, Qt::CaseInsensitive))
                {
                    scrollTo(model()->index(row, 0));
                    selectRow(row);
                    break;
                }
                row++;
            }
            return;
        }
    }

    QTableView::keyPressEvent(event);
}

void IodeAbstractTableView::removeObjects()
{
    QModelIndexList selection = selectionModel()->selectedRows();
    // Note: goes in reverse order because each deleted row shift all the next rows
    for (int i = selection.count() - 1; i >= 0; i--) model()->removeRow(selection.at(i).row(), QModelIndex());
    update();
}

// adapted from https://www.qtcentre.org/threads/42388-Make-QHeaderView-Editable?p=224375#post224375 
void IodeAbstractTableView::openEditorName(int section)
{
    // This block sets up the geometry for the line edit
    QHeaderView* vheader = verticalHeader();
    int y = horizontalHeader()->height() + vheader->sectionViewportPosition(section);
    QRect geometry(0, y, vheader->width(), vheader->sectionSize(section));
    objectNameEdit->setGeometry(geometry);

    QString name = model()->headerData(section, Qt::Vertical).toString();
    objectNameEdit->setText(name);

    objectNameEdit->setHidden(false);
    // objectNameEdit->blockSignals(False);
    objectNameEdit->setFocus();
    objectNameEdit->selectAll();
    sectionEdited = section;
}

// adapted from https://www.qtcentre.org/threads/42388-Make-QHeaderView-Editable?p=224375#post224375
void IodeAbstractTableView::editName()
{
    QString oldName = model()->headerData(sectionEdited, Qt::Vertical).toString();
    QString newName = objectNameEdit->text();
    if(newName != oldName)
        model()->setHeaderData(sectionEdited, Qt::Vertical, newName);

    objectNameEdit->setHidden(true);
    objectNameEdit->setText("");
    //objectNameEdit->blockSignals(true);
    this->setFocus();
}

void IodeAbstractTableView::print()
{
	QPrintPreviewDialog dialog(&printer);
    connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &IodeAbstractTableView::renderForPrinting);
	dumpTableInDocument();
	dialog.exec();
}
