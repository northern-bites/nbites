/**
 * @class DataLoader
 *
 * A class that maintains a UI to select a DataSource
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */

#pragma once

#include <QWidget>
#include <QtGui>

#include "remote/RemoteDataFinder.h"
#include "data/OfflineDataFinder.h"
#include "DataManager.h"
#include "io/InProvider.h"

namespace qtool {
namespace data {

class DataLoader : public QWidget
{
    Q_OBJECT

public:
    DataLoader(DataManager::ptr dataManager ,QWidget *parent = 0);
    ~DataLoader();

private:
    QComboBox* setupDataSelectorBox();

private:
    DataManager::ptr dataManager;
    OfflineDataFinder* offlineDataFinder;
    remote::RemoteDataFinder* remoteDataFinder;
};

}
}
