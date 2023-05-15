
#include "workspace_trend_correction.h"


QIodeMenuWorkspaceTrendCorrection::QIodeMenuWorkspaceTrendCorrection(QWidget* parent, Qt::WindowFlags f) : 
    QIodeSettings(parent, f)
{
    setupUi(this);

    wInputFile = new WrapperFileChooser(label_input_file->text(), *fileChooser_input_file, REQUIRED_FIELD, 
                                        I_VARIABLES_FILE, EXISTING_FILE); 
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

QIodeMenuWorkspaceTrendCorrection::~QIodeMenuWorkspaceTrendCorrection()
{
    delete wInputFile;
    delete wLambda;
    delete wSeries;
    delete wLog;
}

void QIodeMenuWorkspaceTrendCorrection::trend_correction()
{
    try
    {
        std::string filepath = wInputFile->extractAndVerify().toStdString();
        double lambda = wLambda->extractAndVerify();
        std::string series = wSeries->extractAndVerify().toStdString();
        bool log = wLog->extractAndVerify();

        KDBVariables kdb;
        kdb.trend_correction(filepath, lambda, series, log);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }

    this->accept();
}

void QIodeMenuWorkspaceTrendCorrection::help()
{
	QDesktopServices::openUrl(url_manual);
}
