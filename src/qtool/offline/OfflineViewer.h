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

namespace qtool {
namespace offline {

class OfflineViewer : public QWidget, public Subscriber<data::MObject_ID> {
    Q_OBJECT

public:
    OfflineViewer(man::memory::Memory::const_ptr memory,
            QWidget* parent = 0);
    virtual ~OfflineViewer() {}

    void update(data::MObject_ID id);

private slots:
    void loadColorTable();
    void reloadMan();

private:
    man::corpus::OfflineManController::ptr offlineControl;
    OfflineManPreloader manPreloader;
    viewer::MemoryViewer* manMemoryViewer;

};

}
}
