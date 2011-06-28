/**
 * @class OfflineDataFinder
 *
 * Extends DataFinder
 * A widget that lets the user browse his local files for a data source
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */

#pragma once

#include <QFileSystemModel>
#include <QTreeView>
#include <QHBoxLayout>

#include "DataFinder.h"

namespace qtool {
namespace data {

class OfflineDataFinder : public DataFinder {
    Q_OBJECT

public:
    OfflineDataFinder(QWidget *parent = 0);
    ~OfflineDataFinder();

private:
    void setupFSModel();
    void setupFSBrowser();
    void scanFolderForLogs(QString path);

private slots:
    void folderChanged(const QModelIndex& index);

private:
    QFileSystemModel* fsModel;
    QTreeView* fsBrowser;
};

}
}
