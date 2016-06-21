/**
* @file MessageQueue.h
* Definition of class MessageQueue.
*
* Include this file if for declaring instances of MessageQueue.
*
* @author Martin Lötzsch
*/

#pragma once

#include "MessageQueueBase.h"
#include "InMessage.h"
#include "OutMessage.h"

/**
* @class MessageQueue
* A class representing a queue of messages.
* It can be used to collect different types of messages for exchange between different
* processes or systems.
*
* Usage:
* <pre>
* MessageQueue myQueue;
* myQueue.setSize(100000); // set the size of data that can be stored in the queue
* ImageBH image1;
* myQueue.out.bin << image1; // write the binary message
* myQueue.out.finishMessage(idImage); // finish the message, set the type id of the message
* //
* // ... copy the queue between processes, systems
* //
* if (myQueue.in.getMessageID() == idImage) // check for the type of the next message
* {
*   ImageBH image2;
*   myQueue.in.bin >> image2;  // read the image from the queue
* }
* </pre>
*/
class MessageQueue
{
protected:
  MessageQueueBase queue; /**< The queue that actually manages the memory of the message queue. */

public:
  InMessage in; /**< An interface for reading messages from the queue. */
  OutMessage out; /**< An interface for writing messages to the queue. */

  /**
  * Default constructor.
  */
  MessageQueue() : in(queue), out(queue) {}

  /**
  * The method sets the size of memory which is allocated for the queue.
  * Ignored on the Win32 platform (dynamic allocation).
  * @param size The maximum size of the queue in Bytes.
  */
  void setSize(unsigned size) {queue.setSize(size);}

  /**
  * The method returns the size of memory which is needed to write the queue to a stream.
  * @return The number of bytes required.
  */
  int getStreamedSize() const {return 8 + queue.usedSize;}

  /**
  * The method calls a given message handler for all messages in the queue. Note that the messages
  * still remain in the queue and have to be removed manually with clear().
  * @param handler A reference to a message handler.
  */
  void handleAllMessages(MessageHandler& handler);

  /**
  * The method copies all messages from this queue to another queue.
  * @param other The destination queue.
  */
  void copyAllMessages(MessageQueue& other);

  /**
  * The method moves all messages from this queue to another queue.
  * @param other The destination queue.
  */
  void moveAllMessages(MessageQueue& other);

  /**
  * The method deletes older messages from the queue if newer messages of same type
  * are already in the queue. However, some message types remain untouched.
  * This method should not be called during message handling.
  */
  void removeRepetitions() {queue.removeRepetitions();}

  /**
  * The method removes all messages from the queue.
  */
  void clear() {queue.clear();}

  /**
  * The method returns whether the queue is empty.
  * @return Aren't there any messages in the queue?
  */
  bool isEmpty() const {return queue.numberOfMessages == 0;}

  /**
  * The method returns the number of messages in the queue.
  * @return The number of messages.
  */
  int getNumberOfMessages() const {return queue.numberOfMessages;}

  /**
  * The method removes a message from the queue.
  * @param message The number of the message.
  */
  void removeMessage(int message) {queue.removeMessage(message);}

  /**
  * The method removes a message from the queue.
  */
  void removeLastMessage() {if(!isEmpty()) removeMessage(getNumberOfMessages() - 1);}

  /**
  * Hacker interface for messages. Allows patching their data after they were added.
  * @param message The number of the message to be patched.
  * @param index The index of the byte to be patched in the message.
  * @param value The new value of the byte.
  */
  void patchMessage(int message, int index, char value);

  /**
  * The method writes a header for an appendable message queue to a stream.
  * @param stream The stream that is written to.
  */
  void writeAppendableHeader(Out& stream) const;

  /**
  * The method appends the message queue to a stream.
  * @param stream The stream that is appended to.
  */
  void append(Out& stream) const;

  /**
   * Writing to the queue can fail (if it is full).
   * @returns true if any of the last writes failed.
   * @note This flag is reset if finishMessage is called.
   */
  bool writeErrorOccurred() const;

protected:
  /**
  * The method copies a single message to another queue.
  * @param message The number of the message.
  * @param other The other queue.
  */
  void copyMessage(int message, MessageQueue& other);

  /**
  * The method write the message queue to a stream.
  * @param stream The stream that is written to.
  */
  void write(Out& stream) const;

  /**
  * The method reads all messages from a stream and appends them to this message queue.
  * @param stream The stream that is read from.
  */
  void append(In& stream);

  friend In& operator>>(In& stream, MessageQueue& messageQueue); /**< Gives the streaming operator access to append(). */
  friend Out& operator<<(Out& stream, const MessageQueue& messageQueue); /**< Gives the streaming operator access to write(). */
};

/**
* Streaming operator that reads a MessageQueue from a stream.
* @param stream The stream from which is read.
* @param messageQueue The MessageQueue object.
* @return The stream.
*/
In& operator>>(In& stream, MessageQueue& messageQueue);

/**
* Streaming operator that writes a MessageQueue to a stream.
* @param stream The stream to write on.
* @param messageQueue The MessageQueue object.
* @return The stream.
*/
Out& operator<<(Out& stream, const MessageQueue& messageQueue);

/**
* Streaming operator that writes a InMessage to another MessageQueue.
* @param message The InMessage to write.
* @param queue The MessageQueue object.
*/
void operator>>(InMessage& message, MessageQueue& queue);
