
#include "workspace_seasonal_adjustment.h"


MenuWorkspaceSeasonalAdjustment::MenuWorkspaceSeasonalAdjustment(QWidget* parent) : 
    IodeSettingsDialog(parent)
{
    setupUi(this);

    wInputFile = new WrapperFileChooser(label_input_file->text(), *fileChooser_input_file, REQUIRED_FIELD, 
                                        FILE_VARIABLES, EXISTING_FILE); 
    wSeries = new WrapperQTextEdit(label_series->text(), *textEdit_series, OPTIONAL_FIELD);  
    wEPSTest = new WrapperDoubleSpinBox(label_EPS_test->text(), *doubleSpinBox_EPS_test, OPTIONAL_FIELD); 

    mapFields["InputFile"] = wInputFile;
    mapFields["Series"] = wSeries;
    mapFields["EPSTest"] = wEPSTest;

    className = "MENU_WORKSPACE_SEASONAL_ADJUSTMENT";
    loadSettings();
}

MenuWorkspaceSeasonalAdjustment::~MenuWorkspaceSeasonalAdjustment()
{
    delete wInputFile;
    delete wSeries;
    delete wEPSTest;
}

void MenuWorkspaceSeasonalAdjustment::seasonal_adjustment()
{
    try
    {
        std::string filepath = wInputFile->extractAndVerify().toStdString();
        std::string series = wSeries->extractAndVerify().toStdString();
        double EPS_test = wEPSTest->extractAndVerify();

        Variables.seasonal_adjustment(filepath, series, EPS_test);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }

    this->accept();
}

void MenuWorkspaceSeasonalAdjustment::help()
{
	QDesktopServices::openUrl(url_manual);
}
