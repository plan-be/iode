#include "plot.h"


QIodePlotDialog::QIodePlotDialog(EnumIodeGraphChart chartType, const bool logScale, EnumIodeGraphAxisThicks xTicks, 
    EnumIodeGraphAxisThicks yTicks, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), chartType(chartType), 
    logScale(logScale), xTicks(xTicks), yTicks(yTicks), fixedMinY(NAN), fixedMaxY(NAN)
{
    // prepare chart
    chart = new QChart();
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->legend()->setAlignment(Qt::AlignRight);

    // prepare widget wrapping the chart
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRubberBand(QChartView::RectangleRubberBand);

    // layout
    layout = new QGridLayout();

    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    layout->addWidget(menuBar, 0, 0, 1, -1);
    
    comboChartType = new QComboBox();
    QStringList q_chart_types;
    for(const std::string& chart_type: vGraphsChartTypes) q_chart_types << QString::fromStdString(chart_type);
    comboChartType->addItems(q_chart_types);
    connect(comboChartType, &QComboBox::currentIndexChanged, this, &QIodePlotDialog::updateChartType);
    layout->addWidget(comboChartType, 1, 0);

    QSpacerItem* horizontalSpacer = new QSpacerItem(0, 20);
    layout->addItem(horizontalSpacer, 0, 1);

    checkYScale = new QCheckBox("Log Scale");
    connect(checkYScale, &QCheckBox::stateChanged, this, &QIodePlotDialog::enableLogScale);
    layout->addWidget(checkYScale, 1, 2, Qt::AlignCenter);

    comboXTicks = new QComboBox();
    QStringList q_x_axis_ticks;
    for(const std::string& axis_ticks: vGraphsAxisThicks) q_x_axis_ticks << "X " + QString::fromStdString(axis_ticks);
    comboXTicks->addItems(q_x_axis_ticks);
    connect(comboXTicks, &QComboBox::currentIndexChanged, this, &QIodePlotDialog::updateXTicks);
    layout->addWidget(comboXTicks, 1, 3, Qt::AlignCenter);

    comboYTicks = new QComboBox();
    QStringList q_y_axis_ticks;
    for(const std::string& axis_ticks: vGraphsAxisThicks) q_y_axis_ticks << "Y " + QString::fromStdString(axis_ticks);
    comboYTicks->addItems(q_y_axis_ticks);
    connect(comboYTicks, &QComboBox::currentIndexChanged, this, &QIodePlotDialog::updateYTicks);
    layout->addWidget(comboYTicks, 1, 4, Qt::AlignCenter);

    // span = -1 -> takes the whole reaming rows/columns
    layout->addWidget(chartView, 2, 0, 1, -1);

    this->resize(600, 400);
    this->setLayout(layout);
    this->setModal(false);
}

QIodePlotDialog::~QIodePlotDialog()
{
    delete comboChartType;
    delete checkYScale;
    delete comboXTicks;
    delete comboYTicks;

    delete chart;
    delete chartView;
}

void QIodePlotDialog::setMenuBar(QMenuBar* menuBar)
{
    QMenu* menuFile = menuBar->addMenu("File");

    QAction* saveAsAction = new QAction("Save As", this);
    saveAsAction->setVisible(true);
    saveAsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    connect(saveAsAction, &QAction::triggered, this, &QIodePlotDialog::saveAs);
    menuFile->addAction(saveAsAction);

    QAction* copyAction = new QAction("Copy To Clipboard", this);
    copyAction->setVisible(true);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, &QIodePlotDialog::copyToClipboard);
    menuFile->addAction(copyAction);

    QAction* printAction = new QAction("Print", this);
    printAction->setVisible(true);
    printAction->setShortcut(QKeySequence::Print);
    connect(printAction, &QAction::triggered, this, &QIodePlotDialog::print);
    menuFile->addAction(printAction);

    QAction* closeAction = new QAction("Close", this);
    closeAction->setVisible(true);
    closeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
    connect(closeAction, &QAction::triggered, this, &QIodePlotDialog::close);
    menuFile->addAction(closeAction);
}

// Note: see https://doc.qt.io/qt-6/qtcharts-zoomlinechart-example.html
void QIodePlotDialog::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) 
    {
    case Qt::Key_Plus:
        chart->zoomIn();
        break;
    case Qt::Key_Minus:
        chart->zoomOut();
        break;
    case Qt::Key_Escape:
        chart->zoomReset();
        break;
    default:
        QDialog::keyPressEvent(event);
        break;
    }
}

// Note: see https://stackoverflow.com/a/48626725 
void QIodePlotDialog::wheelEvent(QWheelEvent *event)
{
    // QChart::zoom() documentation: A factor over 1.0 zooms into the view 
    //                               and a factor between 0.0 and 1.0 zooms out of it.
    double factor = event->angleDelta().y() < 0 ? 0.5 : 2;
    chart->zoom(factor);
    event->accept();
    QDialog::wheelEvent(event);
}

// see https://doc.qt.io/qt-6/graphicsview.html#printing
// and https://doc.qt.io/qt-6/qpixmap.html#reading-and-writing-image-files for supported formats
void QIodePlotDialog::saveAs()
{
    QString filePath;
    try
    {
        QPixmap pixmap(chartView->size());
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        chartView->render(&painter);
        painter.end();
        QSettings user_settings(QSettings::UserScope, this);
        QString projectPath = user_settings.value("projectPath", QDir::homePath()).toString();
        filePath = QFileDialog::getSaveFileName(this, "Save Graph", projectPath, 
            "Images (*.png *.bpm *.jpg *.jpeg *.xpm *.xbm)");
        pixmap.save(filePath);
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "Save Graph As", "Could not save graph to file " + 
            filePath + "\n" + QString(e.what()) + "\n");
    }
}

void QIodePlotDialog::copyToClipboard()
{
    QClipboard* clipboard = QApplication::clipboard();
    QPixmap pixmap(chartView->size());
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    chartView->render(&painter);
    painter.end();
    clipboard->setPixmap(pixmap);
}

// see https://doc.qt.io/qt-6/graphicsview.html#printing 
// see https://doc.qt.io/qt-6/qtprintsupport-index.html for Qt Print Support
void QIodePlotDialog::print()
{
    try
    {
            QPrintPreviewDialog dialog(&printer);
            connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &QIodePlotDialog::renderChart);
            dialog.exec(); 
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "Print Graph", "Could not print graph\n" + QString(e.what()));
    }
}

void QIodePlotDialog::updateChartType(int index)
{
    if(index < 0 || index >= I_NB_CHART_TYPES)
        return;

    chartType = (EnumIodeGraphChart) index;
    buildSeries();
}

void QIodePlotDialog::enableLogScale(int state)
{
    QAbstractAxis* Yaxis = chart->axisY();
    if (Yaxis)
    {
        chart->removeAxis(Yaxis);

        if (state == Qt::Checked)
        {
            logScale = true;
            Yaxis = createYLogAxis();
        }
        else
        {
            logScale = false;
            Yaxis = createYAxis();
        }

        chart->setAxisY(Yaxis);
        foreach(QAbstractSeries* series, chart->series()) series->attachAxis(Yaxis);
    }
}

void QIodePlotDialog::updateXTicks(int index)
{
    if(index < 0 || index >= I_NB_AXIS_THICKS)
        return;

    xTicks = (EnumIodeGraphAxisThicks) index;
    QAbstractAxis* Xaxis = chart->axisX();

    switch (index)
    {
    case I_G_MAJOR_THICKS:
        Xaxis->setGridLineVisible(true);
        Xaxis->setMinorGridLineVisible(false);
        break;
    case I_G_NO_THICKS:
        Xaxis->setGridLineVisible(false);
        Xaxis->setMinorGridLineVisible(false);
        break;
    case I_G_MINOR_THICKS:
        Xaxis->setGridLineVisible(true);
        Xaxis->setMinorGridLineVisible(true);
        break;
    default:
        break;
    }
}

void QIodePlotDialog::updateYTicks(int index)
{
    if(index < 0 || index >= I_NB_AXIS_THICKS)
        return;

    yTicks = (EnumIodeGraphAxisThicks) index;
    QAbstractAxis* Yaxis = chart->axisY();

    switch (index)
    {
    case I_G_MAJOR_THICKS:
        Yaxis->setGridLineVisible(true);
        Yaxis->setMinorGridLineVisible(false);
        break;
    case I_G_NO_THICKS:
        Yaxis->setGridLineVisible(false);
        Yaxis->setMinorGridLineVisible(false);
        break;
    case I_G_MINOR_THICKS:
        Yaxis->setGridLineVisible(true);
        Yaxis->setMinorGridLineVisible(true);
        break;
    default:
        break;
    }
}

void QIodePlotDialog::setPeriods(const Sample& sample, const QString& from, const QString& to)
{
    if(from.isEmpty())
    {
        Period start_period = sample.start_period();
        this->from = QString::fromStdString(start_period.to_string());
        start_t = 0;
        minX = start_period.to_double();
    }
    else
    {
        Period start_period(from.toStdString());
        this->from = from;
        start_t = sample.get_period_position(start_period);
        minX = start_period.to_double();
    }

    if(to.isEmpty())
    {
        Period end_period = sample.end_period();
        this->to = QString::fromStdString(end_period.to_string());
        end_t = sample.get_period_position(end_period);
        maxX = end_period.to_double();
    }
    else
    {
        Period end_period(to.toStdString());
        this->to = to;
        end_t = sample.get_period_position(end_period);
        maxX = end_period.to_double();
    }
    
    if(start_t >= end_t)
    {
        std::string msg = "Cannot create a plot since the starting period " + from.toStdString() + " is ";
        msg += (start_t == end_t) ? " equal to " : "after ";
        msg += "the ending period " + to.toStdString();
        throw IodeExceptionFunction(msg);
    }

    nb_periods = end_t - start_t + 1;
    stepX = 1. / sample.start_period().nb_periods_per_year();
}

void QIodePlotDialog::plot()
{
    // prepare series
    buildSeries();

    // title
    QString title_ = (title.isEmpty()) ? defaultTitle() : title;
    chart->setTitle(title_);
}

QValueAxis* QIodePlotDialog::createXAxis()
{
    // prepare X axis
    QValueAxis* Xaxis = new QValueAxis();
    Xaxis->setTitleText("Time");
    Xaxis->setLabelFormat("%d");
    Xaxis->setMin(minX);
    Xaxis->setMax(maxX);
    // choose best X ticks
    Xaxis->applyNiceNumbers();
    Xaxis->setMinorTickCount(((double) nb_periods) / Xaxis->tickCount());
    return Xaxis;
}

QBarCategoryAxis* QIodePlotDialog::createXBarAxis()
{
    QBarCategoryAxis* Xaxis = new QBarCategoryAxis();
    Xaxis->setTitleText("Time");
    Period from_period(from.toStdString());
    Xaxis->append(from);
    QString s_period;
    for(int t = 1; t < nb_periods; t++)
    {
        s_period = QString::fromStdString(from_period.shift(t).to_string());
        Xaxis->append(s_period);
    }
    return Xaxis;
}

void QIodePlotDialog::computeMinMaxIntervalYAxis(const QList<double>& values)
{
    // minimum value on the Y axis
    if (L_ISAN(fixedMinY))
        minY = fixedMinY; 
    else
    {
        minY = *std::min_element(values.begin(), values.end());
        if (minY > 0.0) minY = 0.0;
    }

    // maximum value on the Y axis
    if (L_ISAN(fixedMaxY))
        maxY = fixedMaxY; 
    else
        maxY = *std::max_element(values.begin(), values.end());

    double range = maxY - minY;
    int order = (int) log10(range);
    if (!L_ISAN(fixedMaxY)) maxY = (((int) (maxY / pow(10, order))) + 1) * pow(10, order);
    if (!L_ISAN(fixedMinY) && minY < 0.0) minY = (((int) (minY / pow(10, order))) - 1) * pow(10, order);
    YTickInterval = pow(10, order);
}

QValueAxis* QIodePlotDialog::createYAxis()
{
    QValueAxis* YValueAxis = new QValueAxis();
    YValueAxis->setTitleText("Values");
    YValueAxis->setLabelFormat("%.2f");
    YValueAxis->setMin(minY);
    YValueAxis->setMax(maxY);
    YValueAxis->setTickInterval(YTickInterval);
    YValueAxis->setMinorTickCount(MINOR_YTICKS_COUNT);
    return YValueAxis;
}

QLogValueAxis* QIodePlotDialog::createYLogAxis()
{
    QLogValueAxis* YLogAxis = new QLogValueAxis();
    YLogAxis->setTitleText("Values");
    YLogAxis->setLabelFormat("%g");
    YLogAxis->setBase(10.0);
    YLogAxis->setMin(minY);
    YLogAxis->setMax(maxY);

    return YLogAxis;
}

void QIodePlotDialog::buildSeries()
{
    QVector<double> values;
    QList<QAbstractSeries*> list_series;
    double minValue;
    double maxValue;
    QString legend;
    QAbstractAxis* Xaxis;

    chart->removeAllSeries();
    foreach(QAbstractAxis* axis, chart->axes()) chart->removeAxis(axis);

    try
    {
        QMap<QString, QVector<double>>::const_iterator it = chart_series.constBegin();

        // prepare series
        double XValue;
        switch (chartType)
        {
        case EnumIodeGraphChart::I_G_CHART_LINE :
            while (it != chart_series.constEnd())
            {
                QLineSeries* series = new QLineSeries();
                legend = legend_series[it.key()];
                series->setName(legend);
                XValue = minX;
                foreach(const double value, it.value())
                {
                    if(L_ISAN(value))
                    {
                        series->append(XValue, value);
                        values << value;
                    }
                    XValue += stepX;
                }
                chart->addSeries(series);
                list_series << series;
                ++it;
            }
            Xaxis = createXAxis();
            break;
        case EnumIodeGraphChart::I_G_CHART_SCATTER :
            while (it != chart_series.constEnd())
            {
                QScatterSeries* series = new QScatterSeries();
                legend = legend_series[it.key()];
                series->setName(legend);
                XValue = minX;
                foreach(const double value, it.value())
                {
                    if(L_ISAN(value))
                    {
                        series->append(XValue, value);
                        values << value;
                    }
                    XValue += stepX;
                }
                chart->addSeries(series);
                list_series << series;
                ++it;
            }
            Xaxis = createXAxis();
            break;
        case EnumIodeGraphChart::I_G_CHART_BAR :
            QBarSeries* series = new QBarSeries();
            while (it != chart_series.constEnd())
            {
                legend = legend_series[it.key()];                
                QBarSet *set = new QBarSet(legend);
                foreach(const double value, it.value())
                { 
                    if(!L_ISAN(value)) 
                        values << 0.0;
                    else
                        values << value;
                    set->append(value);
                }
                series->append(set);
                ++it;
            }
            chart->addSeries(series);
            list_series << series;
            Xaxis = createXBarAxis();
            break;
        }
    }
    catch(const std::exception& e)
    {
        QMessageBox::critical(nullptr, "ERROR", QString::fromStdString(e.what()));
    }

    // X axis
    chart->setAxisX(Xaxis);
    foreach(QAbstractSeries* series, list_series) series->attachAxis(Xaxis);

    // Y axis
    if(values.size() > 0)
    {
        computeMinMaxIntervalYAxis(values);
        QValueAxis* Yaxis = createYAxis();
        chart->setAxisY(Yaxis);
        foreach(QAbstractSeries* series, list_series) series->attachAxis(Yaxis);
    }
    else
    {
        QMessageBox::warning(this, "WARNING", QString("Couldn't find any valid value for the Y axis ") +
            "for the sample from " + from + " to " + to);
    }
}
