/**
* @file OutMessage.cpp
*
* Implementation of class OutMessage, OutBinaryMessage, OutTextMessage,
* OutConfigMessage and OutMessageQueue.
*
* @author Martin Lötzsch
*/

#include "OutMessage.h"
#include "MessageQueue.h"

OutMessageQueue::OutMessageQueue()
  : queue(0)
{
}

void OutMessageQueue::open(MessageQueueBase* q)
{
  if(queue == 0) queue = q;
}

void OutMessageQueue::writeToStream(const void* p, int size)
{
  if(queue != 0) queue->write(p, size);
}

OutBinaryMessage::OutBinaryMessage(MessageQueueBase* q)
{
  open(q);
}

OutTextMessage::OutTextMessage(MessageQueueBase* q)
{
  open(q);
}

OutTextRawMessage::OutTextRawMessage(MessageQueueBase* q)
{
  open(q);
}

OutMessage::OutMessage(MessageQueueBase& queue)
  : queue(queue), bin(&queue), text(&queue), textRaw(&queue)
{
}

bool OutMessage::finishMessage(MessageID id)
{
  return queue.finishMessage(id);
}

void OutMessage::cancelMessage()
{
  queue.cancelMessage();
}
