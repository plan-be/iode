#pragma once

#include <QCheckBox>
#include <QShortcut>
#include <QSpacerItem>
#include <QPushButton>

#include "utils.h"
#include "qiode_tab_text.h"
#include "custom_widgets/report_editor.h"


/**
 * @brief Allows to edit and run an Iode report.
 *
 *        Implemented features:
 *
 *        - Syntax color highlight.
 *        - Auto-completion on $/#/@-functions + all Iode objects names.
 *        - Shortcuts:
 *          -> CTRL + R runs the report.
 * 
 */
class QIodeReportWidget : public QIodeAbstractEditor
{
    Q_OBJECT

    QShortcut* runShortcut;

public:
    QIodeReportWidget(const QString& filepath,  QTextEdit* output, 
        std::shared_ptr<QIodeCompleter>& completer, QWidget* parent = nullptr);
    ~QIodeReportWidget();

signals:
    void askComputeHash(bool);

private slots:
    void run()
    {
        emit askComputeHash(true);
        static_cast<ReportEditor*>(editor)->run(filepath);
        emit askComputeHash(false);
    }

    void toggleAutocomplete(int state)
    {
        static_cast<ReportEditor*>(editor)->enableAutocomplete(state == Qt::Checked);
    }
};
