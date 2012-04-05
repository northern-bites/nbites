/**
 * @class QProtobufMessage
 *
 * A protobuf message that has a signal for when it gets updated
 *
 */

#pragma once

#include <QObject>

#include "io/ProtobufMessage.h"
#include "MemorySignalingInterface.h"

namespace qtool {
namespace data {

class QProtobufMessage : public QObject,
                         public common::io::NotifyingProtobufMessage {

    Q_OBJECT

public:
    QProtobufMessage(ProtoMessage_ptr protoMessage,
                     std::string name) :
                NotifyingProtobufMessage(protoMessage, name) {

        this->addSubscriber(&convertor);
        connect(&convertor, SIGNAL(subscriberUpdate()),
                    this, SLOT(newData()));
    }

    virtual ~QProtobufMessage() {}

signals:
    void dataUpdated();

protected slots:
    void newData() {
        emit dataUpdated();
    }


protected:
    SubscriberEmiterConvertor convertor;

};

}
}
