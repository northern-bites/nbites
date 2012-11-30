#pragma once

#include <QMainWindow>
#include "LogModule.h"
#include "MemoryModule.h"

class VSSPMain : public QWidget
{
    Q_OBJECT;

};

class VSSPTool : public QMainWindow
{
    Q_OBJECT;

public:
    VSSPTool() : QMainWindow()
    {
        this->setCentralWidget(new VSSPMain());

        LogModule thing("/home/ecat/nbites/data/logs/failed_walktogoals/returnfrompenalty/MVision.log");
        MemoryModule memory;

        memory.input.wireTo(&thing.output);

        RoboGram diagram;
        diagram.addModule(thing);
        diagram.addModule(memory);

        for (int i = 0; i < 3; i++)
        {
            diagram.run();
        }
    };

    ~VSSPTool() {};
};
