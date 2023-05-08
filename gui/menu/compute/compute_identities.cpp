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
    completer = new QIodeCompleter(false, false, I_IDENTITIES, textEdit_identities_list);
    textEdit_identities_list->setCompleter(completer);

	QList<QString> q_langs;
	for(const std::string& lang: vLangs) q_langs << QString::fromStdString(lang);

	qFrom = new WrapperSampleEdit(label_from->text(), *sampleEdit_sample_from, REQUIRED_FIELD);
	qTo = new WrapperSampleEdit(label_to->text(), *sampleEdit_sample_to, REQUIRED_FIELD);
    qIdentitiesList = new WrapperQPlainTextEdit(label_identities_list->text(), *textEdit_identities_list, OPTIONAL_FIELD);
    qVariablesFile1 = new WrapperFileChooser(label_variables_list->text(), *fileChooser_var_file1, OPTIONAL_FIELD, I_VARIABLES_FILE, EXISTING_FILE);
    qVariablesFile2 = new WrapperFileChooser(label_variables_list->text(), *fileChooser_var_file2, OPTIONAL_FIELD, I_VARIABLES_FILE, EXISTING_FILE);
    qVariablesFile3 = new WrapperFileChooser(label_variables_list->text(), *fileChooser_var_file3, OPTIONAL_FIELD, I_VARIABLES_FILE, EXISTING_FILE);
    qScalarsFile1 = new WrapperFileChooser(label_scalars_list->text(), *fileChooser_scl_file1, OPTIONAL_FIELD, I_SCALARS_FILE, EXISTING_FILE);
    qScalarsFile2 = new WrapperFileChooser(label_scalars_list->text(), *fileChooser_scl_file2, OPTIONAL_FIELD, I_SCALARS_FILE, EXISTING_FILE);
    qScalarsFile3 = new WrapperFileChooser(label_scalars_list->text(), *fileChooser_scl_file3, OPTIONAL_FIELD, I_SCALARS_FILE, EXISTING_FILE);
    qTrace = new WrapperCheckBox(label_trace->text(), *checkBox_trace, REQUIRED_FIELD);
    qLanguage = new WrapperComboBox(label_language->text(), *comboBox_language, REQUIRED_FIELD, q_langs);

    mapFields["From"] = qFrom;
    mapFields["To"] = qTo;
    mapFields["Identities_List"] = qIdentitiesList;
    mapFields["Variables_File_1"] = qVariablesFile1;
    mapFields["Variables_File_2"] = qVariablesFile2;
    mapFields["Variables_File_3"] = qVariablesFile3;
    mapFields["Scalars_File_1"] = qScalarsFile1;
    mapFields["Scalars_File_2"] = qScalarsFile2;
    mapFields["Scalars_File_3"] = qScalarsFile3;
    mapFields["Trace"] = qTrace;
    mapFields["Language"] = qLanguage;

    className = "MENU_COMPUTE_IDENTITIES";
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
    delete qVariablesFile1;
    delete qVariablesFile2;
    delete qVariablesFile3;
    delete qScalarsFile1;
    delete qScalarsFile2;
    delete qScalarsFile3;
    delete qTrace;
    delete qLanguage;

    delete completer;
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

        QStringList variablesFiles;
        variablesFiles << qVariablesFile1->extractAndVerify();
        variablesFiles << qVariablesFile2->extractAndVerify();
        variablesFiles << qVariablesFile3->extractAndVerify();
        std::string variables_files = variablesFiles.join(";").toStdString();

        QStringList scalarsFiles;
        scalarsFiles << qScalarsFile1->extractAndVerify();
        scalarsFiles << qScalarsFile2->extractAndVerify();
        scalarsFiles << qScalarsFile3->extractAndVerify();
        std::string scalars_files = scalarsFiles.join(";").toStdString();

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