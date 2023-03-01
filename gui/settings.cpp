#pragma once
#include "settings.h"


QIodeSettings::QIodeSettings(const QString& filepath, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), className("")
{
    project_settings = new QSettings(filepath, QSettings::IniFormat);
}

QIodeSettings::~QIodeSettings()
{
    delete project_settings;
}

void QIodeSettings::saveSettings()
{
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

void QIodeSettings::loadSettings()
{
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

void QIodeSettings::closeEvent(QCloseEvent* event)
{
    saveSettings();
    event->accept();
}

void QIodeSettings::help()
{
    
    QDesktopServices::openUrl(url_manual);
}

void QIodeSettings::accept()
{
    saveSettings();
    QDialog::accept();
}

void QIodeSettings::reject()
{
    saveSettings();
    QDialog::reject();
}
