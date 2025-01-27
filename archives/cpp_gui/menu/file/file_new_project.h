#pragma once

#include <QWidget>
#include <QFileInfo>

#include "ui_file_new_project.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from IodeSettingsDialog,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class MenuFileNewProject : public QDialog, public Ui::MenuFileNewProject
{
    Q_OBJECT

    WrapperFileChooser* wDirPath;

public:
    MenuFileNewProject(const QString& currentProjectPath, QWidget* parent = nullptr);
    ~MenuFileNewProject();

    QString getPathNewProject() const { return wDirPath->getQValue().toString(); }

public slots:
    void create();
};
