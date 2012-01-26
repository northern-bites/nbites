#include "DataLoader.h"

namespace qtool {
namespace data {

using namespace remote;

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
            SIGNAL(signalNewInputProvider(common::io::InProvider::ptr)),
            dataManager.get(),
            SLOT(newInputProvider(common::io::InProvider::ptr)));

    connect(offlineDataFinder,
            SIGNAL(signalNewInputProvider(common::io::InProvider::ptr)),
            dataManager.get(),
            SLOT(newInputProvider(common::io::InProvider::ptr)));

    this->setLayout(layout);
}

DataLoader::~DataLoader()
{
    delete offlineDataFinder;
    delete remoteDataFinder;
}

}
}
