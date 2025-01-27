#pragma once
#include <QPrinter>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QGridLayout>
#include <QPlainTextEdit>
#include <QPrintPreviewDialog>

#include "tab_abstract.h"
#include "text_edit/text_editor.h"


class AbstractTextWidget: public IodeAbstractWidget
{
    Q_OBJECT

protected:
    QString filepath;
    QString filter;
    QPrinter printer;

    QPlainTextEdit* editor_;
    QPlainTextEdit* editor_2_;

signals:
    void tabTextModified(const QString& filepath, const bool modified);

public:
    AbstractTextWidget(const IodeFileType fileType, const QString& filepath, QWidget* parent = nullptr);

    QString getFilepath() const override
    { 
        return filepath; 
    }

    bool updateFilepath(const QString& filepath) override
    {
        if(checkNewFilepath(filepath))
        {
            this->filepath = filepath;
            setModified(false);
            return true;
        }
        else
            return false;
    }

    void update() {}
    bool load__(IodeTextEditor* editor, const QString& filepath, const bool forceOverwrite);
    virtual bool load_(const QString& filepath, const bool forceOverwrite) = 0;
    QString save_(IodeTextEditor* editor, const QString& filepath);
    virtual QString save(const QString& filepath) = 0;
    QString save();
    QString saveAs_();

    void split(const Qt::Orientation orientation) override;
    void unsplit() override;

public slots:
    void setModified(bool modified) override
    {
        this->modified = modified;
        emit tabTextModified(filepath, modified); 
    }
};
