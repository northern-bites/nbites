#pragma once

#include <boost/thread/mutex.hpp>
#include <map>
#include <vector>

/* A thread-safe wrapper around the stl map class
 * Uses a boost::mutex to ensure atomicity */
template <class Key, class Data>
class ConcurrentMap {
   public:
      /* Used to index into the map
       * @param key the key to look up in the map
       * @return reference to the value corresonding to the key */
      Data & operator[](const Key& key);
      /* Find how many values correspond to a key
       * @param key the key to look up in the map
       * @return the number of corresponding values (0 or 1) since it's not a multimap */
      unsigned int count(const Key& key) const;
      /* @return a vectory with a copy of all the keys
       * Useful for thread-safe iteration of keys */
      std::vector<Key> keys() const;
   private:
      /* Private instance of the stl map, being wrapped */
      std::map<Key, Data> theMap;
      /* Mutex to ensure atomicity of operations on the ConcurrentMap */
      mutable boost::mutex theLock;
};

#include "utils/ConcurrentMap.tcc"
