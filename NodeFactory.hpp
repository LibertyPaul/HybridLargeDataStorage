#ifndef NODEFACTORY_HPP
#define NODEFACTORY_HPP

#include "BaseNode.hpp"
#include "Node.hpp"
#include "ValueNode.hpp"


template<typename Key, typename Value>
class NodeFactory{
public:
	static BaseNode<Key, Value> *createNode(const Key &key, const Value &value){
		if(key.empty()){
			return new ValueNode<Key, Value>(value);
		}
		else{
			return new Node<Key, Value>();
		}
	}
};

#endif // NODEFACTORY_HPP
