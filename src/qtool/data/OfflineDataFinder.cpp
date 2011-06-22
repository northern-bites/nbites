#include "OfflineDataFinder.h"

#include <iostream>
#include <QCoreApplication>

namespace qtool {
namespace data {

static const int DEFAULT_NAME_COLUMN_WIDTH = 300;

OfflineDataFinder::OfflineDataFinder(QWidget* parent) :
    DataFinder(parent) {

    QLayout* layout = new QHBoxLayout;

    setupFSModel();
    setupFSBrowser();
    layout->addWidget(fsBrowser);

    this->setLayout(layout);
}

OfflineDataFinder::~OfflineDataFinder() {
    delete fsBrowser;
    delete fsModel;
}

void OfflineDataFinder::setupFSModel() {

    fsModel = new QFileSystemModel();
    fsModel->setRootPath(QDir::homePath());
    fsModel->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
}

void OfflineDataFinder::setupFSBrowser() {

    fsBrowser = new QTreeView();
    fsBrowser->setModel(fsModel);
    fsBrowser->setRootIndex(fsModel->index(QDir::homePath()));
    fsBrowser->expand(fsModel->index(QDir::currentPath()));

    fsBrowser->setColumnWidth(0, DEFAULT_NAME_COLUMN_WIDTH);
    fsBrowser->setColumnHidden(1, true);
    fsBrowser->setColumnHidden(2, true);
    fsBrowser->setColumnHidden(3, true);

    connect(fsBrowser, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(folderChanged(const QModelIndex&)));
}

void OfflineDataFinder::folderChanged(const QModelIndex& index) {
    this->scanFolderForLogs(fsModel->filePath(index));
}

void OfflineDataFinder::scanFolderForLogs(QString path) {
    QDir dir(path, "*.log");
    QFileInfoList list = dir.entryInfoList();
     std::cout << "     Bytes Filename" << std::endl;
     for (int i = 0; i < list.size(); ++i) {
         QFileInfo fileInfo = list.at(i);
         std::cout << qPrintable(QString("%1 %2").arg(fileInfo.size(), 10)
                                                 .arg(fileInfo.fileName()));
         std::cout << std::endl;
     }
}

}
}
