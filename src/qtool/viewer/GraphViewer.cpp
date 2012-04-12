#include "GraphViewer.h"

namespace qtool
{
  namespace viewer
  {
    GraphViewer::GraphViewer(QWidget *parent)
      : QWidget(parent), layout(new QGridLayout(this)), graphOptions(new QVBoxLayout(this)),
	graphTitleOption(new QHBoxLayout(this))
    {
      updateGraphButton = new QPushButton(tr("Update Graph"), this);
      graphTitleInput   = new QLineEdit(tr("Default Plot"), this);
      graphTitleLabel   = new QLabel(tr("Graph title: "));
      xAxisTitleLabel   = new QLabel(tr("Horizontal axis label: "));
      xAxisTitleInput   = new QLineEdit(this);
      yAxisTitleLabel   = new QLabel(tr("Vertical axis label: "));
      yAxisTitleInput   = new QLineEdit(this);

      graphTitleOption->addWidget(graphTitleLabel);
      graphTitleOption->addWidget(graphTitleInput);
      graphTitleOption->addWidget(xAxisTitleLabel);
      graphTitleOption->addWidget(xAxisTitleInput);
      graphTitleOption->addWidget(yAxisTitleLabel);
      graphTitleOption->addWidget(yAxisTitleInput);

      graphOptions->addLayout(graphTitleOption);
      graphOptions->addWidget(updateGraphButton);

      connect(updateGraphButton, SIGNAL( clicked() ), SLOT( updateGraph() ));

      centralPlot = new QwtPlot(QwtText("Default Plot"));

      // Set some default curve data. 
      QVector<double> xData;
      QVector<double> yData;
      for(int i = 0; i < 100; ++i)
      {
	xData.push_back((double)i);
	yData.push_back(std::sqrt((double)i));
      }
      plotCurve(xData, yData, "f(x) = sqrt(x)");

      layout->addLayout(graphOptions, 1, 0, Qt::AlignTop);
      layout->addWidget(centralPlot, 0, 0, 1, 1);

      this->setLayout(layout);

    }

    GraphViewer::~GraphViewer()
    {
      //delete R;
      //R = 0;
      delete centralPlot;
      centralPlot = 0;
      // Clean up curves in memory.
      for(int i = 0; i < curves.size(); ++i)
      {
	if(curves[i])
	{
	  std::cout << "Deleting curve " << i << "." << std::endl;
	  delete curves[i];
	  curves[i] = 0;
	}
      }
    }

    //void GraphViewer::initializeR(int argc, char *argv[])
    //{
    //if(R == NULL)
    //{
    //	R = new RTool(argc, argv);
    //}
    //}

    void GraphViewer::plotCurve(QVector<double> x, QVector<double> y, QString title)
    {
      QwtPlotCurve *curve = new QwtPlotCurve(title);

      curves.push_back(curve);

      curve->attach(centralPlot);

      curve->setSamples(x, y);

      centralPlot->replot();
    }

    void GraphViewer::updateGraph()
    {
      // @todo
      std::cout << "Updating graph..." << std::endl;

      centralPlot->setTitle(graphTitleInput->text());
      centralPlot->setAxisTitle(QwtPlot::xBottom, xAxisTitleInput->text());
      centralPlot->setAxisTitle(QwtPlot::yLeft, yAxisTitleInput->text());
    }
  }
}
