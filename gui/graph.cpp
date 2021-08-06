#include "graph.h"

IODEGraphDialog::IODEGraphDialog(QSettings& settings, QWidget* parent, Qt::WindowFlags f) : MixinSettings(settings, parent, f)
{
	ui = new Ui::IODEGraph();
	ui->setupUi(this);

	plot();
}

IODEGraphDialog::~IODEGraphDialog()
{
	delete ui;
}

void IODEGraphDialog::plot()
{
	QCustomPlot* cplot = ui->graph_widget;
	// generate some data:
	QVector<double> x(101), y(101); // initialize with entries 0..100
	for (int i = 0; i < 101; ++i)
	{
		x[i] = i / 50.0 - 1; // x goes from -1 to 1
		y[i] = x[i] * x[i]; // let's plot a quadratic function
	}
	// create graph and assign data to it:
	cplot->addGraph();
	cplot->graph(0)->setData(x, y);
	// give the axes some labels:
	cplot->xAxis->setLabel("x");
	cplot->yAxis->setLabel("y");
	// set axes ranges, so we see all data:
	cplot->xAxis->setRange(-1, 1);
	cplot->yAxis->setRange(0, 1);
	cplot->replot();
}
