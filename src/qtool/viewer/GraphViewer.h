/**
 * View and create graphs/plots through R and Qwt (faster).
 *
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @date   April 2012
 */
#include <QWidget>
#include <QtGui>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QVector>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_text.h>
#include <qwt/qwt_plot_curve.h>
//#include "R/RTool.h"
#include <cmath>
#include <iostream>

namespace qtool 
{
  namespace viewer
  {
    class GraphViewer : public QWidget
    {
      Q_OBJECT
    public:
      GraphViewer(QWidget *parent = NULL);
      virtual ~GraphViewer();

      //void initializeR(int argc/* = 3*/, 
      //char *argv[]/* = {"R", "--no-save", "--silent"}*/);
      
      void plotCurve(QVector<double> x, QVector<double> y, QString title = "curve");

    private slots:
      void updateGraph();

    protected:
      QGridLayout *layout;
      QVBoxLayout *graphOptions;
      QHBoxLayout *graphTitleOption;

      QPushButton *updateGraphButton;

      QLineEdit *graphTitleInput;
      QLineEdit *xAxisTitleInput;
      QLineEdit *yAxisTitleInput;

      QLabel *graphTitleLabel;
      QLabel *xAxisTitleLabel;
      QLabel *yAxisTitleLabel;
      
      QwtPlot *centralPlot;

      QVector<QwtPlotCurve *> curves;
      
      //RTool* R;

    };
  }
}
