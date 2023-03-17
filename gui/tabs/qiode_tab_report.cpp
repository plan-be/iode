#include "qiode_tab_report.h"

QIodeReportWidget::QIodeReportWidget(const QString& filepath, QTextEdit* output, std::shared_ptr<QIodeCompleter>& completer, 
    QWidget* parent) : QIodeAbstractEditor(I_REPORTS_FILE, filepath, parent)
{
    filter = "IODE report files (*" + QString::fromStdString(report_ext) + ")";

    // spacer
    QSpacerItem* horizontalSpacer = new QSpacerItem(800, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addItem(horizontalSpacer, 0, 0);

    // add Run button
    QPushButton* runButton = new QPushButton("Run");
    runButton->setObjectName(QString::fromUtf8("runButton"));
    QSizePolicy sizePolicyAdd(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicyAdd.setHorizontalStretch(0);
    sizePolicyAdd.setVerticalStretch(0);
    sizePolicyAdd.setHeightForWidth(runButton->sizePolicy().hasHeightForWidth());
    runButton->setSizePolicy(sizePolicyAdd);
    layout->addWidget(runButton, 0, 1, Qt::AlignRight);

    // add report editor
    editor = new ReportEditor(completer, output, parent);
    addEditorToLayout(1);

    // ---- Signals and Slots ----
    connect(editor, &TextEditor::modificationChanged, this, &QIodeReportWidget::setModified);

    runShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_R), this);
    connect(runShortcut, &QShortcut::activated, this, &QIodeReportWidget::run);

    connect(runButton, &QPushButton::clicked, this, &QIodeReportWidget::run);
}

QIodeReportWidget::~QIodeReportWidget()
{
    delete runShortcut;
}
