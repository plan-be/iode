#include "qiode_tab_report.h"

QIodeReportWidget::QIodeReportWidget(const QString& filepath, QTextEdit* output, std::shared_ptr<QIodeCompleter>& completer, 
    QWidget* parent) : QIodeAbstractEditor(I_REPORTS_FILE, filepath, parent)
{
    filter = "IODE report files (*" + QString::fromStdString(report_ext) + ")";

    int col = 0;

    // autocomplete checkbox
    /*
    QCheckBox* autocomplete = new QCheckBox("autocomplete", this);
    autocomplete->setChecked(true);
    autocomplete->setObjectName(QString::fromUtf8("autocomplete"));
    QSizePolicy sizePolicyCompleter(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicyCompleter.setHorizontalStretch(0);
    sizePolicyCompleter.setVerticalStretch(0);
    sizePolicyCompleter.setHeightForWidth(autocomplete->sizePolicy().hasHeightForWidth());
    autocomplete->setSizePolicy(sizePolicyCompleter);
    layout->addWidget(autocomplete, 0, col, Qt::AlignLeft);
    col++;
    */

    // spacer
    QSpacerItem* horizontalSpacer = new QSpacerItem(720, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addItem(horizontalSpacer, 0, col);
    col++;

    // Run button
    QPushButton* runButton = new QPushButton("Run", this);
    runButton->setObjectName(QString::fromUtf8("runButton"));
    QSizePolicy sizePolicyRun(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicyRun.setHorizontalStretch(0);
    sizePolicyRun.setVerticalStretch(0);
    sizePolicyRun.setHeightForWidth(runButton->sizePolicy().hasHeightForWidth());
    runButton->setSizePolicy(sizePolicyRun);
    layout->addWidget(runButton, 0, col, Qt::AlignRight);
    col++;

    // add report editor
    editor = new ReportEditor(completer, output, parent);
    addEditorToLayout(1);

    // ---- Signals and Slots ----
    connect(editor, &TextEditor::modificationChanged, this, &QIodeReportWidget::setModified);

    runShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_R), this);
    connect(runShortcut, &QShortcut::activated, this, &QIodeReportWidget::run);
    connect(runButton, &QPushButton::clicked, this, &QIodeReportWidget::run);

    // connect(autocomplete, &QCheckBox::stateChanged, this, &QIodeReportWidget::toggleAutocomplete);
}

QIodeReportWidget::~QIodeReportWidget()
{
    delete runShortcut;
}
