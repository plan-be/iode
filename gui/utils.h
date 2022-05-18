#pragma once

#pragma once
#include "../cpp_api/iode_cpp_api.h"

#include <string>

#include <QObject>
#include <QVariant>
#include <QString>
#include <QVector>
#include <QList>
#include <QMap>
#include <QUrl>
#include <QDir>
#include <QDialog>
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDesktopServices>

#include <string>
#include <typeinfo>      // typeid
#include <unordered_map>


/* ****************************** *
 *          GLOBAL VARS           *
 * ****************************** */

QString settings_filepath = "settings.ini";

/* ****************************** *
 *        ENUMS AND MAPS          *
 * ****************************** */

/* NOTE FOR THE DEVELOPPERS:
 * enum documentation: https://en.cppreference.com/w/cpp/language/enum
 */

// OPTIONAL is a protected keyword -> must add _FIELD
enum EnumItemType
{
    REQUIRED_FIELD,
    OPTIONAL_FIELD
};


/* ****************************** *
 *          FUNCTIONS             *
 * ****************************** */

// equivalent to function HLP_filename() in dos/o_help.c
inline const QUrl get_url_iode_helpfile(const QString& filename)
{
    // try to find the help file in the application directory
    QString filepath = QDir(QCoreApplication::applicationDirPath()).filePath(filename);
    // if doesn't exist, build a path suing the default iode installation directory 
    if (!QFileInfo(filepath).exists()) filepath = QDir(DEFAULT_INSTALLATION_DIR).filePath(filename);
    filepath = "file:///" + filepath;
    return QUrl(filepath);
}

inline const QUrl get_url_iode_manual()
{
    return get_url_iode_helpfile("iode.chm");
}


/* ****************************** *
 *         BASE CLASSES           *
 * ****************************** */

/* NOTE FOR THE DEVELOPPERS:
 * Because of how Qt is implemented, it is NOT possible for a template class
 * to define signals and slots (using Q_OBJECT)
 * ( see https://doc.qt.io/qt-5/moc.html#limitations )
 */


// Base class for Wrapper classes dedicated to be used in MixingSettings
struct BaseWrapper
{
    virtual QVariant getQValue() = 0;
    virtual void setQValue(const QVariant& qvalue) = 0;
};


// Mixin class for handling settings in derived class. 
class QIodeSettings : public QDialog
{
    Q_OBJECT

protected:
    QSettings* settings;
    QString className;
    QMap<QString, BaseWrapper*> mapFields;

public:
    QIodeSettings(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) :
        QDialog(parent, f), className("") 
    {
        settings = new QSettings(settings_filepath, QSettings::IniFormat);
    }

    ~QIodeSettings() 
    {
        delete settings;
    }

protected:
    void saveSettings()
    {
        settings->beginGroup(className);
        QMap<QString, BaseWrapper*>::iterator i;
        for (i = mapFields.begin(); i != mapFields.end(); ++i)
        {
            QString name = i.key();
            BaseWrapper* field = i.value();
            QVariant value = field->getQValue();
            settings->setValue(name, value);
        }
        settings->endGroup();
    }

    void loadSettings()
    {
        settings->beginGroup(className);
        QMap<QString, BaseWrapper*>::iterator i;
        for (i = mapFields.begin(); i != mapFields.end(); ++i)
        {
            QString name = i.key();
            BaseWrapper* field = i.value();
            QVariant value = settings->value(name).toString();
            field->setQValue(value);
        }
        settings->endGroup();
    }

    void closeEvent(QCloseEvent* event) override
    {
        saveSettings();
        event->accept();
    }

public slots:
    void help()
    {
        QUrl url = get_url_iode_manual();
        QDesktopServices::openUrl(url);
    }

    void accept() override
    {
        saveSettings();
        QDialog::accept();
    }

    void reject() override
    {
        saveSettings();
        QDialog::reject();
    }

};
