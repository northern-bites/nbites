/**
 * @class OfflineViewer
 *
 * Loads libman and feeds data from the qtool memory to man
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <QWidget>
#include "Subscriber.h"
#include "viewer/MemoryViewer.h"
#include "data/DataManager.h"
#include "man/corpus/offlineconnect/OfflineManController.h"
#include "man/memory/Memory.h"
#include "data/RobotMemoryManager.h"

namespace qtool {
namespace offline {

class OfflineViewer : public QWidget, public Subscriber {
    Q_OBJECT

public:
    OfflineViewer(data::DataManager::const_ptr dataManager,
            QWidget* parent = 0);
    virtual ~OfflineViewer() {}

private slots:
    void update();
    void loadColorTable();
    void startMan();
    void stopMan();

private:
    QVBoxLayout *mainLayout;
    man::corpus::OfflineManController offlineControl;
    data::RobotMemoryManager::const_ptr manMemoryManager;
    viewer::MemoryViewer* manMemoryViewer;

};

}
}
