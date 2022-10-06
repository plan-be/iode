#include "workspace_high_to_low.h"


QIodeMenuWorkspaceHighToLow::QIodeMenuWorkspaceHighToLow(const QString& settings_filepath, QWidget* parent, Qt::WindowFlags f) : 
    QIodeSettings(settings_filepath, parent, f)
{
    setupUi(this);

    wInputFile = new WrapperFileChooser(label_input_file->text(), *fileChooser_input_file, REQUIRED_FIELD, 
                                        I_VARIABLES_FILE, EXISTING_FILE);
    wLastObs = new WrapperQTextEdit(label_last_obsvervation->text(), *textEdit_last_observation, OPTIONAL_FIELD);  
    wMean = new WrapperQTextEdit(label_mean->text(), *textEdit_mean, OPTIONAL_FIELD);  
    wSum = new WrapperQTextEdit(label_sum->text(), *textEdit_sum, OPTIONAL_FIELD);  

    mapFields["InputFile"] = wInputFile;
    mapFields["LastObs"] = wLastObs;
    mapFields["Mean"] = wMean;
    mapFields["Sum"] = wSum;

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

QIodeMenuWorkspaceHighToLow::~QIodeMenuWorkspaceHighToLow()
{
    delete wInputFile;
    delete wLastObs;
    delete wMean;
    delete wSum;
}

void QIodeMenuWorkspaceHighToLow::slot_high_to_low()
{
    try
    {
        std::string filepath = wInputFile->extractAndVerify().toStdString();
        std::string last_obs_series = wLastObs->extractAndVerify().toStdString();
        std::string mean_series = wMean->extractAndVerify().toStdString();
        std::string sum_series = wSum->extractAndVerify().toStdString();

        if(last_obs_series.empty() && mean_series.empty() && sum_series.empty()) 
            throw IodeExceptionFunction("Cannot excute command 'high to low'", 
                "all series last observation, mean and sum are empty!");

        if (!last_obs_series.empty()) high_to_low(HTOL_LAST, filepath, last_obs_series);
        if (!mean_series.empty()) high_to_low(HTOL_MEAN, filepath, mean_series);
        if (!sum_series.empty()) high_to_low(HTOL_SUM, filepath, sum_series);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }

    this->accept();
}

void QIodeMenuWorkspaceHighToLow::help()
{
	QUrl url = get_url_iode_manual();
	QDesktopServices::openUrl(url);
}
