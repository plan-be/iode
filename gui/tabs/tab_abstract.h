#pragma once

#include <QObject>
#include <QWidget>
#include <QFileInfo>
#include <QMessageBox>
#include <QStringList>

#include "utils.h"


// tab prefix per file type
// Note: If the tab's label contains an ampersand, the letter following the ampersand is used as a shortcut for the tab, 
//       e.g. if the label is "Bro&wse" then Alt+W becomes a shortcut which will move the focus to this tab.
//       See https://doc.qt.io/qt-6/qtabwidget.html#addTab 
const static QVector<QString> tabPrefix({"(&CMT) ", "(&EQS) ", "(&IDT) ", "(&LST) ", 
                                         "(&SCL) ", "(&TBL) ", "(&VAR) ", 
                                         "(REP) ", "(TXT) ", "(ASCII) ", "(A2M) ", 
                                         "(RTF) ", "(HTML) ", "(MIF) ", "(CSV) ",
                                         "(REF) ", "(AGL) ", "(DIF) ", "(LOG) ", 
                                         "(SET) "});

class AbstractTabWidget: public QWidget
{
    Q_OBJECT

protected:
    EnumIodeFile fileType;
    QString filepath;
    bool modified;

protected:
    virtual bool load_(const QString& filepath, const bool forceOverwrite) = 0;
    virtual QString saveAs_() = 0;

signals:
    void tabContentModified(const QString& filepath, const bool modified);

public:
    AbstractTabWidget(const EnumIodeFile fileType, const QString& filepath="", QWidget* parent = nullptr) : 
        QWidget(parent), fileType(fileType), filepath(filepath), modified(false)
    {
        this->setGeometry(QRect(10, 11, 951, 26));
    }

    EnumIodeFile getFiletype() const 
    { 
        return fileType; 
    }

    QString getFilepath() const 
    { 
        return filepath; 
    }

    bool isModified() const
    {
        return modified;
    }

    virtual QString getTabText() const
    {
        QString text = tabPrefix[fileType] + QFileInfo(filepath).fileName();
        if(modified) text += "*";
        return text;
    }

    virtual QString getTooltip() const
    {
        return QFileInfo(filepath).absoluteFilePath();
    }
    
    bool load(const QString& filepath, const bool forceOverwrite)
    {
        bool success = load_(filepath, forceOverwrite);
        if(success)
        {
            updateFilepath(filepath);
            setModified(false);
        }
        return success;
    }

    QString saveAs()
    {
        QString newFilepath = saveAs_();
        if(!newFilepath.isEmpty())
        {
            updateFilepath(newFilepath);
            setModified(false);
        }
        return newFilepath;
    }

    /**
     * @brief Update the filepath associated with the current tab.
     * 
     * @param filepath QString New filepath.
     * @return bool Update successful.
     * 
     * @note Need to be called each time a file is moved or renamed.
     */
    virtual bool updateFilepath(const QString& filepath)
    {
        QFileInfo fileInfo(filepath);

        // check if file exists
        if(!fileInfo.exists())
        {
            QMessageBox::warning(nullptr, "WARNING", "File " + filepath + " could not been found");
            return false;
        }

        // check if correct extension
        if(get_iode_file_type(filepath.toStdString()) != fileType)
        {
            QString filename = fileInfo.fileName(); 
            QString ext = fileInfo.suffix();
            QStringList expected_ext;
            for(const std::string& ext_: get_extensions(fileType)) expected_ext << QString::fromStdString(ext_);

            QMessageBox::warning(nullptr, "WARNING", "Expected file with extension " + expected_ext.join(" or ") + "\n" +
                "But got file " + filename + " with extension " + ext);
            return false;
        }

        // update filepath
        this->filepath = filepath;
        return true;
    }

    virtual void update() = 0;
    virtual QString save() = 0;

public slots:
    void setModified(bool modified)
    {
        this->modified = modified;
        emit tabContentModified(filepath, modified); 
    }
};
