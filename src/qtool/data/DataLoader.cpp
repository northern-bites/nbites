#include "DataLoader.h"


namespace qtool {
namespace data {

DataLoader::DataLoader(DataManager* dataManager, QWidget *parent) :
    QWidget(parent),
    dataManager(dataManager),
    dataFinder(new OfflineDataFinder())
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);

    QComboBox* dataTypeSelector = this->setupDataSelectorBox();
    layout->addWidget(dataTypeSelector);
    layout->addWidget(dataFinder);

    connect(dataFinder, SIGNAL(signalNewDataSource(DataSource::ptr)),
            this, SLOT(newDataSource(DataSource::ptr)));

    this->setLayout(layout);
}

QComboBox* DataLoader::setupDataSelectorBox() {
    QComboBox* dataTypeSelector = new QComboBox;
    dataTypeSelector->addItem(QString("Offline"),
                      QVariant(static_cast<int>(DataSource::offline)));
    dataTypeSelector->addItem(QString("Online"),
                      QVariant(static_cast<int>(DataSource::online)));
    dataTypeSelector->addItem(QString("Old"),
                      QVariant(static_cast<int>(DataSource::old)));
    return dataTypeSelector;
}

DataLoader::~DataLoader()
{
    delete dataFinder;
}

void DataLoader::newDataSource(DataSource::ptr dataSource) {
    dataManager->newDataSource(dataSource);
}

}
}
