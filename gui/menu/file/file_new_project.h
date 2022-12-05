#pragma once

#include <QWidget>
#include <QFileInfo>

#include "ui_file_new_project.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class QIodeMenuFileNewProject : public QDialog, public Ui::QIodeMenuFileNewProject
{
    Q_OBJECT

    WrapperFileChooser* wDirPath;

public:
    QIodeMenuFileNewProject(const QString& currentProjectPath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeMenuFileNewProject();

    QString getPathNewProject() const { return wDirPath->getQValue().toString(); }

public slots:
    void create();
};
