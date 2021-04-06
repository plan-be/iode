
#include "workspace_clear.h"


/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from MixingSettings MUST:
 * 1. instantiate the pointer *ui,
 * 2. call ui->setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from MixinSettings) with the wrapped field items,
 * 5. initialize the inherited className member (using typeid(this).name()) 
 * 6. call the loadSettings() method (inherited from MixinSettings).
 */
ClearWorkspaceDialog::ClearWorkspaceDialog(QSettings& settings, QWidget* parent, Qt::WindowFlags f) : MixinSettings(settings, parent, f)
{
    ui = new Ui::clear_workspace();
    ui->setupUi(this);

    // TODO: use Wrapper classes (see wrapper_classes.h) to wrap input field items
    /* Examples:
     * input_type = new WrapperComboBox(ui->label_input_type->text(), *ui->comboBox_input_type, Required, qmapIodeTypes);
     * input_file = new WrapperFileChooser(ui->label_input_file->text(), *ui->fileChooser_input_file, Required, AnyFile, ExistingFile);
     * sample_from = new WrapperSampleEdit(ui->label_sample->text() + QString("From"), *ui->sampleEdit_sample_from, Required);
     */

    // TODO: fill mapFields
    /* Examples:
     * mapFields["InputType"] = input_type;
     * mapFields["InputFile"] = input_file;
     * mapFields["SampleFrom"] = sample_from;
     */

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

ClearWorkspaceDialog::~ClearWorkspaceDialog()
{
    delete ui;
}

void ClearWorkspaceDialog::clear()
{
// TODO: implement this
}
