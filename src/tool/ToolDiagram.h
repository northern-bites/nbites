#pragma once

#include <QWidget>
#include "unlog/UnlogModule.h"
#include "Header.pb.h"
#include <iostream>

namespace tool{

// Things for mapping names to types
typedef unlog::UnlogBase*(* Construct)(std::string);
typedef std::map<std::string, Construct> TypeMap;

#define ADD_MAPPED_TYPE(name) \
    typeMap[std::string("messages.") + std::string(#name)] = \
        &makeUnlogger<messages::name>;

template<class T>
unlog::UnlogBase* makeUnlogger(std::string path)
{
    return new unlog::UnlogModule<T>(path);
}

// Actual class

class ToolDiagram : public QObject
{
    Q_OBJECT;

public:
    ToolDiagram(QWidget *parent = 0);

    void addModule(portals::Module& mod) { diagram->addModule(mod); }
    bool unlogFrom(std::string path);

    template<class T>
    bool connectToUnlogger(portals::InPortal<T>& input,
                           std::string path = "none")
    {
        T test;
        for (std::vector<unlog::UnlogBase*>::iterator i = unloggers.begin();
             i != unloggers.end(); i++)
        {
            if(test.GetTypeName() == (*i)->getType())
            {
                unlog::UnlogModule<T>* u =
                    dynamic_cast<unlog::UnlogModule<T>*>(*i);
                input.wireTo(&u->output);
                std::cout << "Connected successfully to "
                          << test.GetTypeName() << " unlogger!"
                          << std::endl;
                return true;
            }
        }

        std::cout << "Tried to connect a module to a nonexistent unlogger!"
                  << std::endl;
        return false;
    }

signals:
    void signalNewDisplayWidget(QWidget*, std::string);
    void signalDeleteDisplayWidgets();
    void signalUnloggersReady();

public slots:
    void runForward();
    void runBackward();
    void addUnloggers(std::vector<std::string> paths);

protected:
    portals::RoboGram* diagram;
    std::vector<unlog::UnlogBase*> unloggers;
    std::vector<portals::Module*> displays;

    TypeMap typeMap;
};

template<>
bool ToolDiagram::connectToUnlogger(portals::InPortal<messages::YUVImage>& input,
                                    std::string path);

}
