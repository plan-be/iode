#include "plot.h"


PlotDialog::PlotDialog(TableGraphType chartType, const bool logScale, TableGraphGrid xTicks, 
    TableGraphGrid yTicks, QWidget* parent) : QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint), 
    chartType(chartType), logScale(logScale), xTicks(xTicks), yTicks(yTicks), fixedMinY(NAN), fixedMaxY(NAN)
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
    for(const std::string& chart_type: v_graph_chart_types) q_chart_types << QString::fromStdString(chart_type);
    comboChartType->addItems(q_chart_types);
    connect(comboChartType, &QComboBox::currentIndexChanged, this, &PlotDialog::updateChartType);
    layout->addWidget(comboChartType, 1, 0);

    QSpacerItem* horizontalSpacer = new QSpacerItem(0, 20);
    layout->addItem(horizontalSpacer, 0, 1);

    checkYScale = new QCheckBox("Log Scale");
    connect(checkYScale, &QCheckBox::stateChanged, this, &PlotDialog::enableLogScale);
    layout->addWidget(checkYScale, 1, 2, Qt::AlignCenter);

    comboXTicks = new QComboBox();
    QStringList q_x_axis_ticks;
    for(const std::string& axis_ticks: v_graph_axis_thicks) q_x_axis_ticks << "X " + QString::fromStdString(axis_ticks);
    comboXTicks->addItems(q_x_axis_ticks);
    connect(comboXTicks, &QComboBox::currentIndexChanged, this, &PlotDialog::updateXTicks);
    layout->addWidget(comboXTicks, 1, 3, Qt::AlignCenter);

    comboYTicks = new QComboBox();
    QStringList q_y_axis_ticks;
    for(const std::string& axis_ticks: v_graph_axis_thicks) q_y_axis_ticks << "Y " + QString::fromStdString(axis_ticks);
    comboYTicks->addItems(q_y_axis_ticks);
    connect(comboYTicks, &QComboBox::currentIndexChanged, this, &PlotDialog::updateYTicks);
    layout->addWidget(comboYTicks, 1, 4, Qt::AlignCenter);

    // span = -1 -> takes the whole reaming rows/columns
    layout->addWidget(chartView, 2, 0, 1, -1);

    this->resize(600, 400);
    this->setLayout(layout);
    this->setModal(false);
}

PlotDialog::~PlotDialog()
{
    delete comboChartType;
    delete checkYScale;
    delete comboXTicks;
    delete comboYTicks;

    delete chart;
    delete chartView;
}

void PlotDialog::setMenuBar(QMenuBar* menuBar)
{
    QMenu* menuFile = menuBar->addMenu("File");

    QAction* saveAsAction = new QAction("Save As", this);
    saveAsAction->setVisible(true);
    saveAsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    connect(saveAsAction, &QAction::triggered, this, &PlotDialog::saveAs);
    menuFile->addAction(saveAsAction);

    QAction* copyAction = new QAction("Copy To Clipboard", this);
    copyAction->setVisible(true);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, &PlotDialog::copyToClipboard);
    menuFile->addAction(copyAction);

    QAction* printAction = new QAction("Print", this);
    printAction->setVisible(true);
    printAction->setShortcut(QKeySequence::Print);
    connect(printAction, &QAction::triggered, this, &PlotDialog::print);
    menuFile->addAction(printAction);

    QAction* closeAction = new QAction("Close", this);
    closeAction->setVisible(true);
    closeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
    connect(closeAction, &QAction::triggered, this, &PlotDialog::close);
    menuFile->addAction(closeAction);
}

// Note: see https://doc.qt.io/qt-6/qtcharts-zoomlinechart-example.html
void PlotDialog::keyPressEvent(QKeyEvent *event)
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
void PlotDialog::wheelEvent(QWheelEvent *event)
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
void PlotDialog::saveAs()
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

void PlotDialog::copyToClipboard()
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
void PlotDialog::print()
{
    try
    {
            QPrintPreviewDialog dialog(&printer);
            connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &PlotDialog::renderChart);
            dialog.exec(); 
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "Print Graph", "Could not print graph\n" + QString(e.what()));
    }
}

void PlotDialog::updateChartType(int index)
{
    if(index < 0 || index >= IODE_NB_CHART_TYPES)
        return;

    chartType = (TableGraphType) index;
    buildSeries();
}

void PlotDialog::enableLogScale(int state)
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

void PlotDialog::updateXTicks(int index)
{
    if(index < 0 || index >= IODE_NB_AXIS_THICKS)
        return;

    xTicks = (TableGraphGrid) index;
    QAbstractAxis* Xaxis = chart->axisX();

    switch (index)
    {
    case TABLE_GRAPH_MAJOR:
        Xaxis->setGridLineVisible(true);
        Xaxis->setMinorGridLineVisible(false);
        break;
    case TABLE_GRAPH_NONE:
        Xaxis->setGridLineVisible(false);
        Xaxis->setMinorGridLineVisible(false);
        break;
    case TABLE_GRAPH_MINOR:
        Xaxis->setGridLineVisible(true);
        Xaxis->setMinorGridLineVisible(true);
        break;
    default:
        break;
    }
}

void PlotDialog::updateYTicks(int index)
{
    if(index < 0 || index >= IODE_NB_AXIS_THICKS)
        return;

    yTicks = (TableGraphGrid) index;
    QAbstractAxis* Yaxis = chart->axisY();

    switch (index)
    {
    case TABLE_GRAPH_MAJOR:
        Yaxis->setGridLineVisible(true);
        Yaxis->setMinorGridLineVisible(false);
        break;
    case TABLE_GRAPH_NONE:
        Yaxis->setGridLineVisible(false);
        Yaxis->setMinorGridLineVisible(false);
        break;
    case TABLE_GRAPH_MINOR:
        Yaxis->setGridLineVisible(true);
        Yaxis->setMinorGridLineVisible(true);
        break;
    default:
        break;
    }
}

void PlotDialog::setPeriods(const Sample& sample, const QString& from, const QString& to)
{
    if(from.isEmpty())
    {
        Period start_period = sample.start_period();
        this->from = QString::fromStdString(start_period.to_string());
        start_t = 0;
        minX = start_period.to_float();
    }
    else
    {
        Period start_period(from.toStdString());
        this->from = from;
        start_t = sample.get_period_position(start_period);
        minX = start_period.to_float();
    }

    if(to.isEmpty())
    {
        Period end_period = sample.end_period();
        this->to = QString::fromStdString(end_period.to_string());
        end_t = sample.get_period_position(end_period);
        maxX = end_period.to_float();
    }
    else
    {
        Period end_period(to.toStdString());
        this->to = to;
        end_t = sample.get_period_position(end_period);
        maxX = end_period.to_float();
    }
    
    if(start_t >= end_t)
    {
        std::string msg = "Cannot create a plot since the starting period " + from.toStdString() + " is ";
        msg += (start_t == end_t) ? " equal to " : "after ";
        msg += "the ending period " + to.toStdString();
        throw std::runtime_error(msg);
    }

    nb_periods = end_t - start_t + 1;
    stepX = 1. / sample.start_period().nb_periods_per_year();
}

void PlotDialog::plot()
{
    // prepare series
    buildSeries();

    // title
    QString title_ = (title.isEmpty()) ? defaultTitle() : title;
    chart->setTitle(title_);
}

QValueAxis* PlotDialog::createXAxis()
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

QBarCategoryAxis* PlotDialog::createXBarAxis()
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

void PlotDialog::computeMinMaxIntervalYAxis(const QList<double>& values)
{
    // minimum value on the Y axis
    if (IODE_IS_A_NUMBER(fixedMinY))
        minY = fixedMinY; 
    else
    {
        minY = *std::min_element(values.begin(), values.end());
        if (minY > 0.0) minY = 0.0;
    }

    // maximum value on the Y axis
    if (IODE_IS_A_NUMBER(fixedMaxY))
        maxY = fixedMaxY; 
    else
        maxY = *std::max_element(values.begin(), values.end());

    double range = maxY - minY;
    int order = (int) log10(range);
    if (!IODE_IS_A_NUMBER(fixedMaxY)) maxY = (((int) (maxY / pow(10, order))) + 1) * pow(10, order);
    if (!IODE_IS_A_NUMBER(fixedMinY) && minY < 0.0) minY = (((int) (minY / pow(10, order))) - 1) * pow(10, order);
    YTickInterval = pow(10, order);
}

QValueAxis* PlotDialog::createYAxis()
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

QLogValueAxis* PlotDialog::createYLogAxis()
{
    QLogValueAxis* YLogAxis = new QLogValueAxis();
    YLogAxis->setTitleText("Values");
    YLogAxis->setLabelFormat("%g");
    YLogAxis->setBase(10.0);
    YLogAxis->setMin(minY);
    YLogAxis->setMax(maxY);

    return YLogAxis;
}

void PlotDialog::buildSeries()
{
    QVector<double> values;
    QList<QAbstractSeries*> list_series;
    double minValue;
    double maxValue;
    QAbstractAxis* Xaxis;

    chart->removeAllSeries();
    foreach(QAbstractAxis* axis, chart->axes()) chart->removeAxis(axis);

    try
    {
        // prepare series
        double XValue;
        switch (chartType)
        {
        case TableGraphType::TABLE_GRAPH_LINE :
            for(PlotSeries& series: chart_series)
            {
                QLineSeries* line_series = new QLineSeries();
                line_series->setName(series.name);
                XValue = minX;
                foreach(const double value, series.y)
                {
                    if(IODE_IS_A_NUMBER(value))
                    {
                        line_series->append(XValue, value);
                        values << value;
                    }
                    XValue += stepX;
                }
                QPen pen(series.color);
                pen.setStyle(series.lineStyle);
                line_series->setPen(pen);

                chart->addSeries(line_series);
                list_series << line_series;
            }
            Xaxis = createXAxis();
            break;
        case TableGraphType::TABLE_GRAPH_SCATTER :
            for(PlotSeries& series: chart_series)
            {
                QScatterSeries* scatter_series = new QScatterSeries();
                scatter_series->setName(series.name);
                XValue = minX;
                foreach(const double value, series.y)
                {
                    if(IODE_IS_A_NUMBER(value))
                    {
                        scatter_series->append(XValue, value);
                        values << value;
                    }
                    XValue += stepX;
                }
                scatter_series->setColor(series.color);

                chart->addSeries(scatter_series);
                list_series << scatter_series;
            }
            Xaxis = createXAxis();
            break;
        case TableGraphType::TABLE_GRAPH_BAR :
            QBarSeries* bar_series = new QBarSeries();
            for(PlotSeries& series: chart_series)
            {
                QBarSet *set = new QBarSet(series.name);
                foreach(const double value, series.y)
                { 
                    if(!IODE_IS_A_NUMBER(value)) 
                        values << 0.0;
                    else
                        values << value;
                    set->append(value);
                }
                set->setColor(series.color);
                bar_series->append(set);
            }
            chart->addSeries(bar_series);
            list_series << bar_series;
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
