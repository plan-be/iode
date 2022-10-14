#pragma once
#include "utils.h"


// Mixin class for handling settings in derived class. 
class QIodeSettings : public QDialog
{
    Q_OBJECT

protected:
    QSettings* project_settings;
    QString className;
    QMap<QString, BaseWrapper*> mapFields;

public:
    QIodeSettings(const QString& filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeSettings();

protected:
    void saveSettings();
    void loadSettings();
    void closeEvent(QCloseEvent* event) override;

public slots:
    void help();
    void accept() override;
    void reject() override;
};
