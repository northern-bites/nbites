/**
 * @class DataFinder
 *
 * Abstract class that has the duty to find a data set somewhere and
 * return a data source
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 *
 */

#pragma once

#include <QWidget>

#include "io/InProvider.h"

namespace qtool {
namespace data {

class DataFinder : public QWidget {
    Q_OBJECT

public:
    DataFinder(QWidget *parent = 0) : QWidget(parent) {}

signals:
    void signalNewInputProvider(common::io::InProvider::const_ptr newInput);

};

}
}
