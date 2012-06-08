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
#include "man/corpus/offlineconnect/OfflineManController.h"
#include "man/memory/Memory.h"
#include "man/OfflineManPreloader.h"
#include "data/RobotMemoryManager.h"

namespace qtool {
namespace offline {

class OfflineViewer : public QWidget, public Subscriber {
    Q_OBJECT

public:
    OfflineViewer(man::memory::Memory::const_ptr memory,
            QWidget* parent = 0);
    virtual ~OfflineViewer() {}

    void update();

private slots:
    void loadColorTable();
    void reloadMan();
    void loadMan();

private:
    QVBoxLayout *mainLayout;
    man::corpus::OfflineManController::ptr offlineControl;
    OfflineManPreloader manPreloader;
    data::RobotMemoryManager::const_ptr manMemoryManager;
    viewer::MemoryViewer* manMemoryViewer;
    bool loaded;

};

}
}
