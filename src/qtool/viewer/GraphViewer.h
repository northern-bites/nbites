/**
 * View and create graphs/plots through (R and) Qwt.
 *
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @date   April 2012
 */
#include "data/DataManager.h"
#include <QWidget>
#include <QtGui>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QLineEdit>
#include <QVector>
#include <QButtonGroup>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_text.h>
#include <qwt/qwt_plot_curve.h>
//#include "R/RTool.h"
#include <cmath>
#include <iostream>
#include <string>

namespace qtool 
{
  namespace viewer
  {
    class GraphViewer : public QWidget
    {
      Q_OBJECT
    public:
      GraphViewer(qtool::data::DataManager::ptr dataManager, QWidget *parent = NULL);
      virtual ~GraphViewer();

      //void initializeR(int argc/* = 3*/, 
      //char *argv[]/* = {"R", "--no-save", "--silent"}*/);
      
      /**
       *
       * Styles:
       *  QwtPlotCurve::NoCurve
       *  QwtPlotCurve::Lines
       *  QwtPlotCurve::Sticks
       *  QwtPlotCurve::Steps
       *  QwtPlotCurve::Dots
       */ 
      void plotCurve(QVector<double> x, QVector<double> y, 
		     QString title = "curve", 
		     QwtPlotCurve::CurveStyle style = QwtPlotCurve::Lines);

    private slots:
      void updateGraph();

      void updateCurveStyle(int id);

      // For protocol buffer message handling.
      void updateLocalization();

    protected:
      QGridLayout *layout;
      QVBoxLayout *graphOptions;
      QHBoxLayout *graphTitleOption;
      QHBoxLayout *plotOptions;

      QButtonGroup *plotOptionsGroup;

      QPushButton *updateGraphButton;
      QRadioButton *setPlotLineButton;
      QRadioButton *setPlotDotsButton;

      QLineEdit *graphTitleInput;
      QLineEdit *xAxisTitleInput;
      QLineEdit *yAxisTitleInput;

      QLabel *graphTitleLabel;
      QLabel *xAxisTitleLabel;
      QLabel *yAxisTitleLabel;
      QLabel *plotOptionsLabel;
      
      QwtPlot *centralPlot;

      QVector<QwtPlotCurve *> curves;
      
      //RTool* R;

      // For protocol buffer graphing. 
      qtool::data::DataManager::ptr dataManager;

      man::memory::MLocalization::const_ptr localization;

    };
  }
}
