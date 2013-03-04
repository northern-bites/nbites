#include "DataSelector.h"
#include "PathConfig.h"
#include "LogDefinitions.h"

#include <iostream>
#include <vector>
#include <QCoreApplication>

namespace tool {

static const int DEFAULT_NAME_COLUMN_WIDTH = 300;

DataSelector::DataSelector(QWidget* parent) : QWidget(parent)
{
    QLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);

    setupFSModel();
    setupFSBrowser();
    layout->addWidget(fsBrowser);

    this->setLayout(layout);
}

DataSelector::~DataSelector() {
}

void DataSelector::setupLogLabels() {

    logLabel = new QLabel(this);
    logLabel->setText("Current log:");

    logPathLabel = new QLineEdit(this);
    logPathLabel->setReadOnly(true);
}

void DataSelector::setupFSModel() {
    fsModel = new QFileSystemModel(this);
    fsModel->setRootPath(QDir::homePath());
    fsModel->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
}

void DataSelector::setupFSBrowser() {

    fsBrowser = new QTreeView(this);
    fsBrowser->setModel(fsModel);
    fsBrowser->setRootIndex(fsModel->index(QString(NBITES_DIR) + "/data/logs"));
    fsBrowser->expand(fsModel->index(QDir::currentPath()));

    fsBrowser->setColumnWidth(0, DEFAULT_NAME_COLUMN_WIDTH);
    fsBrowser->setColumnHidden(1, true);
    fsBrowser->setColumnHidden(2, true);
    fsBrowser->setColumnHidden(3, true);

    connect(fsBrowser, SIGNAL(expanded(const QModelIndex&)),
            this, SLOT(folderChanged(const QModelIndex&)));
}

void DataSelector::folderChanged(const QModelIndex& index) {
    this->scanFolderForLogs(fsModel->filePath(index));
}

void DataSelector::scanFolderForLogs(QString path) {
    QString filter = QString(("*" + LOG_EXTENSION).data());
    QDir dir(path, filter);
    QFileInfoList list = dir.entryInfoList();
    std::cout << "Found " << list.size() << " log files." << std::endl;

    if (list.size() == 0) {
        return;
    }

    logPathLabel->setText(path); // notify the user of the new log

    std::vector<std::string> files;
    for (int i = 0; i < list.size(); i++) {
        QFileInfo fileInfo = list.at(i);
        std::string path = fileInfo.absoluteFilePath().toStdString();

        if (fileInfo.size() != 0) {
            files.push_back(path);
        }
        else
        {
            std::cout << "Invalid log file: " << path << std::endl;
        }
    }

    emit signalNewDataSet(files);
}

}
