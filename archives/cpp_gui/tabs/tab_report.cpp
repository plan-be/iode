#include "tab_report.h"


ReportWidget::ReportWidget(const QString& filepath, QWidget* parent): 
    AbstractTextWidget(FILE_REP, filepath, parent)
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
    for(const std::string& lang: v_table_langs) q_langs << QString::fromStdString(lang);

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

void ReportWidget::loadSettings(const QSettings* project_settings)
{
    if(!project_settings)
        return;

    QString parameters = project_settings->value("report_parameters", "").toString();
    int languageIndex = project_settings->value("report_language", 0).toInt();
    int nbDecimals = project_settings->value("report_nbDecimals", 2).toInt();

    lineEdit_parameters->setText(parameters);
    comboBox_language->setCurrentIndex(languageIndex);
    spinBox_nbDecimals->setValue(nbDecimals);
}

void ReportWidget::saveSettings(QSettings* project_settings)
{
    if(!project_settings)
        return;

    QString parameters = lineEdit_parameters->text();
    int languageIndex = comboBox_language->currentIndex();
    int nbDecimals = spinBox_nbDecimals->value();

    project_settings->setValue("report_parameters", parameters);
    project_settings->setValue("report_language", languageIndex);
    project_settings->setValue("report_nbDecimals", nbDecimals);
}
