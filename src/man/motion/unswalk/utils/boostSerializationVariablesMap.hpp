#pragma once

#include <boost/program_options/variables_map.hpp>
#include <boost/serialization/map.hpp>

#include <iostream>

namespace boost {
   namespace serialization {
      template<class Archive>
      inline void save_construct_data(Archive & ar,
                                      const boost::program_options::variable_value* t,
                                      const unsigned int file_version) {
         std::cerr << __PRETTY_FUNCTION__ << std::endl;
         // save data required to construct instance
         ar << t->defaulted();
         ar << t->value();
      }

      template<class Archive>
      inline void load_construct_data(Archive & ar,
                                      boost::program_options::variable_value* t,
                                      const unsigned int file_version) {
         std::cerr << __PRETTY_FUNCTION__ << std::endl;
         // retrieve data from archive required to construct new instance
         bool defaulted;
         boost::any value;
         ar >> defaulted;
         ar >> value;
         std::cerr << value.type().name();
         // invoke inplace constructor to initialize instance of my_class
         ::new (t) boost::program_options::variable_value(value, defaulted);
      }

      template<class Archive>
      inline void save(Archive & ar,
                       const boost::program_options::variable_value &t,
                       const unsigned int /* file_version */) {
         ar & t.value(); // TODO(jayen): figure out how to serialize boost::any
         ar & t.defaulted();
      }
      template<class Archive>
      inline void load(Archive & ar,
                       boost::program_options::variable_value &t,
                       const unsigned int /* file_version */) {
         boost::any v;
         ar & v;
         bool defaulted;
         ar & defaulted;
         boost::program_options::variable_value vv(v, defaulted);
         memcpy(&t, &vv, sizeof(vv));
      }
      template<class Archive>
      inline void serialize(Archive & ar,
                            boost::program_options::variable_value &t,
                            const unsigned int file_version) {
         // we should have nothing to do, as it's all in the constructor, but
         // boost serialization of pairs uses the default constructor and not
         // the above stuff.
         boost::serialization::split_free(ar, t, file_version);
      }

      // just use the superclass's serialize function
      template<class Archive>
      inline void serialize(Archive & ar,
                            boost::program_options::variables_map &t,
                            const unsigned int file_version) {
         serialize(ar, (std::map<std::string,
                                 boost::program_options::variable_value> &)t,
                   file_version);
      }
   }  // serialization
}  // namespace boost
