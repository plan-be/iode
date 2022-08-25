#pragma once
#include "settings.h"


QIodeSettings::QIodeSettings(const QString& filepath, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), className("")
{
    settings = new QSettings(filepath, QSettings::IniFormat);
}

QIodeSettings::~QIodeSettings()
{
    delete settings;
}

void QIodeSettings::saveSettings()
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

void QIodeSettings::loadSettings()
{
    settings->beginGroup(className);
    QMap<QString, BaseWrapper*>::iterator i;
    for (i = mapFields.begin(); i != mapFields.end(); ++i)
    {
        QString key = i.key();
        BaseWrapper* field = i.value();
        // use contains() to not override default value set in UI form file
        // (i.e. default value used when a user uses the field for the first time)
        if (settings->contains(key))
        {
            // Note: if key does not exist, a default value is infered (not from UI form file!)
            QVariant value = settings->value(key).toString();
            field->setQValue(value);
        }
    }
    settings->endGroup();
}

void QIodeSettings::closeEvent(QCloseEvent* event)
{
    saveSettings();
    event->accept();
}

void QIodeSettings::help()
{
    QUrl url = get_url_iode_manual();
    QDesktopServices::openUrl(url);
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
