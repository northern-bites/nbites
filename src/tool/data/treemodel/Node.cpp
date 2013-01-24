/*
 * Node.cpp
 *
 *  Created on: Apr 2, 2011
 *      Author: oneamtu
 */

#include "Node.h"
#include <assert.h>

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

void Node::addChild(Node * child) {
    assert(child != NULL);
    this->children.push_back(child);
}

void Node::popChild() {
    this->children.pop_back();
}

const Node* Node::getChild(int index) const {
    assert(0 <= index && index < children.size());
    return children.at(index);
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
    //const casting is BAD but I think there is some bug at work here
    //the QT API says indexOf takes const T& as the first paramenter,
    //yet gcc still tries converting child from const Node* to Node*
    return children.indexOf(const_cast<Node*>(child));
}

}
}
}
