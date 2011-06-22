#include "DataLoader.h"


namespace qtool {
namespace data {

DataLoader::DataLoader(QWidget *parent) :
    QWidget(parent),
    dataFinder(new DataFinder())
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);

    QComboBox* dataTypeSelector = this->setupDataSelectorBox();
    layout->addWidget(dataTypeSelector);
    layout->addWidget(dataFinder);

    this->setLayout(layout);
}

QComboBox* DataLoader::setupDataSelectorBox() {
    QComboBox* dataTypeSelector = new QComboBox;
    dataTypeSelector->addItem(QString("Offline"),
                      QVariant(static_cast<int>(DataSource::Type::offline)));
    dataTypeSelector->addItem(QString("Online"),
                      QVariant(static_cast<int>(DataSource::Type::online)));
    dataTypeSelector->addItem(QString("Old"),
                      QVariant(static_cast<int>(DataSource::Type::old)));
    return dataTypeSelector;
}

DataLoader::~DataLoader()
{
    delete dataFinder;
}

}
}
