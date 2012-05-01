#include "GraphViewer.h"

namespace qtool
{
  namespace viewer
  {
    GraphViewer::GraphViewer(qtool::data::DataManager::ptr dataManager, QWidget *parent)
      : QWidget(parent), layout(new QGridLayout(this)), graphOptions(new QVBoxLayout()),
	graphTitleOption(new QHBoxLayout()), plotOptions(new QHBoxLayout()),
	dataManager(dataManager)
    {
      // Setup signals for protocol buffer messages.
      // dataManager->connectSlotToMObject(this, 
      // 					SLOT( updateLocalization() ), 
      // 					man::memory::MLOCALIZATION_ID);

      updateGraphButton = new QPushButton(tr("Update Graph"), this);
      graphTitleInput   = new QLineEdit(tr("Default Plot"), this);
      graphTitleLabel   = new QLabel(tr("Graph title: "), this);
      xAxisTitleLabel   = new QLabel(tr("Horizontal axis label: "), this);
      xAxisTitleInput   = new QLineEdit(this);
      yAxisTitleLabel   = new QLabel(tr("Vertical axis label: "), this);
      yAxisTitleInput   = new QLineEdit(this);

      plotOptionsLabel  = new QLabel(tr("Plot curve options: "), this);
      setPlotLineButton = new QRadioButton(tr("Line"), this);
      setPlotDotsButton = new QRadioButton(tr("Points"), this);

      plotOptionsGroup  = new QButtonGroup(this);
      plotOptionsGroup->addButton(setPlotLineButton);
      plotOptionsGroup->addButton(setPlotDotsButton);

      graphTitleOption->addWidget(graphTitleLabel);
      graphTitleOption->addWidget(graphTitleInput);
      graphTitleOption->addWidget(xAxisTitleLabel);
      graphTitleOption->addWidget(xAxisTitleInput);
      graphTitleOption->addWidget(yAxisTitleLabel);
      graphTitleOption->addWidget(yAxisTitleInput);

      plotOptions->addWidget(plotOptionsLabel);
      plotOptions->addWidget(setPlotLineButton);
      plotOptions->addWidget(setPlotDotsButton);

      graphOptions->addLayout(graphTitleOption);
      graphOptions->addLayout(plotOptions);
      graphOptions->addWidget(updateGraphButton);

      connect(updateGraphButton, SIGNAL( clicked() ), this, SLOT( updateGraph() ));

      connect(plotOptionsGroup, SIGNAL( buttonPressed(int) ), this, SLOT( updateCurveStyle(int) ));

      centralPlot = new QwtPlot(QwtText("Default Plot"));

      // Set some default curve data. 
      QVector<double> xData;
      QVector<double> yData;
      for(int i = 0; i < 100; ++i)
      {
	xData.push_back((double)i);
	yData.push_back(std::sqrt((double)i));
      }
      plotCurve(xData, yData, "f(x) = sqrt(x)", QwtPlotCurve::Lines);

      layout->addLayout(graphOptions, 1, 0, Qt::AlignTop);
      layout->addWidget(centralPlot, 0, 0, 1, 1);

      this->setLayout(layout);

      localization = dataManager->getMemory()->getMLocalization();

   }

    GraphViewer::~GraphViewer()
    {
      //delete R;
      //R = 0;
      delete centralPlot;
      centralPlot = 0;
      delete graphOptions;
      graphOptions = 0;
      delete graphTitleOption;
      graphTitleOption = 0;
      delete plotOptions;
      plotOptions = 0;
    }

    //void GraphViewer::initializeR(int argc, char *argv[])
    //{
    //if(R == NULL)
    //{
    //	R = new RTool(argc, argv);
    //}
    //}

    void GraphViewer::plotCurve(QVector<double> x, QVector<double> y, QString title,
				QwtPlotCurve::CurveStyle style)
    {
      QwtPlotCurve *curve = new QwtPlotCurve(title);

      curve->setStyle(style);

      curves.push_back(curve);

      curve->attach(centralPlot);

      curve->setSamples(x, y);

      centralPlot->replot();
    }

    void GraphViewer::updateGraph()
    {
      //std::cout << "Updating graph..." << std::endl;

      centralPlot->setTitle(graphTitleInput->text());
      centralPlot->setAxisTitle(QwtPlot::xBottom, xAxisTitleInput->text());
      centralPlot->setAxisTitle(QwtPlot::yLeft, yAxisTitleInput->text());
    }

    void GraphViewer::updateCurveStyle(int id)
    {
      //std::cout << "Updating curve style..." << std::endl;
      if(id == plotOptionsGroup->id(setPlotLineButton))
      {
	curves[0]->setStyle(QwtPlotCurve::Lines);
	centralPlot->replot();
      }
      else if(id == plotOptionsGroup->id(setPlotDotsButton))
      {
	curves[0]->setStyle(QwtPlotCurve::Dots);
	centralPlot->replot();
      }
      else
      {
      	std::cout << "GraphViewer::updateCurveStyle() bad id!" << std::endl;
      }
    }

    void GraphViewer::updateLocalization()
    {

    }
  }
}
