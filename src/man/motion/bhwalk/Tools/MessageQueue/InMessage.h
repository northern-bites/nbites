/**
* @file InMessage.h
*
* Declaration of class InMessageQueue, InBinaryMessage, InTextMessage,
* InConfigMessage, InMessage and MessageHandler.
*
* Include that file for the definition of classes that derive from MessageHandler.
*
* @author Martin Lötzsch
*/

#pragma once

#include "Tools/Streams/InStreams.h"
#include "MessageQueueBase.h"

class MessageQueue;
/**
* @class InMessageQueue
*
* A PhysicalInStream that reads the data from a MessageQueue.
*/
class InMessageQueue : public PhysicalInStream
{
private:
  /** The queue where the data are read from */
  MessageQueueBase* queue;

public:
  /** Default constructor */
  InMessageQueue();


  /**
  * The function states whether the stream actually exists.
  * @return Does the file exist?
  */
  virtual bool exists() const;

  /**
  * The function states whether the end of the file has been reached.
  * @return End of file reached?
  */
  virtual bool getEof() const;

protected:
  /**
  * opens the stream.
  * @param q A pointer to the message queue base
    */
  void open(MessageQueueBase* q);

  /**
  * The function reads a number of bytes from the stream.
  * @param p The address the data is written to. Note that p
  *          must point to a memory area that is at least
  *          "size" bytes large.
  * @param size The number of bytes to be read.
  */
  virtual void readFromStream(void* p, int size);
};

/**
* @class InBinaryMessage
*
* A binary stream from a message queue.
*/
class InBinaryMessage : public InStream<InMessageQueue, InBinary>
{
public:
  /**
  * Constructor
  * @param q A pointer to the message queue base
    */
  InBinaryMessage(MessageQueueBase* q);

  /**
  * The function returns whether this is a binary stream.
  * @return Does it output data in binary format?
  */
  virtual bool isBinary() const {return true;}
};

/**
* @class InTextMessage
*
* A text stream from a message queue.
*/
class InTextMessage : public InStream<InMessageQueue, InText>
{
public:
  /**
  * Constructor
  * @param q A pointer to the message queue base
    */
  InTextMessage(MessageQueueBase* q);

  /**
  * Reads the complete remaining stream and returns it as a single string.
  * @return The string containing the remaining contents of the stream.
  */
  std::string readAll();
};

/**
* @class InConfigMessage
*
* A config-file-style text stream from a message queue.
*/
class InConfigMessage : public InStream<InMessageQueue, InConfig>
{
public:
  /**
  * Constructor
  * @param q A pointer to the message queue base
    */
  InConfigMessage(MessageQueueBase* q);
};

/**
* @class InMessage
*
* An Interface for reading single messages from a MessageQueue that is used by
* MessageHandler derivates.
*
* Use the bin, text or config member for formated reading from a message queue.
*/
class InMessage
{
private:
  /**
  * The message queue where the messages are read from. Note that the selection which message is
  * read is not done by InMessageQueue but by MessageQueue itself.
    */
  MessageQueueBase& queue;

public:
  /** An interface for reading binary messages from the queue */
  InBinaryMessage bin;

  /** An interface for reading text messages from the queue */
  InTextMessage text;

  /** An interface for reading config-file-style text messages from the queue */
  InConfigMessage config;

  /**
  * Constructor
  * @param queue A reference to a MessageQueueBase
  */
  InMessage(MessageQueueBase& queue);

  /** returns the message id of the current message */
  MessageID getMessageID() const;

  /** returns the message size of the current message */
  int getMessageSize() const;

  /**
   * The method returns the number of bytes not read yet in the current message.
   * @return The number of bytes left.
   */
  int getBytesLeft() const;

  /** sets the read position to 0 so that the message can be read again */
  void resetReadPosition();

protected:
  /**
  * Gives a direct read access to the message.
  * @return The adress of the first byte of the message.
  */
  const char* getData() const;

  /** gives the class MessageQueue access to protected members */
  friend class MessageQueue;

  /** gives the operator that copies a InMessage to another queue access to protected members */
  friend void operator >> (InMessage& message, MessageQueue& queue);

  /** Gives the stream operator access to protected members */
  friend Out& operator<<(Out& stream, const MessageQueue& messageQueue);
};

/**
* @class MessageHandler
*
* Responsible for distribution of incoming messages.
* Derive any class that shall receive messages from MessageHandler and implement the
* handleMessage() function.
*
* The MessageQueue needs a reference to a MessageHandler to distribute incoming messages.
*/
class MessageHandler
{
public:
  /**
   * Virtual destructor (for bush).
   */
  virtual ~MessageHandler() {}

  /**
  * Called from a MessageQueue to distribute messages.
  * Use message.getMessageID to decide if the message is relavant for
  * the MesssageHandler derivate.
  * Use message.bin, message.text or message.config as In streams to get the data from.
  * @param message The message that can be read.
  * @return true if the message was read (handled).
  */
  virtual bool handleMessage(InMessage& message) = 0;
};
