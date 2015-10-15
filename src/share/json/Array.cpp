#include "Array.h"

namespace json {
    
    const std::string Array::serialize() const {
        std::string builder = "[";
        for (int i = 0; i < size(); ++i) {
            builder += at(i)->serialize();
            if (i < (size() - 1))
                builder += ",";
        }
        
        builder += "]";
        
        return builder;
    }
    
    const std::string Array::printi(int indent) const {
        std::string builder = indentStr(indent) + "[\n";
        for (int i = 0; i < size(); ++i) {
            builder += indentStr(indent);
            
            JsonValue * val = this->at(i);
            builder += val->printi(indent + 1);
            
            if (i < (size() - 1))
                builder += ",";
            
            builder += "\n";
        }
        
        builder += indentStr(indent) + "]\n";
        
        return builder;
    }
    
}