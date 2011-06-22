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

#include "OfflineDataFinder.h"
#include "DataSource.h"

namespace qtool {
namespace data {

class DataLoader : public QWidget
{
    Q_OBJECT

public:
    DataLoader(QWidget *parent = 0);
    ~DataLoader();

private:
    QComboBox* setupDataSelectorBox();

private:
    DataFinder* dataFinder;
};

}
}
