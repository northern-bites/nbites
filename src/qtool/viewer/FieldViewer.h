/**
 * @class Viewer
 *
 * This class is going to be the WorldController from
 * the previous Tool.  It will show the field with
 * robots and balls in their positions which are
 * passed in.
 *
 * @author Dani McAvoy
 * @date October 2011
 *
 *
 * @modified Brian Jacobel
 * @date April 2012
 */

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QtGui>
#include <QPainter>
#include <vector>

#include "data/DataManager.h"
#include "image/PaintField.h"
#include "image/PaintLocalization.h"
#include "BMPImageViewer.h"
#include "image/PaintBots.h"


namespace qtool {
	namespace viewer {

		class FieldViewer : public QWidget{
			Q_OBJECT

			public:
			FieldViewer(data::DataManager::ptr dataManager, QWidget* parent = 0);

		public slots:
			void stopDrawing();
			void drawBots();

		private:
			data::DataManager::ptr dataManager;
			viewer::BMPImageViewer* fieldView;
			image::OverlayedImage* overlayView;
			image::PaintBots* bot_locs;
			image::PaintField* fieldImage;
			bool keepDrawing;

		protected:
			QPushButton* startButton;
			QPushButton* stopButton;
			QVBoxLayout* mainLayout;
			QHBoxLayout* buttonLayout;
			QHBoxLayout* field;
			QSpacerItem* spacer;
			float scaleFactor;
		};


	}
}
