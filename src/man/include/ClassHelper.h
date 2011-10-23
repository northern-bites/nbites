/**
 * ClassHelper.h
 *
 * Useful macros when declaring classes
 *
 * @author Octavian Neamtu
 */

#include <boost/shared_ptr.hpp>

#define ADD_SHARED_PTR(class_name) \
public:\
	typedef boost::shared_ptr<class_name> ptr;\
	typedef boost::shared_ptr<const class_name> const_ptr;\
