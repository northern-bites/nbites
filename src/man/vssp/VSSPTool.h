#pragma once

#include <QMainWindow>

class VSSPMain : public QWidget
{
    Q_OBJECT;

};

class VSSPTool : public QMainWindow
{
    Q_OBJECT;

public:
    VSSPTool() : QMainWindow() { this->setCentralWidget(new VSSPMain()); };
    ~VSSPTool() {};
};
