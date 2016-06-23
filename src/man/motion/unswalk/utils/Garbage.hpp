template<int size, typename Type>
struct Garbage {
   Type garbage[size];
};

namespace boost {
   namespace serialization {

      template<class Archive, int size, typename Type>
      void serialize(Archive & ar, Garbage<size, Type> & g, const unsigned int version)
      {
         for (int element = 0; element < size; ++element)
            ar & g.garbage[element];
      }

   } // namespace serialization
} // namespace boost

