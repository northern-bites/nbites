/**
 * @file Range.h
 *
 * The file defines a template class to represent ranges.
 *
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 */

#pragma once

#include "Tools/Streams/InOut.h"
#include "Tools/Streams/Streamable.h"

/**
 * A template class to represent ranges. It also defines the 13 Allen relations
 */
template <class T = float> class RangeBH : public Streamable
{
public:
  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN;
    STREAM(min);
    STREAM(max);
    STREAM_REGISTER_FINISH;
  }

  T min, max;                   /**< The limits of the range. */

  /**
   * Constructor.
   * Defines an empty range.
   */
  RangeBH() {min = max = T();}

  /**
   * Constructor.
   * Defines an empty range.
   * @param minmax A conjoined starting and ending point of the empty range.
   */
  RangeBH(T minmax) {min = max = minmax;}

  /**
   * Constructor.
   * @param min The minimum of the range.
   * @param max The maximum of the range.
   */
  RangeBH(T min, T max)
  {RangeBH::min = min; RangeBH::max = max;}

  /**
   * The function enlarges the range so that a certain value will be part of it.
   * @param t The value that will be part of the range.
   * @return A reference to the range.
   */
  RangeBH<T>& add(T t)
  {
    if(min > t)
      min = t;
    if(max < t)
      max = t;
    return *this;
  }

  /**
   * The function enlarges the range so that the resulting range also contains another one.
   * @param r The range that also will be part of the range.
   * @return A reference to the range.
   */
  RangeBH<T>& add(const RangeBH<T>& r)
  {
    add(r.min);
    add(r.max);
    return *this;
  }

  /**
   * The function checks whether a certain value is in the range.
   * Note that the function is able to handle circular range, i.e. max < min.
   * @param t The value.
   * @return Is the value inside the range?
   */
  bool isInside(T t) const
  {return min <= max ? t >= min && t <= max : t >= min || t <= max;}

  /**
   * The function limits a certain value to the range.
   * Note that the function is not able to handle circular range, i.e. max < min.
   * @param t The value that will be "clipped" to the range.
   * @return The limited value.
   */
  T limit(T t) const {return t < min ? min : t > max ? max : t;} //sets a limit for a RangeBH

  /**
   * The function limits another range to this range.
   * Note that the function is able to handle circular range, i.e. max < min.
   * @param r The range that will be "clipped" to this range.
   * @return The limited value.
   */
  RangeBH<T> limit(const RangeBH<T>& r) const {return RangeBH<T>(limit(r.min), limit(r.max));} //sets the limit of a RangeBH

  /**
   * The function returns the size of the range.
   * @return The difference between the lower limit and the higher limit.
   */
  T getSize() const {return max - min;}

  /**
   * The function returns the center of the range.
   * @return The center.
   */
  T getCenter() const {return (max + min) / 2;}

  //!@name The 13 Allen relations
  //!@{
  bool operator==(const RangeBH<T>& r) const {return min == r.min && max == r.max;}
  bool operator<(const RangeBH<T>& r) const {return max < r.min;}
  bool operator>(const RangeBH<T>& r) const {return min > r.max;}
  bool meets(const RangeBH<T>& r) const {return max == r.min;}
  bool metBy(const RangeBH<T>& r) const {return min == r.max;}
  bool overlaps(const RangeBH<T>& r) const {return min < r.min && max < r.max && max > r.min;}
  bool overlappedBy(const RangeBH<T>& r) const {return min > r.min && max > r.max && min < r.max;}
  bool starts(const RangeBH<T>& r) const {return min == r.min && max < r.max;}
  bool startedBy(const RangeBH<T>& r) const {return min == r.min && max > r.max;}
  bool finishes(const RangeBH<T>& r) const {return max == r.max && min > r.min;}
  bool finishedBy(const RangeBH<T>& r) const {return max == r.max && min < r.min;}
  bool during(const RangeBH<T>& r) const {return min > r.min && max < r.max;}
  bool contains(const RangeBH<T>& r) const {return min < r.min && max > r.max;}
  //!@}
};
