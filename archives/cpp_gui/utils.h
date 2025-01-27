#pragma once
#include "cpp_api/iode_cpp_api.h"

#include <string>
#include <memory>   // for the smart pointers

#include <QMap>
#include <QUrl>
#include <QDir>
#include <QList>
#include <QObject>
#include <QWidget>
#include <QString>
#include <QVector>
#include <QDialog>
#include <QVariant>
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

inline QString executable_dir;
inline QUrl url_manual;
inline QUrl url_readme;
inline QUrl url_shortcuts;
const static QString defaultFontFamily = "Consolas, \"Courier New\", monospace";
const static int MAX_PRECISION_NUMBERS = 10;
const static QStringList showInTextTabExtensionsList = { "txt", "a2m", "prf", "dif", "asc", "ref" };


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

// defined in main_window.cpp
QWidget* get_main_window_ptr();

// defined in tab_widget.cpp
QWidget* get_tabs_widget_ptr();


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
