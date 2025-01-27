#pragma once
#include "settings.h"


void IodeSettingsDialog::saveSettings()
{
    QSettings* project_settings = ProjectSettings::getProjectSettings();
    project_settings->beginGroup(className);
    QMap<QString, BaseWrapper*>::iterator i;
    for (i = mapFields.begin(); i != mapFields.end(); ++i)
    {
        QString name = i.key();
        BaseWrapper* field = i.value();
        QVariant value = field->getQValue();
        project_settings->setValue(name, value);
    }
    project_settings->endGroup();
}

void IodeSettingsDialog::loadSettings()
{
    QSettings* project_settings = ProjectSettings::getProjectSettings();
    project_settings->beginGroup(className);
    QMap<QString, BaseWrapper*>::iterator i;
    for (i = mapFields.begin(); i != mapFields.end(); ++i)
    {
        QString key = i.key();
        BaseWrapper* field = i.value();
        // use contains() to not override default value set in UI form file
        // (i.e. default value used when a user uses the field for the first time)
        if (project_settings->contains(key))
        {
            // Note: if key does not exist, a default value is infered (not from UI form file!)
            QVariant value = project_settings->value(key).toString();
            field->setQValue(value);
        }
    }
    project_settings->endGroup();
}

void IodeSettingsDialog::closeEvent(QCloseEvent* event)
{
    saveSettings();
    event->accept();
}

void IodeSettingsDialog::help()
{
    
    QDesktopServices::openUrl(url_manual);
}

void IodeSettingsDialog::accept()
{
    saveSettings();
    QDialog::accept();
}

void IodeSettingsDialog::reject()
{
    saveSettings();
    QDialog::reject();
}
