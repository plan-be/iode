#pragma once
#include "../api/iode.h"

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
 *           CONSTANTS            *  
 * ****************************** */

// TODO: move the 3 constants below to IODE API
// constexpr is better then #define (https://stackoverflow.com/questions/42388077/constexpr-vs-macros)
constexpr char ORGANIZATION_NAME[] = "Federal Planning Bureau";
constexpr char APPLICATION_NAME[] = "IODE";
constexpr char DEFAULT_INSTALLATION_DIR[] = "c:/iode";               // see function HLP_filename() in dos/o_help.c
constexpr char IODE_WEBSITE[] = "https://iode.plan.be/doku.php";


/* ****************************** *
 *        ENUMS AND MAPS          *
 * ****************************** */

/* NOTE FOR THE DEVELOPPERS:
 * enum documentation: https://en.cppreference.com/w/cpp/language/enum
 */

// TODO: use scoped enums (C++) instead of C enums ?

// TODO: Move this enum to IODE API
enum EnumIodeType 
{
    Comments,
    Equations,
    Identities,
    Lists,
    Scalars,
    Tables,
    Variables
};
constexpr int nbIodeTypes = 7;

const static QMap<QString, int> qmapIodeTypes =
{
    {"Comments", Comments},
    {"Equations", Equations},
    {"Identities", Identities},
    {"Lists", Lists},
    {"Scalars", Scalars},
    {"Tables", Tables},
    {"Variables", Variables}
};

// TODO: Move this enum + array to IODE API
enum EnumLang
{
    English,
    French,
    Dutch
};
constexpr int nbLangs = 3;

const static QMap<QString, int> qmapLangs =
{
    {"English", English},
    {"French", French},
    {"Dutch", Dutch}
};

enum EnumItemType
{
    Required,
    Optional
};

// TODO: Move this enum to IODE API
enum EnumIodeFile
{
    CommentsFile,
    EquationsFile,
    IdentitiesFile,
    ListsFile,
    ScalarsFile,
    TablesFile,
    VariablesFile,
    SettingsFile,
    LogsFile,
    AnyFile
};
constexpr int nbIodeFiles = 10;

struct IodeFileExtension
{
    QString name;
    QString ext;
};

const static QMap<EnumIodeFile, IodeFileExtension> qmapIodeFileExtension =
{
    {CommentsFile, {"Comments", "cmt"}},
    {EquationsFile, {"Equations", "eqs"}},
    {IdentitiesFile, {"Identities", "idt"}},
    {ListsFile, {"Lists", "lst"}},
    {ScalarsFile, {"Scalars", "scl"}},
    {TablesFile, {"Tables", "tbl"}},
    {VariablesFile, {"Variables", "var"}},
    {LogsFile, {"Logs", "log"}},
    {SettingsFile, {"Settings", "ini"}},
    {AnyFile, {"Any", "*"}}
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

inline QSettings* get_local_settings()
{
    QSettings* settings = new QSettings("settings.ini", QSettings::IniFormat);
    return settings;
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
class MixinSettings : public QDialog
{
    Q_OBJECT

protected:
    QSettings& settings;
    QString className;
    QMap<QString, BaseWrapper*> mapFields;

public:
    MixinSettings(QSettings& settings, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) : 
        QDialog(parent, f), settings(settings), className(""){ }
    ~MixinSettings() { }

protected:
    void saveSettings()
    {
        settings.beginGroup(className);
        QMap<QString, BaseWrapper*>::iterator i;
        for (i = mapFields.begin(); i != mapFields.end(); ++i)
        {
            QString name = i.key();
            BaseWrapper* field = i.value();
            QVariant value = field->getQValue();
            settings.setValue(name, value);
        }
        settings.endGroup();
    }

    void loadSettings()
    {
        settings.beginGroup(className);
        QMap<QString, BaseWrapper*>::iterator i;
        for (i = mapFields.begin(); i != mapFields.end(); ++i)
        {
            QString name = i.key();
            BaseWrapper* field = i.value();
            QVariant value = settings.value(name).toString();
            field->setQValue(value);
        }
        settings.endGroup();
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