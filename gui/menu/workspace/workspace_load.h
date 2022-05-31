#pragma once

#include <QWidget>
#include <QMap>
#include <QString>

#include "ui_workspace_load.h"
#include "../../utils.h"
#include "../../settings.h"
#include "../../wrapper_classes.h"


const static QMap<QString, int> mapImportFormat({
    {QString("Ascii"), 0},
    {QString("Rotated Ascii"), 1},
    {QString("DIF"), 2},
    {QString("DIF Belgostat"), 3},
    {QString("NIS"), 4},
    {QString("GEM (chronos)"), 5},
    {QString("PRN-AREMOS"), 6},
    {QString("TXT-BELGOSTAT"), 7},
    });


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class QIodeMenuWorkspaceLoad : public QIodeSettings, public Ui::QIodeMenuWorkspaceLoad
{
	Q_OBJECT

    bool exit;

private:


public:
	QIodeMenuWorkspaceLoad(const QString& settings_filepath, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~QIodeMenuWorkspaceLoad();

public slots:
    void load_comments();
    void load_equations();
    void load_identities();
    void load_lists();
    void load_scalars();
    void load_tables();
    void load_variables();
    void load();
};
