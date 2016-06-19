template <class Key, class Data>
Data & ConcurrentMap<Key, Data>::operator[] (const Key &key){
   boost::mutex::scoped_lock lock(theLock);
   return theMap[key];
}

template <class Key, class Data>
unsigned int ConcurrentMap<Key, Data>::count(const Key& key) const {
   boost::mutex::scoped_lock lock(theLock);
   return theMap.count(key);
}

template <class Key, class Data>
std::vector<Key> ConcurrentMap<Key, Data>::keys() const {
   std::vector<Key> keyVec;
   boost::mutex::scoped_lock lock(theLock);

   typename std::map<Key, Data>::const_iterator it;

   for (it = theMap.begin(); it != theMap.end(); ++it) {
      keyVec.push_back(it->first);
   }
   return keyVec;
}

