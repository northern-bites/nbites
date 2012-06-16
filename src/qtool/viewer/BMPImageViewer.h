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

#include "data/Typedefs.h"
#include "image/FastYUVToBMPImage.h"

namespace qtool {
namespace viewer {

class BMPImageViewer: public QWidget {
    Q_OBJECT
public:

    BMPImageViewer(image::BMPImage* image,
				   QWidget *parent = NULL,
				   float scale = 1.0f);

    virtual ~BMPImageViewer();

    virtual QSize minimumSizeHint() const {
        return QSize(image->getWidth()*scale, image->getHeight()*scale);
    }

	virtual unsigned getWidth() { return image->getWidth()*scale;}
	virtual unsigned getHeight() { return image->getHeight()*scale;}

	QVBoxLayout* getLayout();

public slots:
    void updateView();

protected:
    void showEvent(QShowEvent* event);
    void paintEvent(QPaintEvent* event);
    void setupUI();
    void addPixmap();

protected:
    image::BMPImage* image;
    QLabel imagePlaceholder;
	QVBoxLayout* BMPlayout;
	QSize imageSize;
	int initialW;
	int initialH;
	float scale;

private:
	bool shouldRedraw;

};

}
}
