/**
 *
 *@class Viewer
 *
 * This class contains the locations of robots
 *
 * @author Brian Jacobel
 * @date Apr 2012
 *
 */

#pragma once

#include <vector>
#include <QtNetwork/QUdpSocket>
#include "data/DataManager.h"

namespace qtool {
	namespace viewer {

		class BotLocs: public QObject {
			Q_OBJECT

			public:
			BotLocs();
			~BotLocs(){}
			int getX(int);
			int getY(int);
			int getXUncert(int);
			int getYUncert(int);
			int getSize();
			void startListening();
			void stopListening();

		private:
			struct Bot{
				QHostAddress address;
				float xPos;
				float yPos;
				float xUncert;
				float yUncert;
			};
			std::vector<Bot> botPositions;
			std::vector<QHostAddress> onField;
			QUdpSocket udpSocket;

		public slots:
			void readSocket();

		};
	}
}
