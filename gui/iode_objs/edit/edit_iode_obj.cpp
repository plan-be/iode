#include "edit_iode_obj.h"


EditObjectDialog::EditObjectDialog(const IodeTypes iodeType, const QString& name, QWidget* parent) : 
    QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint), 
    iodeType(iodeType), name(name.toStdString()), completer(nullptr)
{
	setupUi(this);

    setWindowTitle("Editing " + QString::fromStdString(v_iode_types[iodeType]) + " " + name);
	textEdit_content->setLineWrapMode(QPlainTextEdit::LineWrapMode::WidgetWidth);

    lineEdit_name->setText(name);

    if(iodeType == IDENTITIES || iodeType == LISTS)
    {
        completer = new IodeCompleter(false, false, {SCALARS, VARIABLES}, textEdit_content);
        textEdit_content->setCompleter(completer);
    }

    fullScreenShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_X), this);
    fullScreenShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);

    connect(fullScreenShortcut, &QShortcut::activated, this, &EditObjectDialog::showMaximized);
}

EditObjectDialog::~EditObjectDialog()
{
	delete fullScreenShortcut;

    if(completer) 
        delete completer;
}
