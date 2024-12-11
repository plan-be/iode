#include "workspace_low_to_high.h"


MenuWorkspaceLowToHigh::MenuWorkspaceLowToHigh(QWidget* parent) : 
    IodeSettingsDialog(parent)
{
    setupUi(this);

    QList<QString> q_methods;
    for(const auto& [_, method] : mLowToHigh) q_methods << QString::fromStdString(method);

    wInputFile = new WrapperFileChooser(label_input_file->text(), *fileChooser_input_file, REQUIRED_FIELD, 
                                        FILE_VARIABLES, EXISTING_FILE);
    wMethod = new WrapperComboBox(label_method->text(), *comboBox_method, REQUIRED_FIELD, q_methods);   
    wStockSeries = new WrapperQTextEdit(label_stock_series->text(), *textEdit_stock_series, OPTIONAL_FIELD);  
    wFlowSeries = new WrapperQTextEdit(label_flow_series->text(), *textEdit_flow_series, OPTIONAL_FIELD); 

    mapFields["InputFile"] = wInputFile;
    mapFields["Method"] = wMethod;
    mapFields["StockSeries"] = wStockSeries;
    mapFields["FlowSeries"] = wFlowSeries; 

    className = "MENU_WORKSPACE_LOW_TO_HIGH";
    loadSettings();
}

MenuWorkspaceLowToHigh::~MenuWorkspaceLowToHigh()
{
    delete wInputFile;
    delete wMethod;
    delete wStockSeries;
    delete wFlowSeries;
}

void MenuWorkspaceLowToHigh::slot_low_to_high()
{
    try
    {
        std::string filepath = wInputFile->extractAndVerify().toStdString();
        int i_method = wMethod->extractAndVerify();
        std::vector<char> v_keys;
        for(const auto& [key, _] : mLowToHigh) v_keys.push_back(key);
        char method = v_keys[i_method];
        std::string stock_series = wStockSeries->extractAndVerify().toStdString();
        std::string flow_series = wFlowSeries->extractAndVerify().toStdString();

        if(stock_series.empty() && flow_series.empty()) 
            throw std::invalid_argument(std::string("Cannot execute command 'low to high': ") + 
                                        std::string("both stock and flow series are empty!"));

        if (!stock_series.empty()) low_to_high(LTOH_STOCK, method, filepath, stock_series);
        if (!flow_series.empty()) low_to_high(LTOH_FLOW, method, filepath, flow_series);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }

    this->accept();
}

void MenuWorkspaceLowToHigh::help()
{
	
	QDesktopServices::openUrl(url_manual);
}
