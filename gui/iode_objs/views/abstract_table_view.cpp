#include "abstract_table_view.h"


AbstractTableView::AbstractTableView(EnumIodeType iodeType, BaseDelegate* delegate, QWidget* parent) 
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
    verticalHeader()->setStretchLastSection(true);

    // stylesheet
    setStyleSheet(stylesheet);

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

    connect(deleteShortcut, &QShortcut::activated, this, &AbstractTableView::removeObjects);

    // for editing IODE object names
    objectNameEdit = new QLineEdit(this);
    QString acceptedLetters;
    if (iodeType == I_COMMENTS)
        acceptedLetters = "A-Za-z";
    else if (iodeType == I_SCALARS)
        acceptedLetters = "a-z";
    else
        acceptedLetters = "A-Z";
	QRegularExpression re(QString("[%1_][0-9%1_]{0,%2}").arg(acceptedLetters).arg(QString::number(K_MAX_NAME - 1)));
	QRegularExpressionValidator* validator = new QRegularExpressionValidator(re, this);
	objectNameEdit->setValidator(validator);
    objectNameEdit->setHidden(true);

    connect(this->verticalHeader(), &QHeaderView::sectionDoubleClicked, this, &AbstractTableView::openEditorName);
    connect(objectNameEdit, &QLineEdit::returnPressed, this, &AbstractTableView::editName);
    connect(objectNameEdit, &QLineEdit::editingFinished, this, &AbstractTableView::closeNameEditor);
}

AbstractTableView::~AbstractTableView()
{
    delete deleteShortcut;
    delete objectNameEdit;
}

void AbstractTableView::keyPressEvent(QKeyEvent* event)
{
    if(objectNameEdit->isVisible() && event->key() == Qt::Key_Escape)
    {
        objectNameEdit->setHidden(true);
        objectNameEdit->setText("");
    }
    QTableView::keyPressEvent(event);
}

void AbstractTableView::removeObjects()
{
    QModelIndexList selection = selectionModel()->selectedRows();
    // Note: goes in reverse order because each deleted row shift all the next rows
    for (int i = selection.count() - 1; i >= 0; i--) model()->removeRow(selection.at(i).row(), QModelIndex());
    update();
}

// adapted from https://www.qtcentre.org/threads/42388-Make-QHeaderView-Editable?p=224375#post224375 
void AbstractTableView::openEditorName(int section)
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
void AbstractTableView::editName()
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
