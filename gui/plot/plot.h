#pragma once
#include "utils.h"
#include <math.h>

#include <QList>
#include <QString>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QMenuBar>
#include <QGridLayout>
#include <QtCharts>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileInfo>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
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
    QStringList variablesNames;
    KDBVariables* kdb_vars;
    bool deleteKDB;

    QString projectPath;

    QString from;
    QString to;
    int start_t;
    int end_t;
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
    QComboBox* comboXAxisType;
    QCheckBox* checkYScale;
    QComboBox* comboXTicks;
    QComboBox* comboYTicks;

    QChart* chart;
    QChartView* chartView;

public:
    QIodePlotDialog(KDBVariables* kdb_vars=nullptr, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodePlotDialog();

    void plot(const QList<QString>& variables_names, const QString& from, const QString& to, 
        const QString& title="", const EnumIodeGraphChart chartType=I_G_CHART_LINE, 
        const EnumIodeGraphAxisType axisType=I_G_LEVEL, const bool logScale=false, 
        const EnumIodeGraphAxisThicks xTicks=I_G_MAJOR_THICKS, 
        const EnumIodeGraphAxisThicks yTicks=I_G_MAJOR_THICKS, const double minY=L_NAN, const double maxY=L_NAN);

private:
    void setMenuBar(QMenuBar* menuBar);
    QAbstractSeries::SeriesType iodeChartTypeToQtSeriesType(const EnumIodeGraphChart chartType);
    void computeMinMaxIntervalYAxis(const QList<double>& values);
    QValueAxis* createXAxis();
    QBarCategoryAxis* createXBarAxis();
    QValueAxis* createYAxis();
    QLogValueAxis* createYLogAxis();

    void buildSeries(const QAbstractSeries::SeriesType seriesType, const EnumIodeGraphAxisType axisType);

protected:
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);

public slots:
    void saveAs();
    void copyToClipboard();
    void print();
	void updateChartType(int index);
    void updateXAxisType(int XAxisType);
    void enableLogScale(int state);
    void updateXTicks(int index);
    void updateYTicks(int index);
};
