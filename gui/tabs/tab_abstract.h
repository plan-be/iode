#pragma once

#include <QObject>
#include <QWidget>
#include <QFileInfo>
#include <QSplitter>
#include <QMessageBox>
#include <QStringList>

#include "utils.h"


class IodeAbstractWidget: public QWidget
{
    Q_OBJECT

protected:
    IodeFileType fileType;

    QVector<QString> tabPrefix; ///< tab prefix per file type

    bool modified;
    bool forcedAsText_;         ///< Whether or not the corresponding file has been forced to open as a text tab
    bool savingFile;            ///< See save() and load() implementation of AbstractTextWidget 
    bool splitted_;

    QSplitter* splitter_;

protected:
    virtual bool load_(const QString& filepath, const bool forceOverwrite) = 0;
    virtual QString saveAs_() = 0;

    bool checkNewFilepath(const QString& filepath)
    {
        try
        {
            check_filepath(filepath.toStdString(), fileType, "load", true);
        }
        catch(const std::exception& e)
        {
            QMessageBox::warning(nullptr, tr("WARNING"), tr(e.what()));
            return false;
        }
        return true;
    }

public:
    IodeAbstractWidget(const IodeFileType fileType, QWidget* parent = nullptr): QWidget(parent), 
        fileType(fileType), modified(false), forcedAsText_(false), savingFile(false), splitted_(false)
    {
        this->setGeometry(QRect(10, 11, 951, 26));

        // Note: If the tab's label contains an ampersand, the letter following the ampersand is used as a shortcut for the tab, 
        //       e.g. if the label is "Bro&wse" then Alt+W becomes a shortcut which will move the focus to this tab.
        //       See https://doc.qt.io/qt-6/qtabwidget.html#addTab 
        QString ext;
        std::vector<std::string> v_ext;
        for(int i = 0; i < v_file_types.size(); i++)
        {
            v_ext = v_file_types[i].v_ext;
            if(v_ext.size() > 0)
            {
                ext = QString::fromStdString(v_ext[0]);
                // remove dot in front of extension + to upper case
                ext = ext.sliced(1).toUpper();
                // prepend extension with an & if it represents an IODE database 
                if(i < FILE_VARIABLES)
                    ext = "&" + ext;
                tabPrefix.append("(" + ext + ") ");
            }
            else
                tabPrefix.append("");
        }
    }

    IodeFileType getFiletype() const 
    { 
        return fileType; 
    }

    virtual QString getFilepath() const = 0;

    bool isModified() const
    {
        return modified;
    }

    bool forcedAsText() const
    {
        return forcedAsText_;
    }

    void setForcedAsText(const bool value)
    {
        forcedAsText_ = value;
    }

    // NOTE:
    // splitted() must not call the isVisible() method on the second widget of the 
    // QSplitter object to check if the tab has been splitted.
    // As the documentation of QWidget::visible() says: 
    // "If an ancestor is not visible, the widget won't become visible until all its ancestors are shown."
    // In other words, isVisible() will always return false if the splitted() is called on a tab 
    // which is not the current open tab.
    bool splitted() const
    {
        return splitted_;
    }

    Qt::Orientation getSplitOrientation() const
    {
        return splitter_->orientation();
    }

    virtual QString getTabText() const
    {
        QString filepath = getFilepath();
        QString text = tabPrefix[fileType] + QFileInfo(filepath).fileName();
        if(modified) text += "*";
        return text;
    }

    virtual QString getTooltip() const
    {
        QString filepath = getFilepath();
        return QFileInfo(filepath).absoluteFilePath();
    }
    
    bool load(const QString& filepath, const bool forceOverwrite)
    {
        bool success = load_(filepath, forceOverwrite);
        if(success)
            success = updateFilepath(filepath);
        return success;
    }

    QString saveAs()
    {
        QString newFilepath = saveAs_();

        if(newFilepath.isEmpty())
            return "";
        
        bool success = updateFilepath(newFilepath);
        return success ? newFilepath : "";
    }

    virtual void loadSettings(const QSettings* project_settings) {}
    virtual void saveSettings(QSettings* project_settings) {}

    /**
     * @brief Update the filepath associated with the current tab.
     * 
     * @param filepath QString New filepath.
     * @return bool Update successful.
     * 
     * @note Need to be called each time a file is moved or renamed.
     */
    virtual bool updateFilepath(const QString& filepath) = 0;

    virtual void update() = 0;
    virtual QString save() = 0;

    virtual void split(const Qt::Orientation orientation) = 0;
    virtual void unsplit() = 0;

public slots:
    virtual void setModified(bool modified) = 0;
};
