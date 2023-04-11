#include "compute_identities.h"


/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from QIodeSettings MUST:
 * 2. call setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from QIodeSettings) with the wrapped field items,
 * 5. initialize the inherited className member (using typeid(this).name())
 * 6. call the loadSettings() method (inherited from QIodeSettings).
 */


QIodeMenuComputeIdentities::QIodeMenuComputeIdentities(const QString& project_settings_filepath, QWidget* parent, Qt::WindowFlags f) : 
    QIodeSettings(project_settings_filepath, parent, f)
{
    setupUi(this);

	QList<QString> q_langs;
	for(const std::string& lang: vLangs) q_langs << QString::fromStdString(lang);

	qFrom = new WrapperSampleEdit(label_from->text(), *sampleEdit_sample_from, REQUIRED_FIELD);
	qTo = new WrapperSampleEdit(label_to->text(), *sampleEdit_sample_to, REQUIRED_FIELD);
    qIdentitiesList = new WrapperQTextEdit(label_identities_list->text(), *textEdit_identities_list, REQUIRED_FIELD);
    qVariablesFiles = new WrapperQTextEdit(label_variables_list->text(), *textEdit_variables_list, OPTIONAL_FIELD);
    qScalarsFiles = new WrapperQTextEdit(label_scalars_list->text(), *textEdit_scalars_list, OPTIONAL_FIELD);
    qTrace = new WrapperCheckBox(label_trace->text(), *checkBox_trace, REQUIRED_FIELD);
    qLanguage = new WrapperComboBox(label_language->text(), *comboBox_language, REQUIRED_FIELD, q_langs);

    mapFields["From"] = qFrom;
    mapFields["To"] = qTo;
    mapFields["Identities_List"] = qIdentitiesList;
    mapFields["Variables_Files"] = qVariablesFiles;
    mapFields["Scalars_Files"] = qScalarsFiles;
    mapFields["Trace"] = qTrace;
    mapFields["Language"] = qLanguage;

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();

    KDBVariables kdb_var;
    Sample sample = kdb_var.get_sample();
    if(sample.nb_periods() > 0)
    {
        if(qFrom->getQValue().toString().isEmpty())
        {
            QString start_period = QString::fromStdString(sample.start_period().to_string());
            qFrom->setQValue(QVariant(start_period));
        } 

        if(qTo->getQValue().toString().isEmpty())
        {
            QString end_period = QString::fromStdString(sample.end_period().to_string());
            qTo->setQValue(QVariant(end_period));
        }
    }
}

QIodeMenuComputeIdentities::~QIodeMenuComputeIdentities()
{
    delete qFrom; 
    delete qTo;
    delete qIdentitiesList;
    delete qVariablesFiles;
    delete qScalarsFiles;
    delete qTrace;
    delete qLanguage;
}

void QIodeMenuComputeIdentities::compute()
{
    try
    {
        KDBIdentities kdb;

        std::string from = qFrom->extractAndVerify().toStdString();
        std::string to = qTo->extractAndVerify().toStdString();
        Sample sample(from, to);
        std::string identities_list = qIdentitiesList->extractAndVerify().toStdString();
        std::string variables_files = qVariablesFiles->extractAndVerify().toStdString();
        std::string scalars_files = qScalarsFiles->extractAndVerify().toStdString();
        bool trace = qTrace->extractAndVerify();
        EnumLang language = (EnumLang) (qLanguage->extractAndVerify() + IT_ENGLISH);

        kdb.execute_identities(from, to, identities_list, variables_files, scalars_files, trace);
        
        this->accept();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
    }
}