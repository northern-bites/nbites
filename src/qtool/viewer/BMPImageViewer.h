/**
 * Renders a BMPImage by transforming it to a QPixmap and attaching
 * it to a QLabel
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QResizeEvent>

#include "data/Typedefs.h"
#include "image/FastYUVToBMPImage.h"

namespace qtool {
namespace viewer {

class BMPImageViewer: public QWidget {
    Q_OBJECT
public:

    BMPImageViewer(image::BMPImage* image,
				   QWidget *parent = NULL);

    virtual ~BMPImageViewer();

	virtual unsigned getWidth() { return scaledPixmap.width();}
	virtual unsigned getHeight() { return scaledPixmap.height();}
	QSize size(){return QSize(scaledPixmap.width(), scaledPixmap.height());}
	void resize(QSize newSize){scaledPixmap = scaledPixmap.scaled(newSize);}
	void resize(float sF){
		scaledPixmap = scaledPixmap.scaled(QSize(sF*scaledPixmap.width(),
												 sF*scaledPixmap.height()));
	}
	void setupUI();
	float getScale(){return scale;}
	void setScale(int newScale){scale = newScale;}


	QVBoxLayout* getLayout();

public slots:
    void updateView(bool resize = 1);

protected:
    void showEvent(QShowEvent* event);
    void paintEvent(QPaintEvent* event);

    void addPixmap();

protected:
    image::BMPImage* image;
    QLabel imagePlaceholder;
	QVBoxLayout* BMPlayout;
	QSize imageSize;
	QPixmap scaledPixmap;
	int initialW;
	int initialH;
	float scale;

private:
	bool shouldRedraw;

};



}
}
