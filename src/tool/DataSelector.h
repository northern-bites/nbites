/**
 * @class DataSelector
 *
 * Extends DataFinder
 * A widget that lets the user browse her local files for a log
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */

#pragma once

#include <QFileSystemModel>
#include <QTreeView>
#include <QHBoxLayout>

namespace tool {

class DataSelector : public QWidget {
    Q_OBJECT;

public:
    DataSelector(QWidget *parent = 0);
    ~DataSelector();

signals:
    void signalNewDataSet(std::vector<std::string> files);

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
