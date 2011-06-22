#include "OfflineDataFinder.h"

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
}

void OfflineDataFinder::setupFSBrowser() {

    fsBrowser = new QTreeView();
    fsBrowser->setModel(fsModel);
    fsBrowser->setRootIndex(fsModel->index(QDir::homePath()));
    fsBrowser->setExpanded(fsModel->index(QDir::currentPath()), true);

    fsBrowser->setColumnWidth(0, DEFAULT_NAME_COLUMN_WIDTH);
    fsBrowser->setColumnHidden(1, true);
    fsBrowser->setColumnHidden(2, true);
    fsBrowser->setColumnHidden(3, true);
}

}
}
