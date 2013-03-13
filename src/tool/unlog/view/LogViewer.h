/**
 * @class Viewer
 *
 * A class that maintains a UI that views the data
 * that the data manager has
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */

#pragma once

#include <QWidget>
#include <QtGui>
#include <vector>
#include <QCheckBox>
#include <google/protobuf/message.h>

#include "RoboGrams.h"
#include "ProtoViewer.h"

namespace tool {
namespace unlog {
namespace view {

class GenericMessageProviderBase : public QObject,
                                   public portals::Module
{
    Q_OBJECT;

public:
    GenericMessageProviderBase(std::string typeName);

    void updateMessage(google::protobuf::Message* message);
    google::protobuf::Message* getMessage() { return message; }

signals:
    void messageUpdated();

protected:
    virtual void run_() = 0;
    google::protobuf::Message* message;
    std::string type;
};

template<class T>
class GenericMessageProvider : public GenericMessageProviderBase
{
public:
    GenericMessageProvider(std::string typeName);

    portals::InPortal<T> input;

protected:
    virtual void run_()
    {
        input.latch();
        message->CopyFrom(input.message());
        emit messageUpdated();
    }
};

class LogViewer : public QMainWindow {
    Q_OBJECT;

public:
    LogViewer(QWidget* parent = 0);

signals:
    void newModule(portals::Module& mod);

private:
    std::vector<QDockWidget*> dockWidget;
};

}
}
}
