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

    void addModule(portals::Module& mod);
    bool unlogFrom(std::string path);

    template<class T>
    void connectToUnlogger(portals::InPortal<T>& input)
    {
        T test;
        for (std::vector<unlog::UnlogBase*>::iterator i = unloggers.begin();
             i != unloggers.end(); i++)
        {
            if(test.GetTypeName() == (*i)->getType())
            {
                input.wireTo(dynamic_cast<unlog::UnlogModule<T> >(*i).output);
            }
        }
    }

signals:
    void signalNewProviders(std::vector<unlog::GenericProviderModule*>);

public slots:
    void run() { diagram.run(); }
    void addUnloggers(std::vector<std::string> paths);
	void setBackDir() {
		if(unlog::UnlogBase::readDir())
		   unlog::UnlogBase::readDir()=0;
	}
	void setForwardDir() {
		if(!unlog::UnlogBase::readDir())
		   unlog::UnlogBase::readDir()=1;
	}

protected:
    portals::RoboGram diagram;
    std::vector<unlog::UnlogBase*> unloggers;
    std::vector<unlog::GenericProviderModule*> providers;

    TypeMap typeMap;
};

}
