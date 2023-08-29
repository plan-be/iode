#include "tab_report.h"


ReportWidget::ReportWidget(const QString& filepath, QWidget* parent): 
    AbstractTextWidget(I_REPORTS_FILE, filepath, parent)
{
    setupUi(this);
    splitter_ = splitter;
    editor_ = editor;
    editor_2_ = editor_2;
    // synchronizes the content of the two Report editors
    editor_2->setDocument(editor->document());

    // setup the editors
    editor->setup();
    editor_2->setup();

    editor_2->hide();

    QList<QString> q_langs;
    for(const std::string& lang: vLangs) q_langs << QString::fromStdString(lang);

    comboBox_language->addItems(q_langs);
    comboBox_language->setEditable(false);

    filter = "IODE report files (*" + QString::fromStdString(report_ext) + ")";

    // autocomplete checkbox
    /*
    QCheckBox* autocomplete = new QCheckBox("autocomplete", this);
    autocomplete->setChecked(true);
    */

    // ---- Signals and Slots ----
    connect(editor, &IodeTextEditor::modificationChanged, this, &ReportWidget::setModified);

    // connect(autocomplete, &QCheckBox::stateChanged, this, &ReportWidget::toggleAutocomplete);

    if(!filepath.isEmpty()) 
        load(filepath, true);
}
