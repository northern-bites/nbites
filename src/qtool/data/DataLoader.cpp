#include "DataLoader.h"

namespace qtool {
namespace data {

using namespace remote;
using namespace common::io;

DataLoader::DataLoader(DataManager::ptr dataManager, QWidget *parent) :
    QWidget(parent),
    dataManager(dataManager),
    offlineDataFinder(new OfflineDataFinder(this)),
    remoteDataFinder(new RemoteDataFinder(this))
{
    QHBoxLayout *layout = new QHBoxLayout;

    layout->addWidget(offlineDataFinder);
    layout->addWidget(remoteDataFinder);

    connect(remoteDataFinder,
            SIGNAL(signalNewInputProvider(common::io::InProvider::ptr, MObject_ID)),
            dataManager.get(),
            SLOT(newInputProvider(common::io::InProvider::ptr, MObject_ID)));

    connect(remoteDataFinder,
            SIGNAL(signalNewDataSet()),
            dataManager.get(),
            SLOT(reset()));

    connect(offlineDataFinder,
            SIGNAL(signalNewInputProvider(common::io::InProvider::ptr, MObject_ID)),
            dataManager.get(),
            SLOT(newInputProvider(common::io::InProvider::ptr, MObject_ID)));

    connect(offlineDataFinder,
            SIGNAL(signalNewDataSet()),
            dataManager.get(),
            SLOT(reset()));

    this->setLayout(layout);
}

DataLoader::~DataLoader()
{
    delete offlineDataFinder;
    delete remoteDataFinder;
}

}
}
