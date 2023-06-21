#pragma once
#include "utils.h"
#include <math.h>

#include <QMap>
#include <QVector>
#include <QString>
#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QMenuBar>
#include <QtCharts>
#include <QPrinter>
#include <QFileInfo>
#include <QGridLayout>
#include <QtCharts/QChart>
#include <QPrintPreviewDialog>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLogValueAxis>
#include <QtCharts/QBarCategoryAxis>


constexpr int MAX_NB_XTICKS = 8;
constexpr int MINOR_YTICKS_COUNT = 2;

/**
 * @brief Shortcuts:
 *        - minus -:  zoom out
 *        - plus +:   zoom in
 *        - Esc:      reset zoom
 *        - CTRL + S: save as (= export)
 *        - CTRL + C: copy to clipboard
 *        - CTRL + P: print
 *        - CTRL + Q: close window
 * 
 * It is possible to zoom in and zoom out using the wheel on the mouse.
 */
class QIodePlotDialog: public QDialog
{
    Q_OBJECT

    QString projectPath;

    QString title;
    EnumIodeGraphChart chartType;
    bool logScale;
    EnumIodeGraphAxisThicks xTicks;
    EnumIodeGraphAxisThicks yTicks;

    QString from;
    QString to;
    int nb_periods;

    double minX;
    double maxX;
    double stepX;
    double fixedMinY;
    double fixedMaxY;
    double minY;
    double maxY;
    double YTickInterval;

    QComboBox* comboChartType;
    QCheckBox* checkYScale;
    QComboBox* comboXTicks;
    QComboBox* comboYTicks;

    QChart* chart;
    QChartView* chartView;

    QPrinter printer;

protected:
    QGridLayout* layout;

    QMap<QString, QVector<double>> chart_series;
    QMap<QString, QString> legend_series;
    
    int start_t;
    int end_t;

public:
    QIodePlotDialog(EnumIodeGraphChart chartType = I_G_CHART_LINE, const bool logScale = false, 
        EnumIodeGraphAxisThicks xTicks = I_G_MAJOR_THICKS, EnumIodeGraphAxisThicks yTicks = I_G_MAJOR_THICKS, 
        QWidget* parent = nullptr);
    ~QIodePlotDialog();

    void plot();

    void setTitle(const QString& title)
    {
        this->title = title;
    }

    void setMinValue(const double min)
    {
        fixedMinY = min;
    }

    void setMaxValue(const double max)
    {
        fixedMaxY = max;
    }

    void setPeriods(const Sample& sample, const QString& from = "", const QString& to = "");

private:
    void setMenuBar(QMenuBar* menuBar);
    void computeMinMaxIntervalYAxis(const QList<double>& values);
    QValueAxis* createXAxis();
    QBarCategoryAxis* createXBarAxis();
    QValueAxis* createYAxis();
    QLogValueAxis* createYLogAxis();

protected:
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);

    void buildSeries();

    virtual void buildValues() {}

    virtual QString defaultTitle() = 0;

private slots:
    void renderChart()
    {
        QPainter painter(&printer);
        painter.setRenderHint(QPainter::Antialiasing);
        chartView->render(&painter);
    }

public slots:
    void saveAs();
    void copyToClipboard();
    void print();
	void updateChartType(int index);
    void enableLogScale(int state);
    void updateXTicks(int index);
    void updateYTicks(int index);
};
