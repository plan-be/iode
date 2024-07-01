#pragma once

#include <QSettings>

#include "utils.h"

const static QString FILE_SETTINGSNAME = "iode_gui_settings.ini";


/**
 * @brief alternative to avoid non-constant global
 * 
 */
class ProjectSettings
{
  private:
    inline static QSettings* project_settings{nullptr};

  public:
    static QSettings* changeProject(const QDir& projectDir, QObject *parent = nullptr)
    {
        if(project_settings) delete project_settings;

        QString filepath = projectDir.absoluteFilePath("iode_gui_settings.ini");
        project_settings = new QSettings(filepath, QSettings::IniFormat, parent);

        return project_settings;
    }

    static QSettings* getProjectSettings()
    {
      return project_settings;
    }
};


// Mixin class for handling settings in derived class. 
class IodeSettingsDialog : public QDialog
{
    Q_OBJECT

protected:
    QString className;
    QMap<QString, BaseWrapper*> mapFields;

public:
    IodeSettingsDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint)
        : QDialog(parent, f), className("") {}

protected:
    void saveSettings();
    void loadSettings();
    void closeEvent(QCloseEvent* event) override;

public slots:
    void help();
    void accept() override;
    void reject() override;
};
