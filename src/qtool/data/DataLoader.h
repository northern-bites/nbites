/**
 * @class DataLoader
 *
 * A class that maintains a UI to select a DataSource
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */

#pragma once

#include <QWidget>

class DataLoader : public QWidget
{
    Q_OBJECT

public:
    DataLoader(QWidget *parent = 0);
    ~DataLoader();

};
