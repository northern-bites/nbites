/**
 * @file RingBufferWithSum.h
 *
 * Declaration of template class RingBufferWithSumBH
 *
 * @author Matthias Jüngel
 * @author Tobias Oberlies
 */

#pragma once


/**
 * @class RingBufferWithSumBH
 *
 * Template class for cyclic buffering of the last n values of the type C
 * and with a function that returns the sum of all entries.
 */
template <class C, int n> class RingBufferWithSumBH
{
public:
  /** Constructor */
  RingBufferWithSumBH() {init();}

  /**
   * initializes the RingBufferWithSumBH
   */
  inline void init() {current = n - 1; numberOfEntries = 0; sum = C();}

  /**
   * adds an entry to the buffer
   * \param value value to be added
   */
  inline void add(C value)
  {
    if(numberOfEntries == n) sum -= getEntry(numberOfEntries - 1);
    sum += value;
    current++;
    current %= n;
    if(++numberOfEntries >= n) numberOfEntries = n;
    buffer[current] = value;
  }

  /**
   * returns an entry
   * \param i index of entry counting from last added (last=0,...)
   * \return a reference to the buffer entry
   */
  inline C getEntry(int i) const
  {
    return buffer[(n + current - i) % n];
  }

  /**
   * returns the sum of all entries
   */
  inline C getSum() const
  {
    return sum;
  }

  /**
   * returns the smallest entry
   * \return the smallest entry
   */
  C getMinimum() const
  {
    // Return 0 if buffer is empty
    if(0 == numberOfEntries) return C();

    C min = buffer[0];
    for(int i = 0; i < numberOfEntries; i++)
    {
      if(buffer[i] < min) min = buffer[i];
    }
    return min;
  }

    /**
   * \return the biggest entry
   */
  C getMaximum() const
  {
    // Return 0 if buffer is empty
    if(0 == numberOfEntries) return C();

    C max = buffer[0];
    for(int i = 0; i < numberOfEntries; i++)
    {
      if(buffer[i] > max) max = buffer[i];
    }
    return max;
  }

  /**
   * returns the average value of all entries
   * \return the average value
   */
  inline C getAverage() const
  {
    // Return 0 if buffer is empty
    if(0 == numberOfEntries) return C();

    return (sum / numberOfEntries);
  }

  /**
   * returns the average value of all entries
   * \return the average value
   */
  inline C getAverageFloat() const
  {
    // Return 0 if buffer is empty
    if (0==numberOfEntries) return C();
    return (sum / static_cast<float>(numberOfEntries));
  }

  /// 
  /// returns the variance value of all entries
  /// \return the variance value
  ///
  C getVariance() const
  {
    // Return 0 if buffer is empty
    if (numberOfEntries <= 1) return C();
    C avg = getAverage();
    C variance = 0;
    for(int i = 0; i < numberOfEntries; i++)
    {
      variance += pow((buffer[i] - avg), 2.0f);
    }
    return variance / (numberOfEntries-1);
  }

  /// 
  /// returns the standard deviation value of all entries
  /// \return the standard deviation value
  ///
  C getStdDev() const
  {
    // Return 0 if buffer is empty
    if (0==numberOfEntries) return C();
    return pow(getVariance(), 0.5f);
  }
  ///
  /// returns the precision value of all entries
  /// \return the precision value
  ///
  C getPrecision() const
  {
    // Return 0 if buffer is empty
    if (0==numberOfEntries) return C();
    return 1 / getVariance();
  }

  /**
   * returns an entry
   * \param i index of entry counting from last added (last=0,...)
   * \return a reference to the buffer entry
   */
  inline C operator[](int i) const
  {
    return buffer[(n + current - i) % n];
  }

  /** Returns the number of elements that are currently in the ring buffer
  * \return The number
  */
  inline int getNumberOfEntries() const
  {
    return numberOfEntries;
  }

  /**
  * Returns the maximum entry count.
  * \return The maximum entry count.
  */
  inline int getMaxEntries() const
  {
    return n;
  }

  /**
  * Determines whether maximum entry count equals actual number of entries.
  * @return true iff getMaxEntries == getNumberOfEntries.
  */
  inline bool isFilled() const
  {
    return getMaxEntries() == getNumberOfEntries();
  }

private:
  int current;
  int numberOfEntries;
  C buffer[n];
  C sum;
};
