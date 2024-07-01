
#include "workspace_trend_correction.h"


MenuWorkspaceTrendCorrection::MenuWorkspaceTrendCorrection(QWidget* parent) : 
    IodeSettingsDialog(parent)
{
    setupUi(this);

    wInputFile = new WrapperFileChooser(label_input_file->text(), *fileChooser_input_file, REQUIRED_FIELD, 
                                        FILE_VARIABLES, EXISTING_FILE); 
    wLambda = new WrapperDoubleSpinBox(label_lambda->text(), *doubleSpinBox_lambda, REQUIRED_FIELD);
    wSeries = new WrapperQTextEdit(label_series->text(), *textEdit_series, REQUIRED_FIELD);  
    wLog = new WrapperCheckBox(label_log->text(), *checkBox_log, REQUIRED_FIELD);

    mapFields["InputFile"] = wInputFile;
    mapFields["Lambda"] = wLambda;
    mapFields["Series"] = wSeries;
    mapFields["Log"] = wLog;

    className = "MENU_WORKSPACE_TREND_CORRECTION";
    loadSettings();
}

MenuWorkspaceTrendCorrection::~MenuWorkspaceTrendCorrection()
{
    delete wInputFile;
    delete wLambda;
    delete wSeries;
    delete wLog;
}

void MenuWorkspaceTrendCorrection::trend_correction()
{
    try
    {
        std::string filepath = wInputFile->extractAndVerify().toStdString();
        double lambda = wLambda->extractAndVerify();
        std::string series = wSeries->extractAndVerify().toStdString();
        bool log = wLog->extractAndVerify();

        Variables.trend_correction(filepath, lambda, series, log);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "ERROR", QString(e.what()));
        return;
    }

    this->accept();
}

void MenuWorkspaceTrendCorrection::help()
{
	QDesktopServices::openUrl(url_manual);
}
