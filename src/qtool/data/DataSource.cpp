
#include "DataSource.h"

#include <algorithm> //for std::transform
#include <boost/bind.hpp>
#include <iostream>

namespace qtool {
namespace data{

using namespace man::include::io;
using namespace std;

DataSource::DataSource(Type type) : type(type){

}

vector<int> DataSource::getFileDescriptors() {
    vector<int> vec;
    std::transform( fdProviderMap.begin(), fdProviderMap.end(),
                    std::back_inserter(vec),
                    boost::bind(&FDProviderMap::value_type::first, _1) );
    return vec;
}

void DataSource::addProvider(FDProvider::ptr fdprovider) {
    fdProviderMap.insert(
            FDProviderPair(fdprovider->getFileDescriptor(),
                           fdprovider));
}

}
}
