#include "tab_text.h"


TextWidget::TextWidget(const IodeFileType fileType, const QString& filepath, QWidget* parent): 
    AbstractTextWidget(fileType, filepath, parent) 
{
    setupUi(this);
    splitter_ = splitter;
    editor_ = editor;
    editor_2_ = editor_2;
    // synchronizes the content of the two text editors
    editor_2->setDocument(editor->document());
    
    editor_2->hide();

    filter = "Text files (*" + showInTextTabExtensionsList.join(", *") + ")";

    connect(editor, &IodeTextEditor::modificationChanged, this, &TextWidget::setModified);

    if(!filepath.isEmpty()) 
        load(filepath, true);
}
