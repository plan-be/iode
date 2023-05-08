#pragma once

#include <QPrinter>
#include <QCheckBox>
#include <QShortcut>
#include <QSpacerItem>
#include <QPushButton>
#include <QPrintPreviewDialog>

#include "utils.h"
#include "tab_text.h"
#include "ui_tab_report.h"
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
class QIodeReportWidget : public QIodeAbstractEditor, public Ui::QIodeReportWidget
{
    Q_OBJECT

public:
    QIodeReportWidget(const QString& filepath,  QTextEdit* output, 
        std::shared_ptr<QIodeCompleter>& completer, QWidget* parent = nullptr);

    bool load_(const QString& filepath, const bool forceOverwrite)
    {
        return load__(editor, filepath, forceOverwrite);
    }

    QString save(const QString& filepath)
    {
        return save_(editor, filepath);
    }

signals:
    void askComputeHash(bool);

private slots:
    void run()
    {
        emit askComputeHash(true);
        static_cast<ReportEditor*>(editor)->run(filepath);
        emit askComputeHash(false);
    }

    void renderForPrinting()
    {
        editor->print(&printer);
    }

public slots:
    void print()
    {
        try
        {
            QPrintPreviewDialog dialog(&printer);
            connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &QIodeReportWidget::renderForPrinting);
            dialog.exec(); 
        }
        catch(const std::exception& e)
        {
            QMessageBox::warning(nullptr, "Print Graph", "Could not print graph\n" + QString(e.what()));
        }
    }
};
