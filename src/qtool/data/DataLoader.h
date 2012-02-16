/**
 * @class DataLoader
 *
 * A class that combines an offlineDataFinder for browsing for logs offline and
 * a remoteDataFinder for streaming from the robots in a single widget and
 * also connects these widgets up to the dataManager
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
    DataManager::ptr dataManager;
    OfflineDataFinder* offlineDataFinder;
    remote::RemoteDataFinder* remoteDataFinder;
};

}
}
