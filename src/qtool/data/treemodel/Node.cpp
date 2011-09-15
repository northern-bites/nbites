/*
 * Node.cpp
 *
 *  Created on: Apr 2, 2011
 *      Author: oneamtu
 */

#include "Node.h"

namespace qtool {
namespace data {
namespace treemodel {

typedef TreeModel::Node Node;

Node::Node(const Node* _parent) : parent(_parent){
}

Node::~Node() {
	clearChildren();
}

void Node::clearChildren() {
	for (int i = 0; i < childCount(); i++ ) {
		delete children.at(i);
	}
	children.clear();
}

void Node::addChild(const Node * child) {
    this->children.push_back(child);
}

const Node* Node::getChild(int index) const {
    if (0 <= index && index < childCount()) {
        return children.at(index);
    } else {
    	return NULL;
    }
}

int Node::childCount() const {
    return children.size();
}

const Node* Node::getParent() const {
    return parent;
}

int Node::indexInParentList() const {
    return parent->indexOfChild(this);
}

int Node::indexOfChild(const Node* child) const {
    return children.indexOf(child);
}

}
}
}
