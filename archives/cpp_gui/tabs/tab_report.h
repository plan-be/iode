#pragma once

#include "utils.h"
#include "wrapper_classes.h"
#include "tab_text_abstract.h"
#include "ui_tab_report.h"
#include "main_window_abstract.h"
#include "text_edit/report_editor.h"


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
class ReportWidget : public AbstractTextWidget, public Ui::ReportWidget
{
    Q_OBJECT

public:
    ReportWidget(const QString& filepath, QWidget* parent = nullptr);

    bool load_(const QString& filepath, const bool forceOverwrite)
    {
        return load__(editor, filepath, forceOverwrite);
    }

    QString save(const QString& filepath)
    {
        return save_(editor, filepath);
    }

    void loadSettings(const QSettings* project_settings) override;
    void saveSettings(QSettings* project_settings) override;

signals:
    void askComputeHash(bool);

private slots:
    void run()
    {
        emit askComputeHash(true);

        // Save the current content of the report tab because the user may have 
        // made some modifications in the report editor which are not saved yet.
        save(filepath);

        QString parameters = lineEdit_parameters->text();
        TableLang language = (TableLang) comboBox_language->currentIndex();
        int nbDecimals = spinBox_nbDecimals->value();
        static_cast<IodeReportEditor*>(editor)->run(filepath, parameters, nbDecimals, language);

        MainWindowAbstract* main_window = static_cast<MainWindowAbstract*>(get_main_window_ptr());
        main_window->update_tab_and_completer();
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
            connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &ReportWidget::renderForPrinting);
            dialog.exec(); 
        }
        catch(const std::exception& e)
        {
            QMessageBox::warning(nullptr, "Print Graph", "Could not print graph\n" + QString(e.what()));
        }
    }
};
