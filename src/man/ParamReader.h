#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>

namespace man{

class ParamReader
{
public:
    ParamReader(std::string fileName);

    template <class T>
    T getParam(std::string param){
        return tree.get<T>(param);
    }
    bool isEmpty(){
    	return tree.empty();
    }

private:
    boost::property_tree::ptree tree;
};

} // man
