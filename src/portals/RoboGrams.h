/*
 * Message passing infrastructure: provides in and out portal, message, module,
 * and diagram classes, plus internal thread-safety and efficiency mechanisms.
 *
 * @author Bill Silver
 * with modifications for gcc compatibility by Lizzie and Ellis.
 */

#ifndef _RoboGrams_
#define _RoboGrams_

#include <pthread.h>
#include <assert.h>
#include <vector>
#include <string>

namespace portals {

// Prohibit copy/assignment of some class
#define NO_COPY(name) \
  name(const name&);  \
  name& operator=(const name&)

// Uncomment for Windows compiler
//#define USE_WINDOWS

// **********************
// *                    *
// *  Printf To String  *
// *                    *
// **********************

std::string strprintf (const char* format, ...);

// ****************
// *              *
// *  Safe Mutex  *
// *              *
// ****************

// Wrap XGC Library Pthread Mutex functions in a class. Main value is that one can
// declare a MutexSemaphore and have init and destroy done automatically by the
// constructor and destructor.
class MutexSemaphore
{
  pthread_mutex_t mu_;

  NO_COPY(MutexSemaphore);

public:
  MutexSemaphore() { int err = pthread_mutex_init   (&mu_, 0); assert(!err);}
 ~MutexSemaphore() { int err = pthread_mutex_destroy(&mu_   ); assert(!err);}

  void lock()   { int err = pthread_mutex_lock  (&mu_); assert(!err);}
  void unlock() { int err = pthread_mutex_unlock(&mu_); assert(!err);}
};

// A protected critical section of code is created from the point of declaration
// of a Mutex until the end of its scope. C++ destruction semantics guarantees
// that the semaphore gets unlocked when a thread exits the scope, even if exit
// is done with a throw or goto. It's impossibe to forget to unlock.
class Mutex
{
  MutexSemaphore& mu_;

  NO_COPY(Mutex);

public:
  Mutex(MutexSemaphore& mu) : mu_(mu) { mu_.lock  ();}
 ~Mutex(                  )           { mu_.unlock();}
};

// ******************
// *                *
// *  Message Pool  *
// *                *
// ******************
//
// A MessagePool<T> is a pool of objects of type T that have been allocated (on the heap)
// and constructed, and so are quickly ready for use. Clients can allocate from and free
// to the pool. MessagePools are good for Google Protocol Buffers (GPB). Google says,
// "Reuse message objects when possible. ... if you are handling many messages with the same
// type and similar structure in succession, it is a good idea to reuse the same message
// object each time to take load off the memory allocator."
//
// Another time savings is that messages in the pool are pre-constructed and not destroyed
// when freed. Often this is fine, but sometimes it's desirable to perform some client-
// specific actions when a message is allocated or freed. Each instance of a MessagePool has
// flags that control whether initialization or finalization is done. The messgae type T
// must implement the functions initialize() and finalize() to say what to do if the flags
// are set. Note that a GPB must be wrapped in a class that does this.
//
// T must also implement the function std::string describe() const so that the contents of
// a pool can be examined for testing.
//
// The size of a pool is set at construction time and cannot be changed. Any application is
// expected to use some maximum number of messages of a given type T. Allocations beyond
// that number are likely caused by heap leak bugs. In DEBUG mode going beyond the pool
// limit causes an assertion failure. For production builds, there is one special message
// in the pool that is returned for every allocation over the limit. The client who receives
// this message will almost surely no longer work properly, but the alternative is generally
// a total system crash, which is much worse for a competing robot.

// Base class prevents template bloat
class MessagePoolBase
{
protected:
  int*  list_;          // Linked list of pool indicies
  int   freeList_;      // Index of first free pool index

  MutexSemaphore
        mu_;            // Protect critical sections

  int   maxSize_;       // Number of messages in the pool
  int   highWaterMark_; // Maximum number that have ever been in use, including over-allocations
  int   inUse_;         // Number currently in use

  bool  initialize_;    // Call initialize() when a message is allocated?
  bool  finalize_;      // Call finalize() when a message is freed?

  MessagePoolBase(int maxSize);
 ~MessagePoolBase();

 // These are used by MessagePool<T> to do all of the T-independent work
 int alloc();           // Return a new pool index
 void free(int);        // Free the specified index

 // Get description from MessagePool<T>
 virtual std::string getDescription(int index) const = 0;

public:
  int highWatermark() const { return highWaterMark_;}
  bool overflow() const { return highWaterMark_ > maxSize_;}

  bool initialize() const { return initialize_;}
  void initialize(bool b) { initialize_ = b;}
  // effect   Get/set initialization flag

  bool finalize() const { return finalize_;}
  void finalize(bool b) { finalize_ = b;}
  // effect   Get/set finalization flag

  std::string describe(const char* poolName) const;
  // returns  A complete description of the state and contents of the pool
  // note     poolName is any string the client wants to identify the
  //          description
};

template<class T>
class MessagePool : public MessagePoolBase
{
  T* pool_;

  NO_COPY(MessagePool);

public:
  MessagePool(int maxSize);
 ~MessagePool();

  T* alloc();
  void free(T*);
  // effect   Allocate/free message of type T
  //          Call T->initialize()/T->finalize() if enabled

protected:
  virtual std::string getDescription(int index) const { return pool_[index].describe();}
};

template<class T>
MessagePool<T>::MessagePool(int maxSize)
  : MessagePoolBase(maxSize)
{
  pool_ = new T[maxSize];
}

template<class T>
MessagePool<T>::~MessagePool()
{
  delete [] pool_;
}

template<class T>
T* MessagePool<T>::alloc()
{
  T* p = &pool_[MessagePoolBase::alloc()];
  if (initialize_)
    p->initialize();
  return p;
}

template<class T>
void MessagePool<T>::free(T* p)
{
  if (finalize_)
    p->finalize();
  MessagePoolBase::free(p - pool_);
}

// ***************************************************
// *                                                 *
// *  Multi-Processor Thread-Safe Atomic Operations  *
// *                                                 *
// ***************************************************
//
// Extremely efficient, non-portable atomic operations for creating
// thread-safe primitaves in multi-processor systems. Uses special x86
// instructions for that purpose. These are written in MASM/Intel
// syntax, using inline assembly. ATT syntax can be added as a conditional
// compilation alternative. For a non-x86 processor, versions using a Mutex
// can be written, but would be much slower.
//
// Note that I don't know if a given compiler will inline a function containing
// inline assembly.

// Increment the specified counter.

inline void atomicIncrement(int& counter)
{
#ifdef USE_WINDOWS
    _asm
    {
     mov eax, counter
     lock inc dword ptr [eax]
    }
#else
    asm("lock incl %0"
        :
        :"m" (counter)
        );
#endif
}

// Decrement the specified counter and return -1, 0, or +1 to indicate that
// the resulting value was <0, 0, or >0.
inline int atomicDecrement(int& counter)
{
  int status = 0;
#ifdef USE_WINDOWS
  _asm
  {
    mov ecx, counter
    xor eax, eax
    lock dec dword ptr [ecx]
    mov ecx, eax
    setg al
    setl cl
    sub eax, ecx
    mov status, eax
  }
#else
  asm("xor %%eax, %%eax;\n"
      "xor %%ecx, %%ecx;\n"
      "lock decl %1;\n"
      "setg %%al;\n"
      "setl %%cl;\n"
      "sub %%ecx, %%eax;\n"
      "mov %%eax, %0"
      :"=r" (status)
      :"m" (counter)
      :"%eax", "%ecx"
      );
#endif
  return status;
}

// Exchange src with the contents of memory pointed to by dst, and return what was
// in that memory location.
inline void* atomicExchange(void* src, void* dst)
{
#ifdef USE_WINDOWS
  _asm
  {
    mov eax, src
    mov ecx, dst
    xchg [ecx], eax   // lock not needed, xchg implies it
    mov src, eax
  }
#else
  asm("mov %1, %%eax;\n"
      "mov %2, %%ecx;\n"
      "xchg (%%ecx), %%eax;\n"
      "mov %%eax, %0"
      : "=r" (src)
      : "r" (src), "r" (dst)
      : "%eax", "%ecx"
      );
#endif
  return src;
}

// ****************************
// *                          *
// *  Thread-Safe References  *
// *                          *
// ****************************

// Base class for objects to be shared by reference counting.
class RefCounter
{
  friend class Reference;

  int counter_;   // Number of live references to this object

  void grab() { atomicIncrement(counter_);}
  // effect   Grab a new reference

  void drop() { int c = atomicDecrement(counter_); assert(c >= 0); if (!c) release();}
  // effect   Drop an existing reference and call object-specific release() if no
  //          more live references

  NO_COPY(RefCounter);

protected:
  RefCounter() : counter_(0) {}
  virtual ~RefCounter() {}

  virtual void release() { delete this;}
  // effect   Delete the most-derived object on release
  // override To provide different behavior, e.g. return to MessagePool
};

// Base class for references to objects shared by reference counting. References
// can be null.
class Reference
{
  static MutexSemaphore mu_;  // to protect critical sections

protected:
  RefCounter* rc_;

  Reference(RefCounter* rc) : rc_(rc) { if (rc_) rc_->grab();}
  // effect   Construct new reference from existing ref-counted object.
  // note     Available to derived classes only.

public:
  Reference() : rc_(0) {}
  // effect   Construct null reference

 ~Reference() { if (rc_) rc_->drop();}
 // effect    Destroy reference.
 // note      If this reference is being destroyed, no other thread is sharing it.

  Reference(const Reference&);
  Reference& operator=(Reference);
  // effect   Thread-safe copy construction and assignment. See implementation for details.
};

// ********************
// *                  *
// *  Message Holder  *
// *                  *
// ********************
//
// A MessageHolder<T> makes any C++ class or type sharable by reference counting.
template<class T> class Message;

template<class T>
class MessageHolder : private RefCounter
{
  friend class Message<T>;

  static MessagePool<MessageHolder<T> > pool_;
  // note     This is the pool in which all messages of this type are held

  T message;
  // note     The message (object of type T) is the only contents (other than the
  //          base class).

  virtual void release();
  // effect   Return to pool instead of deleting.

  // Default copy/assignment prohibited by base class

public:
  // These functions are required by MessagePool<T>. They redirect the operation to
  // appropriate functions defined by GPBs. Non-GPB classes can implement these
  // functions or provide template specializations. if T is a native type, specializations
  // must be provided.
  void initialize() { message.Clear();}
  void finalize() {}
  std::string describe() const { return message.DebugString();}
};

// Override the default pool size for a given type by providing a template specialization.
#define SET_POOL_SIZE(type, size) \
    template<> MessagePool<MessageHolder<type> > MessageHolder<type>::pool_(size)

// For setting the default size within this template, the template<> isn't needed
#define SET_POOL_SIZE_DEFAULT(type, size) \
    MessagePool<MessageHolder<type> > MessageHolder<type>::pool_(size)

// Here is the default pool size
template<class T>
SET_POOL_SIZE_DEFAULT(T, 8);

template<class T>
void MessageHolder<T>::release()
{
  pool_.free(this);
}

// ***********************
// *                     *
// *  Message Reference  *
// *                     *
// ***********************
//
// A reference to a message. This provides very efficient copying (by sharing). Note that
// a Message<T> holds just a pointer. Messages can be null.
template<class T>
class Message : public Reference
{
public:
  Message() {}
  // effect   Construct null message reference

  Message(const T*);
  // effect   Construct new message by allocating from the pool and then copying the
  //          specified T to the new message. If T* is null the new message is
  //          not initialized, except if pool initialization is enabled.
  // note     For maximum efficiency, make T* null and set the message fields
  //          yourself.

  // Default copy/assign allowed

  const T* get() const { return rc_ ? &((const MessageHolder<T>*) rc_)->message : 0;}
        T* get()       { return rc_ ? &((      MessageHolder<T>*) rc_)->message : 0;}
  // effect   Return a pointer to the message referenced.
  
  // The following static functions are used to operate the underlying MessagePool containing
  // the messages. They are here instead of in MessageHolder<T> for client convenience.
  static void setInitialize(bool b) { MessageHolder<T>::pool_.initialize(b);}
  static void setFinalize  (bool b) { MessageHolder<T>::pool_.finalize  (b);}

  static std::string describe(const char* poolName) { return MessageHolder<T>::pool_.describe(poolName);}
};

template<class T>
Message<T>::Message(const T* m)
  : Reference(MessageHolder<T>::pool_.alloc())
{
  if (m)
    *get() = *m;
}

// ******************
// *                *
// *  Input Portal  *
// *                *
// ******************

template<class T>
class OutPortal;

template<class T>
class InPortal
{
  OutPortal<T>* wiredTo_;

  Message<T> message_;

  bool asynchronous_;

public:
  InPortal() : wiredTo_(0) {}
  // effect   Construct inout portal not wired to anything.

  // Default copy/assign OK

  const T& message() const { const T* m = message_.get(); assert(m); return *m;}
  // returns  The message on this input, which is the one last latched if this input
  //          is wired to something, or the fixed message otherwise.
  // requires The message cannot be null.
  // note     Message is const, client cannot modify

  void setMessage(const Message<T>& message) { message_ = message;}
  // effect   Set a fixed message.

  void wireTo(OutPortal<T>* op, bool asynchronous = false)
  {
    wiredTo_ = op;
    asynchronous_ = asynchronous;
  }
  // effect   Wire this inout to the specified output. The wire can be synchronous
  //          or asynchronous. If the output is null, this input will hold a fixed
  //          message.

  void latch() { if (wiredTo_) message_ = wiredTo_->getMessage(asynchronous_);}
  // effect   Latch a new message from the output to which this input is wired,
  //          if any. If no wire, do nothing.
  // note     Latching a synchronous wire causes the module containing the output
  //          to be run if necessary.
  //          Latching an asynchronous wire gets the most recent message on the
  //          output.
};

// ***********************
// *                     *
// *  Module Base Class  *
// *                     *
// ***********************

class Module
{
  int runState_;
  // note    0 means needs to run
  //         1 means already run
  //        -1 means being run, if asked to run again then we have a cycle error

public:
  Module() : runState_(0) {}
  virtual ~Module() {}

  // Default copy/assign OK (maybe unwise)

  // run() is what clients use for synchronous running. Recursively traverses the
  // graph of modules. The actual work is done by the protected virtual function run_().
  void run()
  {
    assert(runState_ >= 0);
    if (runState_ == 0)
    {
      runState_ = -1;
      run_();
      runState_ = 1;
    }
  }

  void reset() { runState_ = 0;}

protected:
  virtual void run_() = 0;
  // effect   Run this module.
  // override To provide module-specific functionality

  Module& base() { return *this;}
  // returns  Reference to this base class.
  // note     Use to avoid compiler warning in constructing output portals.
};

// *******************
// *                 *
// *  Output Portal  *
// *                 *
// *******************

template<class T>
class OutPortal
{
  Message<T> message_;    // The current message on this output

  Module& module_;        // The module containing this output

public:
  OutPortal(Module& m) : module_(m) {}
  // effect   Construct output on specified module

  // Default copy/assign OK

  const Message<T>& getMessage(bool asynchronous) const;
  // returns  The message on this output.
  // effect   If synchronous, run the module if necessary.
  // note     Message is const, client cannot modify

  void setMessage(const Message<T>& message) { message_ = message;}
  // effect   Set message on this output
};

template<class T>
const Message<T>& OutPortal<T>::getMessage(bool asynchronous) const
{
  if (!asynchronous)
    module_.run();
  return message_;
}

// ****************************
// *                          *
// *  RoboCup Wiring Diagram  *
// *                          *
// ****************************

class RoboGram
{
  std::vector<Module*> modules_;

public:
  // Default copy/assign OK

  void addModule(Module& m) { modules_.push_back(&m);}

  void run();
};

}

#endif

