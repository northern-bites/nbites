#include "OfflineDataFinder.h"

#include <iostream>
#include <QCoreApplication>

#include "NaoPaths.h"
#include "man/memory/parse/Parser.h"
#include "man/include/io/FileFDProvider.h"

namespace qtool {
namespace data {

using man::include::io::FDProvider;
using man::include::io::FileFDProvider;
using man::memory::parse::Parser;
using namespace man::include::paths;

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

    connect(fsBrowser, SIGNAL(expanded(const QModelIndex&)),
            this, SLOT(folderChanged(const QModelIndex&)));
}

void OfflineDataFinder::folderChanged(const QModelIndex& index) {
    this->scanFolderForLogs(fsModel->filePath(index));
}

void OfflineDataFinder::scanFolderForLogs(QString path) {
    QString filter = QString(("*" + NAO_LOG_EXTENSION).data());
    QDir dir(path, filter);
    QFileInfoList list = dir.entryInfoList();

    DataSource::ptr dataSource;
    if (list.size() == 0) {
        return;
    }

    dataSource = DataSource::ptr(new DataSource(DataSource::offline));
    for (int i = 0; i < list.size(); i++) {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.size() != 0) {
            std::string path = fileInfo.absoluteFilePath().toStdString();
            FDProvider::ptr fdprovider = FDProvider::ptr(new FileFDProvider(path, O_RDONLY));
            MObject_ID logID = static_cast<MObject_ID>(Parser::peekAtLogID(
                    fdprovider->getFileDescriptor()));
            dataSource->addFDProvider(logID, fdprovider);
        }
    }
    emit signalNewDataSource(dataSource);
}
}
}
