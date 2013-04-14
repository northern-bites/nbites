// RoboGrams.cpp : Defines the entry point for the console application.
//

#include "RoboGrams.h"
#include <stdarg.h>
#include <cstdio>

// **********************
// *                    *
// *  Printf To String  *
// *                    *
// **********************

namespace portals {

std::string strprintf (const char* format, ...)
{
  char buf[1024];
  va_list ap;
  va_start (ap, format);
  vsnprintf (buf, sizeof(buf), format, ap);
  va_end (ap);
  return std::string(buf);
}

// ****************************
// *                          *
// *  Thread-Safe References  *
// *                          *
// ****************************

MutexSemaphore Reference::mu_;

// A Mutex is necessary because reading the pointer and incrementing the counter
// must be atomic.
Reference::Reference(const Reference& r)
{
  Mutex mu(mu_);
  rc_ = r.rc_;
  if (rc_) rc_->grab();
}

// The argument is a Reference, not a const Reference&, so a new reference is
// safely constructed using the above copy constructor. Once this is done
// the atomicExchange keeps this thread-safe. There is never a point at which
// references and their counters are inconsistent.
Reference& Reference::operator=(Reference r)
{
  if (r.rc_) r.rc_->grab();
  RefCounter* temp = (RefCounter*)atomicExchange(r.rc_, &rc_);
  if (temp) temp->drop();
  return *this;
}

// ******************
// *                *
// *  Message Pool  *
// *                *
// ******************
//
// The special pool message that is returned for over-allocations is index 0. This
// is accomplished by making the free list circular at the end: index 0 points to
// itself.
MessagePoolBase::MessagePoolBase(int maxSize)
  : maxSize_(maxSize), highWaterMark_(0), inUse_(0), initialize_(true), finalize_(false)
{
  assert(maxSize > 0);

  list_ = new int[maxSize];

  list_[0] = 0;
  for (int i = 1; i < maxSize; ++i)
    list_[i] = i - 1;
  freeList_ = maxSize - 1;
}

MessagePoolBase::~MessagePoolBase()
{
  delete [] list_;
}

int MessagePoolBase::alloc()
{
  assert(inUse_ < maxSize_);
  int i;
  {
    Mutex mu(mu_);
    i = freeList_;
    freeList_ = list_[i];
    if (i)
      list_[i] = -1;

    ++inUse_;
    if (inUse_ > highWaterMark_)
      highWaterMark_ = inUse_;
  }

  return i;
}

void MessagePoolBase::free(int i)
{
  {
    Mutex mu(mu_);
    if (i)
    {
      list_[i] = freeList_;
      freeList_ = i;
    }
    --inUse_;
  }
  assert(inUse_ >= 0);
}

std::string MessagePoolBase::describe(const char* poolName) const
{
  std::string s = strprintf("\nPool %s: %d of %d buffers in use, max %d used\n",
                            poolName, inUse_, maxSize_, highWaterMark_);
  for (int i = 0; i < maxSize_; ++i)
  {
    s += strprintf("%2d: ", i);
    if (list_[i] >= 0 && !(i == 0 && inUse_ >= maxSize_))
      s += "free\n";
    else
    {
      std::string t = getDescription(i);

      if (t.at(t.length()-1) == '\n')
          t.erase(t.end()-1);

      int p = 0;
      while (true)
      {
        if (p != 0)
          s += "    ";
        unsigned int q = t.find('\n', p);
        if (q == std::string::npos)
        {
          s += t.substr(p);
          break;
        }
        else
        {
          s += t.substr(p, q - p) + '\n';
          p = q + 1;
        }
      }
      s += '\n';
    }
  }

  return s;
}

// ****************************
// *                          *
// *  RoboCup Wiring Diagram  *
// *                          *
// ****************************

void RoboGram::run()
{
  for (unsigned int i = 0; i < modules_.size(); ++i)
    modules_[i]->reset();
  for (unsigned int i = 0; i < modules_.size(); ++i)
    modules_[i]->run();
}

}
