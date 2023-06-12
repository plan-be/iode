#pragma once

#include "tab_text_abstract.h"
#include "ui_tab_text.h"


class QIodeTextWidget : public QIodeAbstractEditor, public Ui::QIodeTextWidget
{
    Q_OBJECT

    const static QStringList textExtensions;

public:
    QIodeTextWidget(const EnumIodeFile fileType, const QString& filepath, QWidget* parent = nullptr) 
        : QIodeAbstractEditor(fileType, filepath, parent) 
    {
        setupUi(this);

        filter = "Text files (*" + textExtensions.join(", *") + ")";

        connect(editor, &TextEditor::modificationChanged, this, &QIodeTextWidget::setModified);

        if(!filepath.isEmpty()) 
            load(filepath, true);
    }

    static bool isTextExtension(const QString& ext)
    {
        return textExtensions.contains(ext);
    }

    bool load_(const QString& filepath, const bool forceOverwrite)
    {
        return load__(editor, filepath, forceOverwrite);
    }

    QString save(const QString& filepath)
    {
        return save_(editor, filepath);
    }

private slots:
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
            connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &QIodeTextWidget::renderForPrinting);
            dialog.exec(); 
        }
        catch(const std::exception& e)
        {
            QMessageBox::warning(nullptr, "Print Graph", "Could not print graph\n" + QString(e.what()));
        }
    }
};
